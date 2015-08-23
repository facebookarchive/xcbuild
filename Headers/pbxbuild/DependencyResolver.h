// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_DependencyResolver_h
#define __pbxbuild_DependencyResolver_h

#include <pbxbuild/Base.h>
#include <pbxbuild/SchemeContext.h>
#include <pbxbuild/BuildGraph.h>

namespace pbxbuild {

class DependencyResolver {
private:
    SchemeContext::shared_ptr _context;

public:
    DependencyResolver(SchemeContext::shared_ptr context);
    ~DependencyResolver();

public:
    BuildGraph
    resolveDependencies(void) const;
};

}

#endif // !__pbxbuild_DependencyResolver_h
