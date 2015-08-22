// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_PBX_ReferenceProxy_h
#define __pbxproj_PBX_ReferenceProxy_h

#include <pbxproj/PBX/GroupItem.h>
#include <pbxproj/PBX/ContainerItemProxy.h>

namespace pbxproj { namespace PBX {

class ReferenceProxy : public GroupItem {
public:
    typedef std::shared_ptr <ReferenceProxy> shared_ptr;
    typedef std::vector <shared_ptr> vector;

private:
    std::string                    _fileType;
    ContainerItemProxy::shared_ptr _remoteRef;

public:
    ReferenceProxy();

public:
    inline std::string const &fileType() const
    { return _fileType; }
    inline ContainerItemProxy::shared_ptr const &remoteRef() const
    { return _remoteRef; }

public:
    bool parse(Context &context, plist::Dictionary const *dict);

public:
    static inline char const *Isa()
    { return ISA::PBXReferenceProxy; }
};

} }

#endif  // !__pbxproj_PBX_ReferenceProxy_h
