#
# Copyright (c) 2015-present, Facebook, Inc.
# All rights reserved.
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree. An additional grant
# of patent rights can be found in the PATENTS file in the same directory.
#

install(FILES
        com.apple.build-tasks.copy-tiff-file.xcspec
        com.apple.build-tasks.copy-plist-file.xcspec
        com.apple.build-tasks.copy-png-file.xcspec
        com.apple.build-tasks.copy-strings-file.xcspec
        com.apple.build-tasks.ls-register-url.xcspec
        com.apple.build-tools.nmedit.xcspec
        com.apple.compilers.assetcatalog.xcspec
        com.apple.compilers.gcc.xcspec
        com.apple.compilers.lex.xcspec
        com.apple.compilers.llvm.clang.1_0.analyzer.xcspec
        com.apple.compilers.llvm.clang.1_0.astbuilder.xcspec
        com.apple.compilers.llvm.clang.1_0.compiler.xcspec
        com.apple.compilers.llvm.clang.1_0.migrator.xcspec
        com.apple.compilers.llvm.clang.1_0.migrator.xctest.xcspec
        com.apple.compilers.llvm.clang.1_0.xcspec
        com.apple.compilers.model.coredata.xcspec
        com.apple.compilers.model.coredatamapping.xcspec
        com.apple.compilers.resource-copier.xcspec
        com.apple.compilers.yacc.xcspec
        DESTINATION Library/Xcode/Specifications)
