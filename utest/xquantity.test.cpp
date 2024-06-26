/* @file xquantity.test.cpp */

#include "xquantity.hpp"
#include "xquantity_iostream.hpp"
#include "xo/randomgen/random_seed.hpp"
#include "xo/randomgen/xoshiro256.hpp"
#include "xo/indentlog/scope.hpp"
#include "xo/indentlog/print/tag.hpp"
#include <catch2/catch.hpp>
#include <set>
#include <vector>
#include <string>

namespace xo {
    namespace u = xo::qty::u;
    namespace nu = xo::qty::nu;

    using xo::qty::xquantity;
    using xo::qty::natural_unit;
    using xo::qty::power_ratio_type;
    using xo::qty::scalefactor_ratio_type;
    using xo::qty::dim;
    using xo::qty::n_dim;

    using xo::rng::xoshiro256ss;

    using std::vector;
    using std::int64_t;
    using std::size_t;

    namespace ut {
        std::string
        int128_to_string(__int128_t x) {
            size_t p = 256;
            char buf[256];

            buf[--p] = '\0';

            bool minus_flag = (x < 0);

            if (minus_flag)
                x = -x;

            while (p > 1) {
                if (x == 0)
                    break;

                __int128_t x1 = x/10;

                auto digit = (x - 10*x1);  /* not sure if % works on __int128_t */

                buf[--p] = '0' + digit;

                x = x1;
            }

            if (minus_flag)
                buf[--p] = '-';

            return std::string(buf + p);
        }

#ifdef NOT_USING
        /* use Int2x to accumulate scalefactor */
        template <typename Int, typename Int2x>
        scaled_unit<Int>
        nu_ratio_debug(const natural_unit<Int> & nu_lhs,
                       const natural_unit<Int> & nu_rhs)
        {
            XO_SCOPE(log, always);

            natural_unit<Int2x> ratio = nu_lhs.template to_repr<Int2x>();

            /* accumulate product of scalefactors spun off by rescaling
             * any basis-units in rhs_bpu_array that conflict with the same dimension
             * in lh_bpu_array
             */
            auto sfr = (xo::qty::detail::outer_scalefactor_result<Int2x>
                        (ratio::ratio<Int2x>(1, 1) /*outer_scale_exact*/,
                         1.0 /*outer_scale_sq*/));

            for (std::size_t i = 0; i < nu_rhs.n_bpu(); ++i) {
                log && log(xtag("i", i));
                log && log(xtag("ratio[before]", ratio));

                auto sfr2 = xo::qty::detail::nu_ratio_inplace(&ratio, nu_rhs[i].template to_repr<Int2x>());

                log && log(xtag("nu_rhs[i]", nu_rhs[i]));
                log && log(xtag("sfr2.outer_scale_exact.num", int128_to_string(sfr2.outer_scale_exact_.num())));
                log && log(xtag("sfr2.outer_scale_exact.den", int128_to_string(sfr2.outer_scale_exact_.den())));
                log && log(xtag("sfr2.outer_scale_sq", sfr2.outer_scale_sq_));

                /* note: nu_ratio_inplace() reports multiplicative outer scaling factors,
                 *       so multiply is correct here
                 */
                sfr.outer_scale_exact_ = sfr.outer_scale_exact_ * sfr2.outer_scale_exact_;
                sfr.outer_scale_sq_ *= sfr2.outer_scale_sq_;

                log && log(xtag("sfr.outer_scale_exact.num", int128_to_string(sfr.outer_scale_exact_.num())));
                log && log(xtag("sfr.outer_scale_exact.den", int128_to_string(sfr.outer_scale_exact_.den())));
            }

            log && log(xtag("ratio[after]", ratio));

            return scaled_unit<Int>(ratio.template to_repr<Int>(),
                                    sfr.outer_scale_exact_,
                                    sfr.outer_scale_sq_);
        }
#endif

        vector<natural_unit<int64_t>> mass_unit_v
        = { nu::picogram, nu::nanogram, nu::microgram, nu::milligram,
            nu::gram,
            nu::kilogram, nu::tonne, nu::kilotonne, nu::megatonne };

