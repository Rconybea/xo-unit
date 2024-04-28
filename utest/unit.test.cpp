/* @file unit.test.cpp */

//#include "xo/unit/unit2.hpp"
#include "xo/unit/Quantity_iostream.hpp"
#include "xo/unit/scaled_unit_iostream.hpp"
#include "xo/unit/Quantity.hpp"
#include "xo/unit/natural_unit.hpp"
#include "xo/unit/bpu_store.hpp"
#include "xo/unit/bpu.hpp"
#include "xo/indentlog/scope.hpp"
#include "xo/indentlog/print/tag.hpp"
#include <catch2/catch.hpp>

namespace xo {
    namespace ut {
        /* compile-time tests */

        //namespace bu = xo::qty::bu;
        namespace su2 = xo::qty::su2;

        using xo::qty::Quantity;
        using xo::qty::dim;
        using xo::qty::basis_unit2_abbrev_type;
        using xo::qty::scalefactor_ratio_type;
        //using xo::qty::bpu_abbrev_type;
        using xo::qty::basis_unit2_store;
        using xo::qty::power_ratio_type;
        using xo::qty::bpu;
        using xo::qty::natural_unit;
        using xo::qty::unit_qty;

        TEST_CASE("basis_unit2_store", "[basis_unit2_store]") {
            constexpr bool c_debug_flag = false;

            // can get bits from /dev/random by uncommenting the 2nd line below
            //uint64_t seed = xxx;
            //rng::Seed<xoshio256ss> seed;

            //auto rng = xo::rng::xoshiro256ss(seed);

            scope log(XO_DEBUG2(c_debug_flag, "TEST_CASE.basis_unit2_store"));
            //log && log("(A)", xtag("foo", foo));

            basis_unit2_store<class AnyTag> bu_store;

            log && log(xtag("mass*10^-9",       bu_store.bu_abbrev(dim::mass,     scalefactor_ratio_type(          1, 1000000000))));
            log && log(xtag("mass*10^-6",       bu_store.bu_abbrev(dim::mass,     scalefactor_ratio_type(          1,    1000000))));
            log && log(xtag("mass*10^-3",       bu_store.bu_abbrev(dim::mass,     scalefactor_ratio_type(          1,       1000))));
            log && log(xtag("mass",             bu_store.bu_abbrev(dim::mass,     scalefactor_ratio_type(          1,          1))));
            log && log(xtag("mass*10^3",        bu_store.bu_abbrev(dim::mass,     scalefactor_ratio_type(       1000,          1))));
            log && log(xtag("mass*10^6",        bu_store.bu_abbrev(dim::mass,     scalefactor_ratio_type(    1000000,          1))));
            log && log(xtag("mass*10^9",        bu_store.bu_abbrev(dim::mass,     scalefactor_ratio_type( 1000000000,          1))));

            log && log(xtag("distance*10^-9",   bu_store.bu_abbrev(dim::distance, scalefactor_ratio_type(          1, 1000000000))));
            log && log(xtag("distance*10^-6",   bu_store.bu_abbrev(dim::distance, scalefactor_ratio_type(          1,    1000000))));
            log && log(xtag("distance*10^-3",   bu_store.bu_abbrev(dim::distance, scalefactor_ratio_type(          1,       1000))));
            log && log(xtag("distance",         bu_store.bu_abbrev(dim::distance, scalefactor_ratio_type(          1,          1))));
            log && log(xtag("distance*10^3",    bu_store.bu_abbrev(dim::distance, scalefactor_ratio_type(       1000,          1))));

            log && log(xtag("time*10^-9",       bu_store.bu_abbrev(dim::time,     scalefactor_ratio_type(          1, 1000000000))));
            log && log(xtag("time*10^-6",       bu_store.bu_abbrev(dim::time,     scalefactor_ratio_type(          1,    1000000))));
            log && log(xtag("time*10^-3",       bu_store.bu_abbrev(dim::time,     scalefactor_ratio_type(          1,       1000))));
            log && log(xtag("time",             bu_store.bu_abbrev(dim::time,     scalefactor_ratio_type(          1,          1))));
            log && log(xtag("time*60",          bu_store.bu_abbrev(dim::time,     scalefactor_ratio_type(         60,          1))));
            log && log(xtag("time*3600",        bu_store.bu_abbrev(dim::time,     scalefactor_ratio_type(       3600,          1))));
            log && log(xtag("time*24*3600",     bu_store.bu_abbrev(dim::time,     scalefactor_ratio_type(    24*3600,          1))));
            log && log(xtag("time*250*24*3600", bu_store.bu_abbrev(dim::time,     scalefactor_ratio_type(250*24*3600,          1))));
            log && log(xtag("time*360*24*3600", bu_store.bu_abbrev(dim::time,     scalefactor_ratio_type(360*24*3600,          1))));
            log && log(xtag("time*365*24*3600", bu_store.bu_abbrev(dim::time,     scalefactor_ratio_type(365*24*3600,          1))));

            REQUIRE(::strcmp(bu_store.bu_abbrev(dim::mass,     scalefactor_ratio_type(          1, 1000000000)).c_str(), "ng") == 0);
            REQUIRE(::strcmp(bu_store.bu_abbrev(dim::mass,     scalefactor_ratio_type(          1,    1000000)).c_str(), "ug") == 0);
            REQUIRE(::strcmp(bu_store.bu_abbrev(dim::mass,     scalefactor_ratio_type(          1,       1000)).c_str(), "mg") == 0);
            REQUIRE(::strcmp(bu_store.bu_abbrev(dim::mass,     scalefactor_ratio_type(          1,          1)).c_str(), "g") == 0);
            REQUIRE(::strcmp(bu_store.bu_abbrev(dim::mass,     scalefactor_ratio_type(       1000,          1)).c_str(), "kg") == 0);
            REQUIRE(::strcmp(bu_store.bu_abbrev(dim::mass,     scalefactor_ratio_type(    1000000,          1)).c_str(), "t") == 0);
            REQUIRE(::strcmp(bu_store.bu_abbrev(dim::mass,     scalefactor_ratio_type( 1000000000,          1)).c_str(), "kt") == 0);

            REQUIRE(::strcmp(bu_store.bu_abbrev(dim::distance, scalefactor_ratio_type(          1, 1000000000)).c_str(), "nm") == 0);
            REQUIRE(::strcmp(bu_store.bu_abbrev(dim::distance, scalefactor_ratio_type(          1,    1000000)).c_str(), "um") == 0);
            REQUIRE(::strcmp(bu_store.bu_abbrev(dim::distance, scalefactor_ratio_type(          1,       1000)).c_str(), "mm") == 0);
            REQUIRE(::strcmp(bu_store.bu_abbrev(dim::distance, scalefactor_ratio_type(          1,          1)).c_str(), "m") == 0);
            REQUIRE(::strcmp(bu_store.bu_abbrev(dim::distance, scalefactor_ratio_type(       1000,          1)).c_str(), "km") == 0);

            REQUIRE(::strcmp(bu_store.bu_abbrev(dim::time,     scalefactor_ratio_type(          1, 1000000000)).c_str(), "ns") == 0);
            REQUIRE(::strcmp(bu_store.bu_abbrev(dim::time,     scalefactor_ratio_type(          1,    1000000)).c_str(), "us") == 0);
            REQUIRE(::strcmp(bu_store.bu_abbrev(dim::time,     scalefactor_ratio_type(          1,       1000)).c_str(), "ms") == 0);
            REQUIRE(::strcmp(bu_store.bu_abbrev(dim::time,     scalefactor_ratio_type(          1,          1)).c_str(), "s") == 0);
            REQUIRE(::strcmp(bu_store.bu_abbrev(dim::time,     scalefactor_ratio_type(         60,          1)).c_str(), "min") == 0);
            REQUIRE(::strcmp(bu_store.bu_abbrev(dim::time,     scalefactor_ratio_type(       3600,          1)).c_str(), "hr") == 0);
            REQUIRE(::strcmp(bu_store.bu_abbrev(dim::time,     scalefactor_ratio_type(    24*3600,          1)).c_str(), "dy") == 0);
            REQUIRE(::strcmp(bu_store.bu_abbrev(dim::time,     scalefactor_ratio_type(250*24*3600,          1)).c_str(), "yr250") == 0);
            REQUIRE(::strcmp(bu_store.bu_abbrev(dim::time,     scalefactor_ratio_type(360*24*3600,          1)).c_str(), "yr360") == 0);
            REQUIRE(::strcmp(bu_store.bu_abbrev(dim::time,     scalefactor_ratio_type(365*24*3600,          1)).c_str(), "yr365") == 0);

        } /*TEST_CASE(basis_unit2_store)*/

