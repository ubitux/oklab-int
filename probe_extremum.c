/*
 * Copyright (c) 2022 Clément Bœsch <u pkh me>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <float.h>
#include <stdio.h>

#include "oklab_ref.h"
#include "oklab_int.h"

int main(void)
{
    struct LabInt minint = {INT32_MAX, INT32_MAX, INT32_MAX};
    struct LabInt maxint = {INT32_MIN, INT32_MIN, INT32_MIN};
    struct Lab minf32 = {FLT_MAX, FLT_MAX, FLT_MAX};
    struct Lab maxf32 = {-FLT_MAX, -FLT_MAX, -FLT_MAX};

    for (uint32_t c = 0; c <= 0xffffff; c++) {
        const struct Lab labf32 = srgb_u8_to_oklab_f32(c);
        const struct LabInt labint = srgb_u8_to_oklab_int(c);

        if (labint.L < minint.L) minint.L = labint.L;
        if (labint.a < minint.a) minint.a = labint.a;
        if (labint.b < minint.b) minint.b = labint.b;

        if (labint.L > maxint.L) maxint.L = labint.L;
        if (labint.a > maxint.a) maxint.a = labint.a;
        if (labint.b > maxint.b) maxint.b = labint.b;

        if (labf32.L < minf32.L) minf32.L = labf32.L;
        if (labf32.a < minf32.a) minf32.a = labf32.a;
        if (labf32.b < minf32.b) minf32.b = labf32.b;

        if (labf32.L > maxf32.L) maxf32.L = labf32.L;
        if (labf32.a > maxf32.a) maxf32.a = labf32.a;
        if (labf32.b > maxf32.b) maxf32.b = labf32.b;
    }

    printf("min Lab (f32): %f %f %f\n", minf32.L, minf32.a, minf32.b);
    printf("max Lab (f32): %f %f %f\n", maxf32.L, maxf32.a, maxf32.b);
    printf("min Lab (int): %d %d %d\n", minint.L, minint.a, minint.b);
    printf("max Lab (int): %d %d %d\n", maxint.L, maxint.a, maxint.b);
}