        vector<natural_unit<int64_t>> distance_unit_v
        = { nu::picometer, nu::nanometer, nu::micrometer, nu::millimeter, nu::meter,
            nu::kilometer, nu::megameter, nu::gigameter, nu::lightsecond, nu::astronomicalunit };

        vector<natural_unit<int64_t>> time_unit_v
        = { nu::picosecond, nu::nanosecond, nu::microsecond, nu::millisecond,
            nu::second, nu::minute, nu::hour, nu::day, nu::week, nu::month, nu::year,
            nu::year250, nu::year360, nu::year365 };

        vector<natural_unit<int64_t>> currency_unit_v
        = { nu::currency };

        vector<natural_unit<int64_t>> price_unit_v
        = { nu::price };

        vector<vector<natural_unit<int64_t>> *> all_unit_v = {
            &mass_unit_v, &distance_unit_v, &time_unit_v, &currency_unit_v, &price_unit_v
        };

        template <typename Rng>
        void
        quantity_tests(bool debug_flag, Rng & rng)
        {
            REQUIRE(all_unit_v.size() == n_dim);

            /* max number of basis_units to combine.  don't combine a unit more than once
             * (because can have too-extreme scaling differences)
             */
            std::size_t n_bu = 5;
            /* number of combinations to consider within each number up to n_bu */
            std::size_t n_experiment = 10;

            for (size_t nu=1; nu<=n_bu; ++nu) {
                /* will combine nu basis units */

                for (size_t i=0; i<n_experiment; ++i) {
                    scope log1(XO_DEBUG(debug_flag));

                    INFO(tostr(XTAG(nu), XTAG(i)));

                    /* choose which dimensions to use */
                    std::set<xo::qty::dim> dim_set;

                    for (size_t j=0; j<nu; ++j)
                        dim_set.insert(static_cast<xo::qty::dim>(rng() % nu));;

                    /* construct a pair of random product units with the same dimension;
                     * track relative scale as we go
                     */

                    xquantity q1 = natural_unit_qty(nu::dimensionless);
                    xquantity q2 = natural_unit_qty(nu::dimensionless);

                    static_assert(std::same_as<decltype(q1)::ratio_int_type, std::int64_t>);
                    static_assert(std::same_as<decltype(q1)::ratio_int2x_type, __int128_t>);

                    double k1 = 0.0; /*q1/q2*/
                    double k2 = 0.0; /*q2/q1*/
                    {
                        xquantity q12 = (q1/q2);
                        xquantity q21 = (q2/q1);

                        REQUIRE(q12.is_dimensionless());
                        REQUIRE(q21.is_dimensionless());

                        k2 = q12.scale();
                        k1 = q21.scale();
                    }

                    REQUIRE(k1 == 1.0);
                    REQUIRE(k2 == 1.0);

                    /* inv:
                     * - q2 = q1*k1
                     * - q2*k2 = q1
                     */

                    /* Editorial: it's easy to produce units for which scaling requires working
                     * with rationals that have >128bits (ask me how I know).
                     *
                     * e.g. kilotonnes / nanograms is already 10^18
                     *
                     * and 2^128 = (2^12)^10 * 2^8 ~ (10^3)^10 * 256 ~ 10^32
                     *
                     * In below we cap magnitude differences at this much per basis unit
                     * Actual cap for q1/q2 is n_bu * max_magnitude
                     */
                    constexpr double max_magdiff_per_bu = 1.1e5;

                    for (xo::qty::dim d : dim_set) {
                        scope log(XO_DEBUG(debug_flag));

                        size_t d_j = static_cast<uint32_t>(d);

                        const auto * p_nu_v = all_unit_v[d_j];
                        /* pick a random unit for selected dimension */
                        auto nu1_j = (*p_nu_v)[rng() % p_nu_v->size()];

                        REQUIRE(nu1_j.n_bpu() == 1);

                        int power = 1 + (rng() % 2); /* power in {1, 2} */
                        if (rng() % 2)
                            power = -power; /* power in +/- {1,2} */

                        if (power == -1)
                            nu1_j = nu1_j.reciprocal();

                        xquantity q1_j = natural_unit_qty<double, int64_t>(nu1_j);
                        xquantity q2_j = q1_j;
                        xquantity r1;
                        xquantity r2;

                        auto nu2_j = nu1_j;
                        auto nu2_j_ix = rng() % p_nu_v->size();

                        for (;;) {
                            REQUIRE(nu2_j_ix < p_nu_v->size());

                            nu2_j = (*p_nu_v)[nu2_j_ix];

                            if (power == -1)
                                nu2_j = nu2_j.reciprocal();

                            REQUIRE(nu2_j.n_bpu() == 1);

                            double rx = (nu1_j[0].scalefactor().template convert_to<double>()
                                         / nu2_j[0].scalefactor().template convert_to<double>());

                            if ((rx > max_magdiff_per_bu) || (rx < 1.0/max_magdiff_per_bu)) {
                                log && log(xtag("nu_z", p_nu_v->size()), xtag("nu2_j_ix", nu2_j_ix));
                                log && log(xtag("nu1_j", nu1_j));
                                log && log(xtag("nu2_j", nu2_j));
                                log && log("rejecting ", xtag("rx", rx));

                                /* try another value for nu2_j */
                                if (rx > max_magdiff_per_bu) {
                                    /* try a larger value for nu2_j_ix */
                                    ++nu2_j_ix;
                                } else {
                                    /* try a smaller value for nu2_j_ix */
                                    --nu2_j_ix;
                                }

                                continue;
                            }

                            q2_j = natural_unit_qty<double, int64_t>(nu2_j);

                            r1 = q1_j / q2_j;
                            r2 = q2_j / q1_j;

                            REQUIRE(r1.is_dimensionless());
                            REQUIRE(r2.is_dimensionless());

                            break;
                        }

                        q1 *= q1_j;
                        q2 *= q2_j;

                        k1 *= r1.scale();
                        k2 *= r2.scale();

                        log && log(xtag("d", xo::qty::dim2str(d)));
                        log && log(xtag("nu1_j", nu1_j));
                        log && log(xtag("nu2_j", nu2_j));
                        log && log(xtag("r1=q1_j/q2_j", r1.scale()));
                        log && log(xtag("r2=q2_j/q1_j", r2.scale()));
                        log && log(xtag("k1", k1));
                        log && log(xtag("k2", k2));
                        log && log(xtag("q1", q1));
                        log && log(xtag("q2", q2));
                    }

                    INFO(xtag("k1=q1/q2", k1));
                    INFO(XTAG(q1));
                    INFO(xtag("k2=q2/q1", k2));
                    INFO(XTAG(q2));

                    /* q1/q2, with exact representation (given no fractional dimensions)
                     *
                     */
                    auto su = xo::qty::detail::su_ratio<decltype(q1)::ratio_int_type,
                                                        decltype(q1)::ratio_int2x_type,
                                                        double>
                        (q1.unit(), q2.unit());

                    INFO(xtag("su.natural_unit", su.natural_unit_));
                    INFO(xtag("su.outer_scale_exact", su.outer_scale_factor_));
                    INFO(xtag("su.outer_scale_sq", su.outer_scale_sq_));

                    REQUIRE(q1 == q1);
                    REQUIRE(q2 == q2);
                    REQUIRE(su.natural_unit_.is_dimensionless());
                    REQUIRE(su.outer_scale_sq_ == 1.0);

                    /* these will only approximately be true in general */
                    REQUIRE((q1/q2).scale() == Approx(k1).epsilon(1.0e-6));
                    REQUIRE((q2/q1).scale() == Approx(k2).epsilon(1.0e-6));

                    if (abs(k1 - 1.0) > 1.0e-6) {
                        REQUIRE(q1 != q2);
                        REQUIRE(q2 != q1);

                        if (k1 > 1.0) {
                            REQUIRE(q1 >= q2);
                            REQUIRE(q1 > q2);

                            REQUIRE(q2 < q1);
                            REQUIRE(q2 <= q1);
                        } else {
                            REQUIRE(q1 <= q2);
                            REQUIRE(q1 < q2);

                            REQUIRE(q2 > q1);
                            REQUIRE(q2 >= q1);
                        }
                    }

                    auto q1_plus_q2 = q1 + q2;
                    /* k1 = q1/q2;  k2 = q2/q1; so q1+q2 = q1(1 + q2/q1) = q1(1 + k2) */
                    REQUIRE(q1_plus_q2.unit() == q1.unit());
                    REQUIRE(q1_plus_q2.scale() == Approx(1.0 + k2).epsilon(1.0e-6));

                    auto q1_minus_q2 = q1 - q2;
                    REQUIRE(q1_minus_q2.unit() == q1.unit());
                    REQUIRE(q1_minus_q2.scale() == Approx(1.0 - k2).epsilon(1.0e-6));

                    auto q1_neg = -q1;
                    REQUIRE(q1_neg.unit() == q1.unit());
                    REQUIRE(q1_neg.scale() == -q1.scale());

                    auto q1_mult_k2 = q1 * k2;
                    auto k2_mult_q1 = k2 * q1;
                    REQUIRE(q1_mult_k2.unit() == q1.unit());
                    REQUIRE(k2_mult_q1.unit() == q1.unit());
                    REQUIRE(q1_mult_k2 == k2_mult_q1);

                    /* expect q1*k2 ~ q2,  but may not be exact */
                    {
                        auto s = q1_mult_k2 / q2;

                        REQUIRE(s.is_dimensionless());
                        REQUIRE(s.scale() == Approx(1.0).epsilon(1.0e-6));
                    }
                }
            }
        }

