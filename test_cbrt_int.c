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

#include "oklab_int.h"

int main(void)
{
    float max_diff = 0;
    float total_diff = 0;

    for (int i = 0; i <= K; i++) {
        const float ref = cbrtf(i / (float)K);
        const float out = cbrt01_int(i) / (float)K;
        const float d = fabs(ref - out);

        if (d > max_diff)
            max_diff = d;
        total_diff += d;
    }

    printf("max_diff=%f total_diff=%f avg_diff=%f\n",
           max_diff, total_diff, total_diff / (K + 1));
    return 0;
}
