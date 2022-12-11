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

#include <stdio.h>
#include <stdlib.h>

#include "oklab_int.h"
#include "oklab_ref.h"

int main(void)
{
    int max_diff = 0;
    int total_diff = 0;

    for (int i = 0; i <= K; i++) {
        const uint8_t ref = linear_f32_to_srgb_u8(i / (float)K);
        const uint8_t out = linear_int_to_srgb_u8(i);
        const int d = abs((int)ref - (int)out);

        if (d > max_diff)
            max_diff = d;
        total_diff += d;
    }

    printf("Linear to sRGB: max_diff=%d total_diff=%d\n", max_diff, total_diff);
    return 0;
}
