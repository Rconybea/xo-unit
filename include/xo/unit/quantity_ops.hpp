/** @file quantity_ops.hpp
 *
 *  Author: Roland Conybeare
 **/

#pragma once

#include "quantity2_concept.hpp"
//#include <cstdint>

namespace xo {
    namespace qty {
        /** note: won't have constexpr result until c++26 (when ::sqrt(), ::pow() are constexpr)
         **/
        template <typename Quantity, typename Quantity2>
        requires quantity2_concept<Quantity> && quantity2_concept<Quantity2>
        constexpr auto
        operator* (const Quantity & x, const Quantity2 & y)
        {
            return Quantity::multiply(x, y);
        }

        /** note: does not require unit scaling,  so constexpr with c++23 **/
        template <typename Dimensionless, typename Quantity>
        requires std::is_arithmetic_v<Dimensionless> && quantity2_concept<Quantity>
        constexpr auto
        operator* (Dimensionless x, const Quantity & y)
        {
            return y.scale_by(x);
        }

        /** note: does not require unit scaling,  so constexpr with c++23 **/
        template <typename Dimensionless, typename Quantity>
        requires std::is_arithmetic_v<Dimensionless> && quantity2_concept<Quantity>
        constexpr auto
        operator* (const Quantity & x, Dimensionless y)
        {
            return x.scale_by(y);
        }

        /** note: won't have constexpr result until c++26 (when ::sqrt(), ::pow() are constexpr)
         **/
        template <typename Quantity, typename Quantity2>
        requires quantity2_concept<Quantity> && quantity2_concept<Quantity2>
        constexpr auto
        operator== (const Quantity & x, const Quantity2 & y)
        {
            return (Quantity::compare(x, y) == 0);
        }

        /** note: won't have constexpr result until c++26 (when ::sqrt(), ::pow() are constexpr)
         **/
        template <typename Quantity, typename Quantity2>
        requires quantity2_concept<Quantity> && quantity2_concept<Quantity2>
        constexpr auto
        operator<=> (const Quantity & x, const Quantity2 & y)
        {
            return Quantity::compare(x, y);
        }

    } /*namespace qty*/

} /*namespace xo*/


/** end quantity_ops.hpp **/