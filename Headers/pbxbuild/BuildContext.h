// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_BuildContext_h
#define __pbxbuild_BuildContext_h

#include <pbxbuild/Base.h>

namespace pbxbuild {

class BuildContext {
public:
    typedef std::shared_ptr <BuildContext> shared_ptr;

private:
    pbxspec::Manager::shared_ptr         _specManager;
    std::shared_ptr<xcsdk::SDK::Manager> _sdkManager;
    pbxsetting::Environment              _baseEnvironment;

private:
    BuildContext(pbxspec::Manager::shared_ptr specManager, std::shared_ptr<xcsdk::SDK::Manager> sdkManager, pbxsetting::Environment baseEnvironment);

public:
    pbxspec::Manager::shared_ptr const &specManager() const
    { return _specManager; }
    std::shared_ptr<xcsdk::SDK::Manager> const &sdkManager() const
    { return _sdkManager; }

public:
    pbxsetting::Environment const &baseEnvironment() const
    { return _baseEnvironment; }

public:
    static BuildContext::shared_ptr
    Create(pbxspec::Manager::shared_ptr specManager, std::shared_ptr<xcsdk::SDK::Manager> sdkManager, pbxsetting::Environment baseEnvironment);
    static BuildContext::shared_ptr
    Default(void);
};

}

#endif // !__pbxbuild_BuildContext_h