        /* Strategy:
         * 1. start with a set of basis units in each dimension.
         * 2. verify +,- by combining quantities with different units
         */
        TEST_CASE("xquantity.full", "[xquantity.full]") {
            constexpr bool c_debug_flag = false;

            scope log(XO_DEBUG2(c_debug_flag, "TEST_CASE.xquantity.full"));

            // can get bits instead from /dev/random by uncommenting the line below in place of 2nd line
            //rng::Seed<xoshiro256ss> seed;
            uint64_t seed = 7032458451101515502;

            log && log(tag("seed", seed));

            auto rng = xoshiro256ss(seed);

            quantity_tests(c_debug_flag, rng);
        } /*TEST_CASE(xquantity.full)*/

        TEST_CASE("xquantity", "[xquantity]") {
            constexpr bool c_debug_flag = false;

            scope log(XO_DEBUG2(c_debug_flag, "TEST_CASE.xquantity"));

            /* not constexpr until c++26 */
            auto ng = unit_qty(u::nanogram);

            log && log(xtag("ng", ng));

            REQUIRE(ng.scale() == 1);
        } /*TEST_CASE(xquantity)*/

        TEST_CASE("xquantity2", "[xquantity]") {
            constexpr bool c_debug_flag = false;

            scope log(XO_DEBUG2(c_debug_flag, "TEST_CASE.xquantity2"));

            /* not constexpr until c++26 */
            xquantity ng = unit_qty(u::nanogram);
            auto ng2 = ng * ng;

            log && log(xtag("ng*ng", ng2));

            REQUIRE(ng2.scale() == 1);
        } /*TEST_CASE(xquantity2)*/

