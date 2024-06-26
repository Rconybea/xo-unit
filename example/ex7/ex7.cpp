/** @file ex7.cpp **/

#include "xo/unit/quantity.hpp"
#include "xo/unit/quantity_iostream.hpp"
#include <iostream>

int
main () {
    using namespace xo::qty;
    namespace u = xo::qty::u;
    namespace q = xo::qty::qty;
    using namespace std;

    quantity qty1 = 7.55 * q::kilometer / q::minute / q::minute;
    quantity qty2 = q::nanograms(123);
    quantity qty3 = qty2 * qty1;

    cerr << "qty1: " << qty1 << endl;
    cerr << "qty2: " << qty2 << endl;
    cerr << "qty3: " << qty3 << endl;

    /* rescale to not-so-absurd units */

    /* kg.m.s^-2 */
    quantity res = qty3.rescale_ext<u::kilogram * u::meter / (u::second * u::second)>();

    /* 2.57958e-10kg.m.s^-2 */
    cerr << "res: " << res << endl;
}

/** end ex7.cpp */
