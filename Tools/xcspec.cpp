// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/pbxspec.h>

#include <cstdio>

using pbxspec::Manager;
using pbxspec::PBX::Specification;
using libutil::FSUtil;

int
main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage: %s path\n", argv[0]);
        return -1;
    }

    std::string path = argv[1];
    Manager::shared_ptr manager = Manager::Create();
    manager->registerDomain({ "xcspec", path });

    return 0;
}