        TEST_CASE("scaled_unit0", "[scaled_unit0]") {
            constexpr bool c_debug_flag = true;

            // can get bits from /dev/random by uncommenting the 2nd line below
            //uint64_t seed = xxx;
            //rng::Seed<xoshio256ss> seed;

            //auto rng = xo::rng::xoshiro256ss(seed);

            scope log(XO_DEBUG2(c_debug_flag, "TEST_CASE.scaled_unit0"));
            //log && log("(A)", xtag("foo", foo));

            constexpr auto ng = su2::nanogram;
            constexpr auto ng2 = ng * ng;

            log && log(xtag("ng", ng));
            log && log(xtag("ng*ng", ng2));
            //log && log(xtag("ng/ng",

            static_assert(ng.natural_unit_.n_bpu() == 1);
            static_assert(ng2.natural_unit_.n_bpu() == 1);
        } /*TEST_CASE(scaled_unit0)*/

        TEST_CASE("scaled_unit1", "[scaled_unit1]") {
            constexpr bool c_debug_flag = true;

            // can get bits from /dev/random by uncommenting the 2nd line below
            //uint64_t seed = xxx;
            //rng::Seed<xoshio256ss> seed;

            //auto rng = xo::rng::xoshiro256ss(seed);

            scope log(XO_DEBUG2(c_debug_flag, "TEST_CASE.scaled_unit1"));
            //log && log("(A)", xtag("foo", foo));

            constexpr auto ng = su2::nanogram;
            constexpr auto ng2 = ng / ng;

            log && log(xtag("ng", ng));
            log && log(xtag("ng/ng", ng2));
            //log && log(xtag("ng/ng",

            static_assert(ng.natural_unit_.n_bpu() == 1);
            static_assert(ng2.natural_unit_.n_bpu() == 0);
        } /*TEST_CASE(scaled_unit1)*/

