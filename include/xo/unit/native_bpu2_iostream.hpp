/** @file native_bpu2_iostream.hpp
 *
 *  Author: Roland Conybeare
 **/

#pragma once

#include "xo/ratio/ratio_iostream.hpp"
#include "native_bpu2.hpp"
#include <iostream>

namespace xo {
    namespace unit {
        template <typename Int>
        inline std::ostream &
        operator<<(std::ostream & os, const bpu2<Int> & x) {
            os << "<bpu"
               << xtag("dim", x.native_dim())
               << xtag("mult", x.scalefactor())
               << xtag("pwr", x.power())
               << ">";

            return os;
        }
    } /*namespace unit*/
} /*namespace xo*/


/** end native_bpu2_iostream.hpp **/
