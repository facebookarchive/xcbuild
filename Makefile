# Copyright 2013-present Facebook. All Rights Reserved.

build := build

cmake := cmake
cmake_flags := -G Ninja

ninja := ninja
ninja_flags :=

all:
	mkdir -p $(build)
	cd $(build) && $(cmake) $(cmake_flags) ..
	cd $(build) && $(ninja) $(ninja_flags)

test: all
	build/test_*

clean:
	rm -rf $(build)

