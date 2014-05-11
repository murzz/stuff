build_dir=/tmp
make=$(MAKE) --no-print-directory

# make file dir
src_dir:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

$(build_dir):
all: $(build_dir)/cmake_bootstrap_success
	@cd $(build_dir) && $(make) $@

clean:
	@rm -rf $(build_dir)/

config: $(build_dir)/CMakeCache.txt
	@ccmake $(build_dir)/

#test: $(build_dir)/cmake_bootstrap_success all
#	@cd $(build_dir) && $(make) $@

htmlcxx:
	@mkdir -p $(build_dir) && cd $(build_dir) && ../scripts/htmlcxx.sh htmlcxx

# CMake setup
$(build_dir)/CMakeCache.txt:
	@mkdir -p $(build_dir)
	-@cd $(build_dir) && cmake $(src_dir)

# CMake successful setup
$(build_dir)/cmake_bootstrap_success:
	@mkdir -p $(build_dir)
	@cd $(build_dir) && cmake $(src_dir)
	@touch $(build_dir)/cmake_bootstrap_success

# Forward targets to cmake generated makefile
%: $(build_dir)/cmake_bootstrap_success
	@cd $(build_dir) && $(make) $@

#.PHONY: all clean config test
.PHONY: all clean config
.SECONDARY: $(build_dir)/cmake_bootstrap_success
