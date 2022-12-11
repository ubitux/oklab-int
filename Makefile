TESTS = test-cbrt test-srgb2linear-f32-fast test-linear2srgb-f32-fast test-srgb2linear-int test-linear2srgb-int test-oklab-int
tests: $(TESTS)
test-cbrt-int: setup-tests
	./build-tests/test-cbrt-int
test-srgb2linear-int: setup-tests
	./build-tests/test-srgb2linear-int
test-linear2srgb-int: setup-tests
	./build-tests/test-linear2srgb-int
test-srgb2linear-f32-fast: setup-tests
	./build-tests/test-srgb2linear-f32-fast
test-linear2srgb-f32-fast: setup-tests
	./build-tests/test-linear2srgb-f32-fast
test-oklab-int: setup-tests
	./build-tests/test-oklab-int
setup-tests:
	meson setup build-tests -Db_sanitize=address,undefined --buildtype debugoptimized
	meson compile -v -C build-tests
.PHONY: tests $(TESTS) setup-tests

BENCH = bench-ref bench-f32-fast bench-int
bench-all: setup-bench
	hyperfine --warmup=3 ./build-bench/bench-oklab-ref ./build-bench/bench-oklab-f32-fast ./build-bench/bench-oklab-int
bench-ref: setup-bench
	hyperfine --warmup=3 ./build-bench/bench-oklab-ref
bench-f32-fast: setup-bench
	hyperfine --warmup=3 ./build-bench/bench-oklab-f32-fast
bench-int: setup-bench
	hyperfine --warmup=3 ./build-bench/bench-oklab-int
setup-bench:
	meson setup build-bench -Db_lto=true --buildtype release
	meson compile -v -C build-bench
.PHONY: bench-all $(BENCH) setup-bench

clean: clean-tests clean-bench
clean-tests:
	$(RM) -r build-tests
clean-bench:
	$(RM) -r build-bench
.PHONY: clean clean-tests clean-bench

gen-code: gen-code-int gen-code-f32-fast
gen-code-int:
	./oklab-int.py
gen-code-f32-fast:
	./oklab-f32-fast.py
.PHONY: gen-code gen-code-int gen-code-f32-fast