        TEST_CASE("xquantity3", "[xquantity]") {
            constexpr bool c_debug_flag = false;

            scope log(XO_DEBUG2(c_debug_flag, "TEST_CASE.xquantity3"));

            /* not constexpr until c++26 */
            xquantity ng = unit_qty(u::nanogram);
            auto ng0 = ng / ng;

            log && log(xtag("ng/ng", ng0));

            REQUIRE(ng0.scale() == 1);
        } /*TEST_CASE(xquantity3)*/

        TEST_CASE("xquantity4", "[xquantity]") {
            constexpr bool c_debug_flag = false;

            scope log(XO_DEBUG2(c_debug_flag, "TEST_CASE.xquantity4"));

            /* not constexpr until c++26 */
            xquantity ng = unit_qty(u::nanogram);
            xquantity ug = unit_qty(u::microgram);

            {
                auto prod1 = ng * ug;
                log && log(xtag("ng*ug", prod1));

                /* units will be nanograms,  since that's on lhs */
                REQUIRE(prod1.unit().n_bpu() == 1);
                REQUIRE(prod1.unit()[0].native_dim() == dim::mass);
                REQUIRE(prod1.unit()[0].scalefactor() == scalefactor_ratio_type(1, 1000000000));
                REQUIRE(prod1.unit()[0].power() == power_ratio_type(2, 1));
                REQUIRE(prod1.scale() == 1000);
            }

            {
                auto prod2 = ug * ng;
                log && log(xtag("ug*ng", prod2));

                REQUIRE(prod2.unit().n_bpu() == 1);
                REQUIRE(prod2.unit()[0].native_dim() == dim::mass);
                REQUIRE(prod2.unit()[0].native_dim() == dim::mass);
                REQUIRE(prod2.unit()[0].scalefactor() == scalefactor_ratio_type(1, 1000000));
                REQUIRE(prod2.unit()[0].power() == power_ratio_type(2, 1));
                REQUIRE(prod2.scale() == 0.001);
            }

            //REQUIRE(ng2.scale() == 1);
        } /*TEST_CASE(xquantity4)*/

