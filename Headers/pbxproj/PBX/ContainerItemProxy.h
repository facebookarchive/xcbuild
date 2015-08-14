// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_PBX_ContainerItemProxy_h
#define __pbxproj_PBX_ContainerItemProxy_h

#include <pbxproj/PBX/Project.h>
#include <pbxproj/PBX/Target.h>

namespace pbxproj { namespace PBX {

class ContainerItemProxy : public Object {
public:
    typedef std::shared_ptr <ContainerItemProxy> shared_ptr;

private:
    Project            *_containerPortal;
    uint32_t            _proxyType;
    std::string         _remoteGlobalIDString;
    std::string         _remoteInfo;
    Target::shared_ptr  _remoteTarget;

public:
    ContainerItemProxy();

public:
    inline Project const *containerPortal() const
    { return _containerPortal; }
    inline Project *containerPortal()
    { return _containerPortal; }

public:
    inline uint32_t proxyType() const
    { return _proxyType; }

public:
    inline std::string const &remoteGlobalIDString() const
    { return _remoteGlobalIDString; }

    inline std::string const &remoteInfo() const
    { return _remoteInfo; }

public:
    inline Target::shared_ptr const &remoteTarget() const
    { return _remoteTarget; }
    inline Target::shared_ptr &remoteTarget()
    { return _remoteTarget; }

public:
    bool parse(Context &context, plist::Dictionary const *dict);

public:
    static inline char const *Isa()
    { return ISA::PBXContainerItemProxy; }
};

} }

#endif  // !__pbxproj_PBX_ContainerItemProxy_h
