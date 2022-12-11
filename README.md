# OkLab with Integer arithmetic

More information here: http://blog.pkh.me/p/38-porting-oklab-colorspace-to-integer-arithmetic.html


## Dependencies

- C toolchain
- GNU/Make
- meson
- hyperfine (optional, for benchmarks)


## Usage

- `make tests`: build and run accuracy tests
- `make bench-all`: build and run all benchmarks
- `make gen-code`: regenerate C code (integer and fast float32). This is
  required if the scripts or template files (`*_tpl.[ch]`) are updated.

More rules with finer granularity are also available.
