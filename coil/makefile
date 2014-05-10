all: build/cmake_bootstrap_success
	#@cd build && make --no-print-directory $@
	@cd build && make -j $@

clean:
	@rm -rf build/

config: build/CMakeCache.txt
	@ccmake build/

test: build/cmake_bootstrap_success all
	@cd build && make -j --no-print-directory $@
	
htmlcxx:
	@mkdir -p build && cd build && ../scripts/htmlcxx.sh htmlcxx

# CMake setup
build/CMakeCache.txt:
	@mkdir -p build
	-@cd build && cmake ..

# CMake successful setup
build/cmake_bootstrap_success:
	@mkdir -p build
	@cd build && cmake ..
	@touch build/cmake_bootstrap_success

# Forward targets to cmake generated makefile
%: build/cmake_bootstrap_success
	@cd build && make -j --no-print-directory $@

#.PHONY: all clean config test
.PHONY: all clean config
.SECONDARY: build/cmake_bootstrap_success
