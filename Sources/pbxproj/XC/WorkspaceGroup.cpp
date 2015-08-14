// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxproj/XC/WorkspaceGroup.h>
#include <pbxproj/XC/WorkspaceFileRef.h>

using pbxproj::XC::WorkspaceGroup;

WorkspaceGroup::WorkspaceGroup() :
    WorkspaceGroupItem(kTypeGroup)
{
}

bool WorkspaceGroup::
parse(plist::Dictionary const *dict)
{
    if (!WorkspaceGroupItem::parse(dict))
        return false;

    auto N = dict->value <plist::String> ("name");
    if (N != nullptr) {
        _name = N->value();
    }

    if (auto FRd = dict->value <plist::Dictionary> ("FileRef")) {
        auto FR = std::make_shared <WorkspaceFileRef> ();
        if (!FR->parse(FRd))
            return false;

        _items.push_back(FR);
    } else if (auto FRs = dict->value <plist::Array> ("FileRef")) {
        for (size_t n = 0; n < FRs->count(); n++) {
            auto FRd = FRs->value <plist::Dictionary> (n);
            if (FRd == nullptr)
                continue;

            auto FR = std::make_shared <WorkspaceFileRef> ();
            if (!FR->parse(FRd))
                return false;
            
            _items.push_back(FR);
        }
    }

    if (auto Gd = dict->value <plist::Dictionary> ("Group")) {
        auto G = std::make_shared <WorkspaceGroup> ();
        if (!G->parse(Gd))
            return false;

        _items.push_back(G);
    } else if (auto Gs = dict->value <plist::Array> ("Group")) {
        for (size_t n = 0; n < Gs->count(); n++) {
            auto Gd = Gs->value <plist::Dictionary> (n);
            if (Gd == nullptr)
                continue;

            auto G = std::make_shared <WorkspaceGroup> ();
            if (!G->parse(Gd))
                return false;
            
            _items.push_back(G);
        }
    }

    return true;
}
