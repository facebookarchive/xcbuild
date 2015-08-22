// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxproj/PBX/ContainerItemProxy.h>

#include <pbxproj/PBX/NativeTarget.h>
#include <pbxproj/PBX/LegacyTarget.h>

using pbxproj::PBX::ContainerItemProxy;

ContainerItemProxy::ContainerItemProxy() :
    Object    (Isa()),
    _proxyType(0)
{
}

bool ContainerItemProxy::
parse(Context &context, plist::Dictionary const *dict)
{
    std::string CPID;

    auto CP   = context.indirect <FileReference> (dict, "containerPortal", &CPID);
    auto PT   = dict->value <plist::Integer> ("proxyType");
    auto RGIS = dict->value <plist::String> ("remoteGlobalIDString");
    auto RI   = dict->value <plist::String> ("remoteInfo");

    if (CP != nullptr) {
        auto portal = context.parseObject(context.fileReferences, CPID, CP);
        if (!portal) {
            abort();
            return false;
        }

        _containerPortal = portal;
    }

    if (PT != nullptr) {
        _proxyType = PT->value();
    }

    if (RGIS != nullptr) {
        _remoteGlobalIDString = RGIS->value();
    }

    if (RI != nullptr) {
        _remoteInfo = RI->value();
    }

    return true;
}
