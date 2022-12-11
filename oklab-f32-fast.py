#!/usr/bin/env python3

from textwrap import indent

srgb2linear = lambda x: x / 12.92 if x < 0.04045 else ((x + 0.055) / 1.055) ** 2.4
linear2srgb = lambda x: x * 12.92 if x < 0.0031308 else 1.055 * x ** (1 / 2.4) - 0.055

P = (1 << 9) - 1

srgb2linear_table = [srgb2linear(i / 255) for i in range(256)]
linear2srgb_table = [round(linear2srgb(i / P) * 0xFF) for i in range(P + 1)]


def _get_table_str(table: list[int], fmt: str, cols: int):
    assert len(table) % cols == 0
    rows = len(table) // cols
    rows_str = []
    for row in range(rows):
        rows_str.append(
            " ".join(
                fmt.format(elem) + "," for elem in table[row * cols : (row + 1) * cols]
            )
        )
    return indent("\n".join(rows_str), "    ")


def _create_source(tpl, out):
    with open(tpl) as f:
        content = f.read()

    srgb2linear_table_str = _get_table_str(srgb2linear_table, "{:f}", 8)
    linear2srgb_table_str = _get_table_str(linear2srgb_table, "0x{:02x}", 16)

    content = content.replace("/* TPL_SRGB2LINEAR_TABLE */", srgb2linear_table_str)
    content = content.replace("/* TPL_LINEAR2SRGB_TABLE */", linear2srgb_table_str)

    with open(out, "w") as f:
        f.write(content)


def main():
    _create_source("oklab_f32_fast_tpl.c", "oklab_f32_fast.c")


if __name__ == "__main__":
    main()
