#
# Copyright (c) 2015-present, Facebook, Inc.
# All rights reserved.
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree. An additional grant
# of patent rights can be found in the PATENTS file in the same directory.
#

build := build
project := project
PREFIX := /

cmake := cmake
cmake_flags := 
cmake_command := $(cmake) -H. $(cmake_flags)

ninja := $(if $(shell which llbuild),llbuild ninja build,ninja)
ninja_flags := $(if $(shell echo "$$NINJA_JOBS"),-j$(shell echo "$$NINJA_JOBS"),)
ninja_command := $(ninja) $(ninja_flags)

all:
	mkdir -p $(build)
	$(cmake_command) -B$(build) -GNinja
	$(ninja_command) -C $(build)

project:
	mkdir -p $(project)
	$(cmake_command) -B$(project) -GXcode

install:
	mkdir -p $(PREFIX)
	$(cmake_command) -B$(build) -GNinja -DCMAKE_INSTALL_PREFIX:PATH=$(realpath $(PREFIX)) -DCMAKE_INSTALL_NAME_DIR:STRING=@executable_path/../lib
	$(ninja_command) -C $(build) install

test: all
	set -e; for test in build/test_*; do echo; echo "$$test"; ./$$test; done

clean:
	rm -rf $(build)

.PHONY: project
