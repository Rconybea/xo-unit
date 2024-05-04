/** @file quantity.hpp
 *
 *  Author: Roland Conybeare
 **/

#pragma once

#include "quantity_ops.hpp"
#include "natural_unit.hpp"
#include "scaled_unit.hpp"

namespace xo {
    namespace qty {
        /** @class quantity
         *  @brief represent a scalar quantity with associated units.
         *
         *  Enforce dimensional consistency at compile time.
         *  sizeof(quantity) == sizeof(Repr).
         **/
        template <
            typename Repr = double,
            typename Int = std::int64_t,
            natural_unit<Int> NaturalUnit = natural_unit<Int>(),
            typename Int2x = detail::width2x_t<Int> >
        class quantity {
        public:
            using repr_type = Repr;
            using unit_type = natural_unit<Int>;
            using ratio_int_type = Int;
            using ratio_int2x_type = Int2x;

        public:
            constexpr quantity() : scale_{0} {}
            explicit constexpr quantity(Repr scale) : scale_{scale} {}

            static constexpr bool always_constexpr_unit = true;

            constexpr const repr_type & scale() const { return scale_; }
            constexpr const unit_type & unit() const { return s_unit; }

            // is_dimensionless

            // unit_qty
            // zero_qty
            // reciprocal

            /* parallel implementation to Quantity<Repr, Int>::rescale(),
             * except that NaturalUnit2 is a compile-time-only template-argument
             *
             * NOTE: constexpr as long as no fractional units involved.
             */
            template <natural_unit<Int> NaturalUnit2>
            constexpr
            auto rescale() const {
                /* conversion factor from .unit -> unit2*/
                auto rr = detail::su_ratio<ratio_int_type,
                                           ratio_int2x_type>(NaturalUnit, NaturalUnit2);

                if (rr.natural_unit_.is_dimensionless()) {
                    repr_type r_scale = (((rr.outer_scale_sq_ == 1.0)
                                          ? 1.0
                                          : ::sqrt(rr.outer_scale_sq_))
                                         * rr.outer_scale_factor_.template convert_to<repr_type>()
                                         * this->scale_);
                    return quantity<Repr, Int, NaturalUnit2, Int2x>(r_scale);
                } else {
                    return quantity<Repr, Int, NaturalUnit2, Int2x>(std::numeric_limits<repr_type>::quiet_NaN());
                }
            }

            template <scaled_unit<Int> ScaledUnit2>
            constexpr
            auto rescale() const {
                /* conversion factor from .unit -> unit2*/
                auto rr = detail::su_ratio<ratio_int_type,
                                           ratio_int2x_type>(NaturalUnit, ScaledUnit2.natural_unit_);

                if (rr.natural_unit_.is_dimensionless()) {
                    /* NOTE: test for unit .outer_scale_sq values to get constexpr result with c++23
                     *       and integer dimension powers.
                     */
                    repr_type r_scale = ((((rr.outer_scale_sq_ == 1.0)
                                           && (ScaledUnit2.outer_scale_sq_ == 1.0))
                                          ? 1.0
                                          : ::sqrt(rr.outer_scale_sq_ / ScaledUnit2.outer_scale_sq_))
                                         * rr.outer_scale_factor_.template convert_to<repr_type>()
                                         * this->scale_
                                         / ScaledUnit2.outer_scale_factor_.template convert_to<repr_type>());
                    return quantity<Repr, Int, ScaledUnit2.natural_unit_, Int2x>(r_scale);
                } else {
                    return quantity<Repr, Int, ScaledUnit2.natural_unit_, Int2x>(std::numeric_limits<repr_type>::quiet_NaN());
                }
            }

            template <typename Dimensionless>
            requires std::is_arithmetic_v<Dimensionless>
            constexpr auto scale_by(Dimensionless x) const {
                return quantity(x * this->scale_);
            }

            // divide_by
            // divide_into

            // divide
            // add
            // subtract

            /* parallel implementation to Quantity<Repr, Int> */
            template <typename Quantity2>
            static constexpr
            auto compare(const quantity &x, const Quantity2 & y) {
                quantity y2 = y.template rescale<s_unit>();

                return x.scale() <=> y2.scale();
            }

            // operator-
            // operator+=
            // operator-=
            // operator*=
            // operator/=

            constexpr nu_abbrev_type abbrev() const { return s_unit.abbrev(); }

            quantity & operator=(const quantity & x) {
                this->scale_ = x.scale_;
                return *this;
            }

            template <typename Q2>
            requires(quantity_concept<Q2>
                     && Q2::always_constexpr_unit)
            quantity & operator=(const Q2 & x) {
                auto x2 = x.template rescale<s_unit>();

                this->scale_ = x2.scale();

                return *this;
            }