        TEST_CASE("Quantity", "[Quantity]") {
            constexpr bool c_debug_flag = true;

            // can get bits from /dev/random by uncommenting the 2nd line below
            //uint64_t seed = xxx;
            //rng::Seed<xoshio256ss> seed;

            //auto rng = xo::rng::xoshiro256ss(seed);

            scope log(XO_DEBUG2(c_debug_flag, "TEST_CASE.Quantity"));
            //log && log("(A)", xtag("foo", foo));

            /* not constexpr until c++26 */
            auto ng = unit_qty(su2::nanogram);

            log && log(xtag("ng", ng));

            REQUIRE(ng.scale() == 1);
        } /*TEST_CASE(Quantity)*/

        TEST_CASE("Quantity2", "[Quantity]") {
            constexpr bool c_debug_flag = true;

            // can get bits from /dev/random by uncommenting the 2nd line below
            //uint64_t seed = xxx;
            //rng::Seed<xoshio256ss> seed;

            //auto rng = xo::rng::xoshiro256ss(seed);

            scope log(XO_DEBUG2(c_debug_flag, "TEST_CASE.Quantity2"));
            //log && log("(A)", xtag("foo", foo));

            /* not constexpr until c++26 */
            Quantity ng = unit_qty(su2::nanogram);
            auto ng2 = ng * ng;

            log && log(xtag("ng*ng", ng2));

            REQUIRE(ng2.scale() == 1);
        } /*TEST_CASE(Quantity2)*/

        TEST_CASE("Quantity3", "[Quantity]") {
            constexpr bool c_debug_flag = true;

            // can get bits from /dev/random by uncommenting the 2nd line below
            //uint64_t seed = xxx;
            //rng::Seed<xoshio256ss> seed;

            //auto rng = xo::rng::xoshiro256ss(seed);

            scope log(XO_DEBUG2(c_debug_flag, "TEST_CASE.Quantity3"));
            //log && log("(A)", xtag("foo", foo));

            /* not constexpr until c++26 */
            Quantity ng = unit_qty(su2::nanogram);
            auto ng0 = ng / ng;

            log && log(xtag("ng/ng", ng0));

            REQUIRE(ng0.scale() == 1);
        } /*TEST_CASE(Quantity3)*/

