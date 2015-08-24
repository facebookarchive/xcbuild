// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxproj/PBX/TargetDependency.h>
#include <pbxproj/PBX/ContainerItemProxy.h>
#include <pbxproj/PBX/NativeTarget.h>
#include <pbxproj/PBX/AggregateTarget.h>
#include <pbxproj/PBX/LegacyTarget.h>

using pbxproj::PBX::TargetDependency;

TargetDependency::TargetDependency() :
    Object(Isa())
{
}

bool TargetDependency::
parse(Context &context, plist::Dictionary const *dict)
{
    std::string TID;
    std::string TPID;

    if (auto T = context.indirect <NativeTarget> (dict, "target", &TID)) {
        _target = context.parseObject(context.nativeTargets, TID, T);
        if (!_target) {
            abort();
            return false;
        }
    } else if (auto T = context.indirect <AggregateTarget> (dict, "target", &TID)) {
        _target = context.parseObject(context.aggregateTargets, TID, T);
        if (!_target) {
            abort();
            return false;
        }
    } else if (auto T = context.indirect <LegacyTarget> (dict, "target", &TID)) {
        _target = context.parseObject(context.legacyTargets, TID, T);
        if (!_target) {
            abort();
            return false;
        }
    }

    auto TP = context.indirect <ContainerItemProxy> (dict, "targetProxy", &TPID);
    if (TP != nullptr) {
        _targetProxy = context.parseObject(context.containerItemProxies, TPID, TP);
        if (!_targetProxy) {
            abort();
            return false;
        }
    }

    return true;
}
