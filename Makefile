#
# Copyright (c) 2015-present, Facebook, Inc.
# All rights reserved.
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree. An additional grant
# of patent rights can be found in the PATENTS file in the same directory.
#

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

