// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxproj/PBX/ReferenceProxy.h>

using pbxproj::PBX::ReferenceProxy;

ReferenceProxy::ReferenceProxy() :
    GroupItem(Isa(), kTypeReferenceProxy)
{
}

bool ReferenceProxy::
parse(Context &context, plist::Dictionary const *dict)
{
    if (!GroupItem::parse(context, dict))
        return false;

    std::string RRID;

    auto RR = context.indirect <ContainerItemProxy> (dict, "remoteRef", &RRID);
    auto FT = dict->value <plist::String> ("fileType");

    if (RR != nullptr) {
        _remoteRef = context.parseObject(context.containerItemProxies, RRID, RR);
        if (_remoteRef == nullptr) {
            abort();
            return false;
        }
    }

    if (FT != nullptr) {
        _fileType = FT->value();
    }

    return true;
}
