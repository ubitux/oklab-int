#!/usr/bin/env python3

from textwrap import indent

srgb2linear = lambda x: x / 12.92 if x < 0.04045 else ((x + 0.055) / 1.055) ** 2.4
linear2srgb = lambda x: x * 12.92 if x < 0.0031308 else 1.055 * x ** (1 / 2.4) - 0.055

N = 16
K = (1 << N) - 1
P = (1 << 9) - 1

srgb2linear_table = [round(srgb2linear(i / 255) * K) for i in range(256)]
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


def _create_header(tpl, out):
    with open(tpl) as f:
        content = f.read()
    content = content.replace(
        "#define K ((1 << 16) - 1)", f"#define K ((1 << {N}) - 1)"
    )
    with open(out, "w") as f:
        f.write(content)


def _create_source(tpl, out):
    with open(tpl) as f:
        content = f.read()

    srgb2linear_table_str = _get_table_str(srgb2linear_table, "0x{:04x}", 8)
    linear2srgb_table_str = _get_table_str(linear2srgb_table, "0x{:02x}", 16)

    content = content.replace("/* TPL_SRGB2LINEAR_TABLE */", srgb2linear_table_str)
    content = content.replace("/* TPL_LINEAR2SRGB_TABLE */", linear2srgb_table_str)

    constants = dict(
        # cube root
        CBRT_B=-2.19893,
        CBRT_C=2.01593,
        CBRT_D=0.219407,
        # sRGB to (Ok)Lab (Lms)
        S2L_LR=0.4122214708,
        S2L_LG=0.5363325363,
        S2L_LB=0.0514459929,
        S2L_MR=0.2119034982,
        S2L_MG=0.6806995451,
        S2L_MB=0.1073969566,
        S2L_SR=0.0883024619,
        S2L_SG=0.2817188376,
        S2L_SB=0.6299787005,
        # sRGB to (Ok)Lab (Lab)
        S2L_LL=0.2104542553,
        S2L_LM=0.7936177850,
        S2L_LS=-0.0040720468,
        S2L_AL=1.9779984951,
        S2L_AM=-2.4285922050,
        S2L_AS=0.4505937099,
        S2L_BL=0.0259040371,
        S2L_BM=0.7827717662,
        S2L_BS=-0.8086757660,
        # (Ok)Lab to sRGB (Lms)
        L2S_LA=0.3963377774,
        L2S_LB=0.2158037573,
        L2S_MA=-0.1055613458,
        L2S_MB=-0.0638541728,
        L2S_SA=-0.0894841775,
        L2S_SB=-1.2914855480,
        # (Ok)Lab to sRGB (rgb)
        L2S_RL=4.0767416621,
        L2S_RM=-3.3077115913,
        L2S_RS=0.2309699292,
        L2S_GL=-1.2684380046,
        L2S_GM=2.6097574011,
        L2S_GS=-0.3413193965,
        L2S_BL=-0.0041960863,
        L2S_BM=-0.7034186147,
        L2S_BS=1.7076147010,
    )

    for name, value in constants.items():
        int_val = round(value * K)
        content = content.replace(f"#define {name} 0\n", "")
        content = content.replace(name, f"{int_val}LL")

    content = content.replace("\n\n\n", "\n\n")
    content = content.replace("\n\n\n", "\n\n")

    with open(out, "w") as f:
        f.write(content)


def main():
    _create_header("oklab_int_tpl.h", "oklab_int.h")
    _create_source("oklab_int_tpl.c", "oklab_int.c")


if __name__ == "__main__":
    main()