        TEST_CASE("Quantity4", "[Quantity]") {
            constexpr bool c_debug_flag = true;

            // can get bits from /dev/random by uncommenting the 2nd line below
            //uint64_t seed = xxx;
            //rng::Seed<xoshio256ss> seed;

            //auto rng = xo::rng::xoshiro256ss(seed);

            scope log(XO_DEBUG2(c_debug_flag, "TEST_CASE.Quantity4"));
            //log && log("(A)", xtag("foo", foo));

            /* not constexpr until c++26 */
            Quantity ng = unit_qty(su2::nanogram);
            Quantity ug = unit_qty(su2::microgram);

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
        } /*TEST_CASE(Quantity4)*/

        TEST_CASE("Quantity5", "[Quantity]") {
            constexpr bool c_debug_flag = true;

            // can get bits from /dev/random by uncommenting the 2nd line below
            //uint64_t seed = xxx;
            //rng::Seed<xoshio256ss> seed;

            //auto rng = xo::rng::xoshiro256ss(seed);

            scope log(XO_DEBUG2(c_debug_flag, "TEST_CASE.Quantity5"));
            //log && log("(A)", xtag("foo", foo));

            /* not constexpr until c++26 */
            Quantity ng = unit_qty(su2::nanogram);
            Quantity ug = unit_qty(su2::microgram);

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
        } /*TEST_CASE(Quantity5)*/

        TEST_CASE("Quantity6", "[Quantity]") {
            constexpr bool c_debug_flag = true;

            // can get bits from /dev/random by uncommenting the 2nd line below
            //uint64_t seed = xxx;
            //rng::Seed<xoshio256ss> seed;

            //auto rng = xo::rng::xoshiro256ss(seed);

            scope log(XO_DEBUG2(c_debug_flag, "TEST_CASE.Quantity6"));
            //log && log("(A)", xtag("foo", foo));

            /* not constexpr until c++26 */
            Quantity ng = unit_qty(su2::nanogram);
            Quantity ug = unit_qty(su2::microgram);

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
        } /*TEST_CASE(Quantity6)*/

        TEST_CASE("Quantity7", "[Quantity]") {
            constexpr bool c_debug_flag = true;

            // can get bits from /dev/random by uncommenting the 2nd line below
            //uint64_t seed = xxx;
            //rng::Seed<xoshio256ss> seed;

            //auto rng = xo::rng::xoshiro256ss(seed);

            scope log(XO_DEBUG2(c_debug_flag, "TEST_CASE.Quantity7"));
            //log && log("(A)", xtag("foo", foo));

            /* not constexpr until c++26 */
            Quantity ng = unit_qty(su2::nanogram);
            Quantity ug = unit_qty(su2::microgram);

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
        } /*TEST_CASE(Quantity7)*/

        TEST_CASE("Quantity.compare", "[Quantity.compare]") {
            constexpr bool c_debug_flag = false;

            // can get bits from /dev/random by uncommenting the 2nd line below
            //uint64_t seed = xxx;
            //rng::Seed<xoshio256ss> seed;

            //auto rng = xo::rng::xoshiro256ss(seed);

            scope log(XO_DEBUG2(c_debug_flag, "TEST_CASE.Quantity.compare"));
            //log && log("(A)", xtag("foo", foo));

            /* not constexpr until c++26 */
            Quantity ng = 1000 * unit_qty(su2::nanogram);
            Quantity ug = unit_qty(su2::microgram);

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

        } /*TEST_CASE(Quantity.compare)*/

        TEST_CASE("Quantity.compare2", "[Quantity]") {
            constexpr bool c_debug_flag = false;

            // can get bits from /dev/random by uncommenting the 2nd line below
            //uint64_t seed = xxx;
            //rng::Seed<xoshio256ss> seed;

            //auto rng = xo::rng::xoshiro256ss(seed);

            scope log(XO_DEBUG2(c_debug_flag, "TEST_CASE.Quantity.compare2"));
            //log && log("(A)", xtag("foo", foo));

            /* not constexpr until c++26 */
            Quantity ng = unit_qty(su2::nanogram);
            Quantity ug = unit_qty(su2::microgram);

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

        } /*TEST_CASE(Quantity.compare2)*/
    } /*namespace ut*/
} /*namespace xo*/

/* end dimension.test.cpp */