            template <typename Q2>
            requires(quantity_concept<Q2>
                     && Q2::always_constexpr_unit)
            constexpr operator Q2() const {
                return this->template rescale<Q2::s_unit>();
            }

        public: /* need public members so that a quantity instance can be a non-type template parameter (is a structural type) */
            static constexpr natural_unit<Int> s_unit = NaturalUnit;

            Repr scale_ = Repr{};
        };

        template <natural_unit<::std::int64_t> NaturalUnit = natural_unit<::std::int64_t>()>
        using stdquantity = xo::qty::quantity<double, ::std::int64_t, NaturalUnit>;

        template <typename Quantity, typename Int, typename Int2x>
        constexpr auto
        rescale(const Quantity & x, const scaled_unit<Int, Int2x> & su) {
            return x.template rescale<su>();
        }

        namespace detail {
            struct quantity_util {
                /* parallel implementation to Quantity<Repr, Int> multiply,
                 * but return type will have dimension computed at compile-time
                 */
                template <typename Q1, typename Q2>
                requires (quantity_concept<Q1>
                          && quantity_concept<Q2>
                          && Q1::always_constexpr_unit
                          && Q2::always_constexpr_unit)
                static constexpr auto multiply(Q1 x, Q2 y) {
                    using r_repr_type = std::common_type_t<typename Q1::repr_type,
                                                           typename Q2::repr_type>;
                    using r_int_type = std::common_type_t<typename Q1::ratio_int_type,
                                                          typename Q2::ratio_int_type>;
                    using r_int2x_type = std::common_type_t<typename Q1::ratio_int2x_type,
                                                            typename Q2::ratio_int2x_type>;

                    constexpr auto rr = detail::su_product<r_int_type, r_int2x_type>(x.unit(), y.unit());

                    r_repr_type r_scale = (((rr.outer_scale_sq_ == 1.0)
                                            ? 1.0
                                            : ::sqrt(rr.outer_scale_sq_))
                                           * rr.outer_scale_factor_.template convert_to<r_repr_type>()
                                           * static_cast<r_repr_type>(x.scale())
                                           * static_cast<r_repr_type>(y.scale()));

                    return quantity<r_repr_type,
                                    r_int_type,
                                    rr.natural_unit_,
                                    r_int2x_type
                                    >(r_scale);
                }

                template <typename Q1, typename Q2>
                requires (quantity_concept<Q1>
                          && quantity_concept<Q2>
                          && Q1::always_constexpr_unit
                          && Q2::always_constexpr_unit)
                static constexpr auto divide(Q1 x, Q2 y) {
                    using r_repr_type = std::common_type_t<typename Q1::repr_type,
                                                           typename Q2::repr_type>;
                    using r_int_type = std::common_type_t<typename Q1::ratio_int_type,
                                                          typename Q2::ratio_int_type>;
                    using r_int2x_type = std::common_type_t<typename Q1::ratio_int2x_type,
                                                            typename Q2::ratio_int2x_type>;

                    constexpr auto rr = detail::su_ratio<r_int_type, r_int2x_type>(x.unit(), y.unit());

                    r_repr_type r_scale = (((rr.outer_scale_sq_ == 1.0)
                                            ? 1.0
                                            : ::sqrt(rr.outer_scale_sq_))
                                           * rr.outer_scale_factor_.template convert_to<r_repr_type>()
                                           * static_cast<r_repr_type>(x.scale())
                                           / static_cast<r_repr_type>(y.scale()));

                    return quantity<r_repr_type,
                                    r_int_type,
                                    rr.natural_unit_,
                                    r_int2x_type
                                    >(r_scale);
                }
            };
        } /*namespace detail*/

        template <typename Q1, typename Q2, typename Int = typename Q2::ratio_int_type, natural_unit<Int> Unit = Q2::s_unit>
        requires (quantity_concept<Q1>
                  && quantity_concept<Q2>
                  && Q1::always_constexpr_unit
                  && Q2::always_constexpr_unit)
        constexpr auto
        with_units(const Q1 & x, const Q2 & y)
        {
            return x.template rescale<Unit>();
        }

        /** note: won't have constexpr result w/ fractional dimension until c++26 (when ::sqrt(), ::pow() are constexpr)
         **/
        template <typename Q1, typename Q2>
        requires (quantity_concept<Q1>
                  && quantity_concept<Q2>
                  && Q1::always_constexpr_unit
                  && Q2::always_constexpr_unit)
        constexpr auto
        operator* (const Q1 & x, const Q2 & y)
        {
            return detail::quantity_util::multiply(x, y);
        }

