// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_BuildGraph_h
#define __pbxbuild_BuildGraph_h

#include <pbxbuild/Base.h>
#include <list>

namespace pbxbuild {

class BuildGraph {
private:
    std::map<pbxproj::PBX::Target::shared_ptr, std::vector<pbxproj::PBX::Target::shared_ptr>> _contents;

public:
    void
    insert(pbxproj::PBX::Target::shared_ptr const &node, std::vector<pbxproj::PBX::Target::shared_ptr> const &children);

public:
    std::vector<pbxproj::PBX::Target::shared_ptr>
    children(pbxproj::PBX::Target::shared_ptr const &node) const;

public:
    std::vector<pbxproj::PBX::Target::shared_ptr>
    ordered(void) const;
};

}

#endif // !__pbxbuild_BuildGraph_h
