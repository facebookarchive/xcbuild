/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxproj/PBX/BaseGroup.h>
#include <pbxproj/PBX/Group.h>
#include <pbxproj/PBX/VariantGroup.h>
#include <pbxproj/XC/VersionGroup.h>
#include <pbxproj/PBX/FileReference.h>
#include <pbxproj/PBX/ReferenceProxy.h>

using pbxproj::PBX::BaseGroup;

BaseGroup::BaseGroup(std::string const &isa, GroupItem::Type type) :
    GroupItem(isa, type)
{
}

bool BaseGroup::
parse(Context &context, plist::Dictionary const *dict)
{
    if (!GroupItem::parse(context, dict))
        return false;

    auto Cs = dict->value <plist::Array> ("children");

    if (Cs != nullptr) {
        for (size_t n = 0; n < Cs->count(); n++) {
            auto ID = Cs->value <plist::String> (n);
            if (ID == nullptr)
                continue;

            if (auto C = context.get <Group> (ID)) {
                auto O = context.parseObject(context.groups, ID->value(), C);
                if (!O) {
                    abort();
                    return false;
                }

                O->_parent = this;
                _children.push_back(O);
            } else if (auto C = context.get <VariantGroup> (ID)) {
                auto O = context.parseObject(context.variantGroups, ID->value(), C);
                if (!O) {
                    abort();
                    return false;
                }

                O->_parent = this;
                _children.push_back(O);
            } else if (auto C = context.get <XC::VersionGroup> (ID)) {
                auto O = context.parseObject(context.versionGroups, ID->value(), C);
                if (!O) {
                    abort();
                    return false;
                }

                O->_parent = this;
                _children.push_back(O);
            } else if (auto C = context.get <FileReference> (ID)) {
                auto O = context.parseObject(context.fileReferences, ID->value(), C);
                if (!O) {
                    abort();
                    return false;
                }

                O->_parent = this;
                _children.push_back(O);
            } else if (auto C = context.get <ReferenceProxy> (ID)) {
                auto O = context.parseObject(context.referenceProxies, ID->value(), C);
                if (!O) {
                    abort();
                    return false;
                }

                O->_parent = this;
                _children.push_back(O);
            } else if (context.objects->value(ID->value()) != nullptr) {
                fprintf(stderr, "warning: group '%s' contains unsupported child reference to '%s'\n",
                        _name.c_str(), ID->value().c_str());
            }
        }
    }

    return true;
}
