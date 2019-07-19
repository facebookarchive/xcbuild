#
# Copyright (c) 2015-present, Facebook, Inc.
# All rights reserved.
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree.
#

build := build
project := project

cmake := cmake
cmake_flags := -DCMAKE_INSTALL_PREFIX= $(shell echo "$$CMAKE_FLAGS")

ninja := $(if $(shell which llbuild),llbuild ninja build,ninja)
ninja_flags := $(shell echo "$$NINJA_FLAGS")

all:
	mkdir -p $(build)
	$(cmake) -B$(build) -H. -G Ninja $(cmake_flags)
	$(ninja) -C $(build) $(ninja_flags)

install: all
	@DESTDIR=$(realpath $(PREFIX)) \
	$(ninja) -C $(build) $(ninja_flags) install

project:
	mkdir -p $(project)
	$(cmake) -B$(project) -H. -G Xcode $(cmake_flags)

test: all
	set -e; for test in build/test_*; do echo; echo "$$test"; $$TEST_RUNNER ./$$test; done

clean:
	rm -rf $(build)

.PHONY: project