        /** note: won't have constexpr result w/ fractional dimension until c++26 (when ::sqrt(), ::pow() are constexpr)
         **/
        template <typename Q1, typename Q2>
        requires (quantity_concept<Q1>
                  && quantity_concept<Q2>
                  && Q1::always_constexpr_unit
                  && Q2::always_constexpr_unit)
        constexpr auto
        operator/ (const Q1 & x, const Q2 & y)
        {
            return detail::quantity_util::divide(x, y);
        }

        namespace qty {
            // ----- mass -----

            inline constexpr auto picograms(double x) { return quantity<double, std::int64_t, nu::picogram>(x); }
            inline constexpr auto nanograms(double x) { return quantity<double, std::int64_t, nu::nanogram>(x); }
            inline constexpr auto micrograms(double x) { return quantity<double, std::int64_t, nu::microgram>(x); }
            inline constexpr auto milligrams(double x) { return quantity<double, std::int64_t, nu::milligram>(x); }
            inline constexpr auto grams(double x) { return quantity<double, std::int64_t, nu::gram>(x); }
            inline constexpr auto kilograms(double x) { return quantity<double, std::int64_t, nu::kilogram>(x); }
            inline constexpr auto tonnes(double x) { return quantity<double, std::int64_t, nu::tonne>(x); }
            inline constexpr auto kilotonnes(double x) { return quantity<double, std::int64_t, nu::kilotonne>(x); }
            inline constexpr auto megatonnes(double x) { return quantity<double, std::int64_t, nu::megatonne>(x); }
            inline constexpr auto gigatonnes(double x) { return quantity<double, std::int64_t, nu::gigatonne>(x); }

            // ----- distance -----

            inline constexpr auto picometers(double x) { return quantity<double, std::int64_t, nu::picometer>(x); }
            inline constexpr auto nanometers(double x) { return quantity<double, std::int64_t, nu::nanometer>(x); }
            inline constexpr auto micrometers(double x) { return quantity<double, std::int64_t, nu::micrometer>(x); }
            inline constexpr auto millimeters(double x) { return quantity<double, std::int64_t, nu::millimeter>(x); }
            inline constexpr auto meters(double x) { return quantity<double, std::int64_t, nu::meter>(x); }
            inline constexpr auto kilometers(double x) { return quantity<double, std::int64_t, nu::kilometer>(x); }
            inline constexpr auto megameters(double x) { return quantity<double, std::int64_t, nu::megameter>(x); }
            inline constexpr auto gigameters(double x) { return quantity<double, std::int64_t, nu::gigameter>(x); }

            inline constexpr auto lightseconds(double x) { return quantity<double, std::int64_t, nu::lightsecond>(x); }
            inline constexpr auto astronomicalunits(double x) { return quantity<double, std::int64_t, nu::astronomicalunit>(x); }

            static constexpr auto meter = meters(1);

            // ----- time -----

            inline constexpr auto picoseconds(double x) { return quantity<double, std::int64_t, nu::picosecond>(x); }
            inline constexpr auto nanoseconds(double x) { return quantity<double, std::int64_t, nu::nanosecond>(x); }
            inline constexpr auto microseconds(double x) { return quantity<double, std::int64_t, nu::microsecond>(x); }
            inline constexpr auto milliseconds(double x) { return quantity<double, std::int64_t, nu::millisecond>(x); }
            inline constexpr auto seconds(double x) { return quantity<double, std::int64_t, nu::second>(x); }
            inline constexpr auto minutes(double x) { return quantity<double, std::int64_t, nu::minute>(x); }
            inline constexpr auto hours(double x) { return quantity<double, std::int64_t, nu::hour>(x); }
            inline constexpr auto days(double x) { return quantity<double, std::int64_t, nu::day>(x); }
            inline constexpr auto weeks(double x) { return quantity<double, std::int64_t, nu::week>(x); }
            inline constexpr auto months(double x) { return quantity<double, std::int64_t, nu::month>(x); }
            inline constexpr auto years(double x) { return quantity<double, std::int64_t, nu::year>(x); }
            inline constexpr auto year250s(double x) { return quantity<double, std::int64_t, nu::year250>(x); }
            inline constexpr auto year360s(double x) { return quantity<double, std::int64_t, nu::year360>(x); }
            inline constexpr auto year365s(double x) { return quantity<double, std::int64_t, nu::year365>(x); }
            //inline constexpr auto year366s(double x) { return quantity<double, std::int64_t, nu::year366>(x); }

            static constexpr auto second = seconds(1);

            // ----- volatility -----

            /* volatility in units of 1/yr */
            inline constexpr auto volatility_250d(double x) { return quantity<double, std::int64_t, nu::volatility_250d>(x); }
            inline constexpr auto volatility_360d(double x) { return quantity<double, std::int64_t, nu::volatility_360d>(x); }
        }

        /* reminder: see [quantity_ops.hpp] for operator* etc */
    } /*namespace qty*/
} /*namespace xo*/

/** end quantity.hpp **/
