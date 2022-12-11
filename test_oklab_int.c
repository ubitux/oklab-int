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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "oklab_int.h"
#include "oklab_ref.h"

int main(void)
{
    float max_diff = 0.f;
    float total_diff = 0.f;

    int max_diff_r = 0;
    int max_diff_g = 0;
    int max_diff_b = 0;

    for (uint32_t c = 0; c <= 0xffffff; c++) {
        const struct LabInt lab_int = srgb_u8_to_oklab_int(c);
        const struct Lab lab0 = srgb_u8_to_oklab_f32(c);
        const struct Lab lab1 = {lab_int.L / (float)K, lab_int.a / (float)K, lab_int.b / (float)K};

        const float dl = lab0.L - lab1.L;
        const float da = lab0.a - lab1.a;
        const float db = lab0.b - lab1.b;

        const float d = dl*dl + da*da + db*db;
        if (d > max_diff)
            max_diff = d;

        total_diff += d;

        const uint32_t out0 = oklab_f32_to_srgb_u8(lab0);
        const uint32_t out1 = oklab_int_to_srgb_u8(lab_int);

        const int dr8 = abs((int)(out1>>16 & 0xff) - (int)(out0>>16 & 0xff));
        const int dg8 = abs((int)(out1>> 8 & 0xff) - (int)(out0>> 8 & 0xff));
        const int db8 = abs((int)(out1     & 0xff) - (int)(out0     & 0xff));

        if (dr8 > max_diff_r) max_diff_r = dr8;
        if (dg8 > max_diff_g) max_diff_g = dg8;
        if (db8 > max_diff_b) max_diff_b = db8;
    }

    printf("sRGB to OkLab: max_diff=%f total_diff=%f\n", max_diff, total_diff);
    printf("OkLab to sRGB: max_diff_r=%d max_diff_g=%d max_diff_b=%d\n", max_diff_r, max_diff_g, max_diff_b);
    return 0;
}