        TEST_CASE("xquantity5", "[xquantity]") {
            constexpr bool c_debug_flag = false;

            scope log(XO_DEBUG2(c_debug_flag, "TEST_CASE.xquantity5"));

            /* not constexpr until c++26 */
            xquantity ng = unit_qty(u::nanogram);
            xquantity ug = unit_qty(u::microgram);

            {
                auto ratio1 = ng / ug;
                log && log(xtag("ng/ug", ratio1));

                /* units will be nanograms,  since that's on lhs */
                REQUIRE(ratio1.unit().n_bpu() == 0);
                REQUIRE(ratio1.scale() == 0.001);
            }

            {
                auto ratio2 = ug / ng;
                log && log(xtag("ug/ng", ratio2));

                REQUIRE(ratio2.unit().n_bpu() == 0);
                REQUIRE(ratio2.scale() == 1000.0);
            }

            //REQUIRE(ng2.scale() == 1);
        } /*TEST_CASE(xquantity5)*/

        TEST_CASE("xquantity6", "[xquantity]") {
            constexpr bool c_debug_flag = false;

            scope log(XO_DEBUG2(c_debug_flag, "TEST_CASE.xquantity6"));

            /* not constexpr until c++26 */
            xquantity ng = unit_qty(u::nanogram);
            xquantity ug = unit_qty(u::microgram);

            {
                auto sum1 = ng + ug;
                log && log(xtag("ng+ug", sum1));

                /* units will be nanograms,  since that's on lhs */
                REQUIRE(sum1.unit().n_bpu() == 1);
                REQUIRE(sum1.unit()[0].scalefactor() == scalefactor_ratio_type(1, 1000000000));
                REQUIRE(sum1.scale() == 1001.0);
            }

            {
                auto sum2 = ug + ng;
                log && log(xtag("ug+ng", sum2));

                /* units will be micrograms,  since that's on rhs */
                REQUIRE(sum2.unit().n_bpu() == 1);
                REQUIRE(sum2.unit()[0].scalefactor() == scalefactor_ratio_type(1, 1000000));
                REQUIRE(sum2.scale() == 1.001);
            }

            //REQUIRE(ng2.scale() == 1);
        } /*TEST_CASE(xquantity6)*/

