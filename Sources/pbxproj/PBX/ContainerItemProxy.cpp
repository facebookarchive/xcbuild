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

    auto CP   = context.indirect <Project> (dict, "containerPortal", &CPID);
    auto PT   = dict->value <plist::Integer> ("proxyType");
    auto RGIS = dict->value <plist::String> ("remoteGlobalIDString");
    auto RI   = dict->value <plist::String> ("remoteInfo");

    if (CP != nullptr) {
        auto portal = context.parseObject(context.projects, CPID, CP);
        if (!portal) {
            abort();
            return false;
        }

        _containerPortal = portal.get();
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

    if (!_remoteGlobalIDString.empty()) {
        if (auto RG = context.get <NativeTarget> (_remoteGlobalIDString)) {
            _remoteTarget = context.parseObject(context.nativeTargets,
                    _remoteGlobalIDString, RG);
        } else if (auto RG = context.get <LegacyTarget> (_remoteGlobalIDString)) {
            _remoteTarget = context.parseObject(context.legacyTargets,
                    _remoteGlobalIDString, RG);
        }
#if 0
        if (!_remoteTarget) {
            abort();
            return false;
        }
#endif
    }

    return true;
}
