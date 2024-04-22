/** @file scaled_unit2.hpp
 *
 *  Author: Roland Conybeare
 **/

#pragma once

#include "natural_unit.hpp"
//#include <cstdint>

namespace xo {
    namespace qty {
        /** @class bpu2_array_rescale_result
         *  @brief Represents the product sqrt(outer_scale_sq) * outer_scale_exact * nat_unit
         **/
        template <typename Int>
        struct scaled_unit2 {
            constexpr scaled_unit2(const natural_unit<Int> & nat_unit,
                                   ratio::ratio<Int> outer_scale_exact,
                                   double outer_scale_sq)
                : natural_unit_{nat_unit},
                  outer_scale_exact_{outer_scale_exact},
                  outer_scale_sq_{outer_scale_sq}
                {}

            constexpr scaled_unit2 reciprocal() const {
                return scaled_unit2(nu_reciprocal(natural_unit_,
                                                  outer_scale_exact_.reciprocal(),
                                                  1.0 / outer_scale_sq_));
            }

            natural_unit<Int> natural_unit_;
            ratio::ratio<Int> outer_scale_exact_;
            double outer_scale_sq_;
        };

        namespace detail {
            template <typename Int>
            constexpr auto make_unit_rescale_result(const natural_unit<Int> & bpuv) {
                return scaled_unit2<Int>(bpuv,
                                                      ratio::ratio<Int>(1, 1),
                                                      1.0);
            }
        }

        namespace su2 {
            constexpr auto nanogram = detail::make_unit_rescale_result<std::int64_t>(nu2::nanogram);
            constexpr auto microgram = detail::make_unit_rescale_result<std::int64_t>(nu2::microgram);
        }

        namespace detail {
            template <typename Int>
            constexpr
            detail::bpu2_rescale_result<Int>
            bpu2_product(const bpu2<Int> & lhs_bpu,
                         const bpu2<Int> & rhs_bpu)
            {
                assert(lhs_bpu.native_dim() == rhs_bpu.native_dim());

                bpu2<Int> prod_bpu = lhs_bpu;
                auto rr = bpu_product_inplace(&prod_bpu, rhs_bpu);

                return bpu2_rescale_result<Int>(prod_bpu, rr.outer_scale_exact_, rr.outer_scale_sq_);
            }

            template <typename Int>
            constexpr
            scaled_unit2<Int>
            nu_product(const natural_unit<Int> & lhs_bpu_array,
                       const bpu2<Int> & rhs_bpu)
            {
                natural_unit<Int> prod = lhs_bpu_array;
                auto rr = bpu_array_product_inplace(&prod, rhs_bpu);

                return scaled_unit2<Int>(prod,
                                                      rr.outer_scale_exact_,
                                                      rr.outer_scale_sq_);
            };

            template <typename Int>
            constexpr
            scaled_unit2<Int>
            nu_product(const natural_unit<Int> & lhs_bpu_array,
                       const natural_unit<Int> & rhs_bpu_array)
            {
                natural_unit<Int> prod = lhs_bpu_array;

                /* accumulate product of scalefactors spun off by rescaling
                 * any basis-units in rhs_bpu_array that conflict with the same dimension
                 * in lh_bpu_array
                 */
                auto sfr = (detail::outer_scalefactor_result<Int>
                            (scalefactor_ratio_type(1, 1) /*outer_scale_exact*/,
                             1.0 /*outer_scale_sq*/));

                for (std::size_t i = 0; i < rhs_bpu_array.n_bpu(); ++i) {
                    auto sfr2 = bpu_array_product_inplace(&prod, rhs_bpu_array[i]);

                    sfr.outer_scale_exact_ = sfr.outer_scale_exact_ * sfr2.outer_scale_exact_;
                    sfr.outer_scale_sq_ *= sfr2.outer_scale_sq_;
                }

                return scaled_unit2<Int>(prod,
                                         sfr.outer_scale_exact_,
                                         sfr.outer_scale_sq_);
            }

        }

        template <typename Int>
        inline constexpr scaled_unit2<Int>
        operator* (const scaled_unit2<Int> & x_unit,
                   const scaled_unit2<Int> & y_unit)
        {
            auto rr = detail::nu_product(x_unit.natural_unit_,
                                                y_unit.natural_unit_);

            return (scaled_unit2<Int>
                    (rr.natural_unit_,
                     rr.outer_scale_exact_ * x_unit.outer_scale_exact_ * y_unit.outer_scale_exact_,
                     rr.outer_scale_sq_ * x_unit.outer_scale_sq_ * y_unit.outer_scale_sq_));
        }
    } /*namespace qty*/
} /*namespace xo*/

/** end scaled_unit2.hpp **/