        TEST_CASE("xquantity7", "[xquantity]") {
            constexpr bool c_debug_flag = false;

            scope log(XO_DEBUG2(c_debug_flag, "TEST_CASE.xquantity7"));

            /* not constexpr until c++26 */
            xquantity ng = unit_qty(u::nanogram);
            xquantity ug = unit_qty(u::microgram);

            {
                auto sum1 = ng - ug;
                log && log(xtag("ng-ug", sum1));

                /* units will be nanograms,  since that's on lhs */
                REQUIRE(sum1.unit().n_bpu() == 1);
                REQUIRE(sum1.unit()[0].scalefactor() == scalefactor_ratio_type(1, 1000000000));
                REQUIRE(sum1.scale() == -999.0);
            }

            {
                auto sum2 = ug - ng;
                log && log(xtag("ug-ng", sum2));

                /* units will be micrograms,  since that's on rhs */
                REQUIRE(sum2.unit().n_bpu() == 1);
                REQUIRE(sum2.unit()[0].scalefactor() == scalefactor_ratio_type(1, 1000000));
                REQUIRE(sum2.scale() == 0.999);
            }

            //REQUIRE(ng2.scale() == 1);
        } /*TEST_CASE(xquantity7)*/

        TEST_CASE("xquantity.compare", "[xquantity.compare]") {
            constexpr bool c_debug_flag = false;

            scope log(XO_DEBUG2(c_debug_flag, "TEST_CASE.xquantity.compare"));

            /* not constexpr until c++26 */
            xquantity ng = 1000 * unit_qty(u::nanogram);
            xquantity ug = unit_qty(u::microgram);

            {
                auto cmp = (ng == ug);
                log && log(xtag("ng==ug", cmp));

                /* units will be nanograms,  since that's on lhs */
                REQUIRE(cmp == true);
            }

            {
                auto cmp = (ng != ug);
                log && log(xtag("ng!=ug", cmp));

                /* units will be nanograms,  since that's on lhs */
                REQUIRE(cmp == false);
            }

            {
                auto cmp = (ng < ug);
                log && log(xtag("ng<ug", cmp));

               /* units will be nanograms,  since that's on lhs */
                REQUIRE(cmp == false);
            }

            {
                auto cmp = (ng <= ug);
                log && log(xtag("ng=<ug", cmp));

                /* units will be nanograms,  since that's on lhs */
                REQUIRE(cmp == true);
            }

            {
                auto cmp = (ng > ug);
                log && log(xtag("ng>ug", cmp));

                /* units will be nanograms,  since that's on lhs */
                REQUIRE(cmp == false);
            }

            {
                auto cmp = (ng >= ug);
                log && log(xtag("ng>=ug", cmp));

                /* units will be nanograms,  since that's on lhs */
                REQUIRE(cmp == true);
            }

        } /*TEST_CASE(xquantity.compare)*/

        TEST_CASE("xquantity.compare2", "[xquantity]") {
            constexpr bool c_debug_flag = false;

            scope log(XO_DEBUG2(c_debug_flag, "TEST_CASE.xquantity.compare2"));

            /* not constexpr until c++26 */
            xquantity ng = unit_qty(u::nanogram);
            xquantity ug = unit_qty(u::microgram);

            {
                auto cmp = (ng == ug);
                log && log(xtag("ng==ug", cmp));

                /* units will be nanograms,  since that's on lhs */
                REQUIRE(cmp == false);
            }

            {
                auto cmp = (ng != ug);
                log && log(xtag("ng!=ug", cmp));

                /* units will be nanograms,  since that's on lhs */
                REQUIRE(cmp == true);
            }

            {
                auto cmp = (ng < ug);
                log && log(xtag("ng<ug", cmp));

                /* units will be nanograms,  since that's on lhs */
                REQUIRE(cmp == true);
            }

            {
                auto cmp = (ng <= ug);
                log && log(xtag("ng=<ug", cmp));

                /* units will be nanograms,  since that's on lhs */
                REQUIRE(cmp == true);
            }

            {
                auto cmp = (ng > ug);
                log && log(xtag("ng>ug", cmp));

                /* units will be nanograms,  since that's on lhs */
                REQUIRE(cmp == false);
            }

            {
                auto cmp = (ng >= ug);
                log && log(xtag("ng>=ug", cmp));

                /* units will be nanograms,  since that's on lhs */
                REQUIRE(cmp == false);
            }

        } /*TEST_CASE(xquantity.compare2)*/
    } /*namespace ut*/
} /*namespace xo*/


/* end xquantity.test.cpp */
