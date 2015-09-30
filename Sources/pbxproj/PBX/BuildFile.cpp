// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxproj/PBX/BuildFile.h>
#include <pbxproj/PBX/FileReference.h>
#include <pbxproj/PBX/ReferenceProxy.h>
#include <pbxproj/PBX/Group.h>
#include <pbxproj/PBX/VariantGroup.h>
#include <pbxproj/XC/VersionGroup.h>

using pbxproj::PBX::BuildFile;

BuildFile::BuildFile() :
    Object   (Isa())
{
}

BuildFile::~BuildFile()
{
}

bool BuildFile::
parse(Context &context, plist::Dictionary const *dict)
{
    std::string FRID;
    std::string RPID;
    std::string GID;
    std::string VaGID;
    std::string VrGID;

    auto FR  = context.indirect <FileReference> (dict, "fileRef", &FRID);
    auto RP  = context.indirect <ReferenceProxy> (dict, "fileRef", &RPID);
    auto G   = context.indirect <Group> (dict, "fileRef", &GID);
    auto VaG = context.indirect <VariantGroup> (dict, "fileRef", &VaGID);
    auto VrG = context.indirect <XC::VersionGroup> (dict, "fileRef", &VrGID);
    auto S   = dict->value <plist::Dictionary> ("settings");

    if (FR != nullptr) {
        FileReference::shared_ptr fileReference = context.parseObject(context.fileReferences, FRID, FR);
        _fileRef = std::static_pointer_cast <GroupItem> (fileReference);
    } else if (RP != nullptr) {
        ReferenceProxy::shared_ptr referenceProxy = context.parseObject(context.referenceProxies, RPID, RP);
        _fileRef = std::static_pointer_cast <GroupItem> (referenceProxy);
    } else if (G != nullptr) {
        Group::shared_ptr group = context.parseObject(context.groups, GID, G);
        _fileRef = std::static_pointer_cast <GroupItem> (group);
    } else if (VaG != nullptr) {
        VariantGroup::shared_ptr variantGroup = context.parseObject(context.variantGroups, VaGID, VaG);
        _fileRef = std::static_pointer_cast <GroupItem> (variantGroup);
    } else if (VrG != nullptr) {
        XC::VersionGroup::shared_ptr versionGroup = context.parseObject(context.versionGroups, VrGID, VrG);
        _fileRef = std::static_pointer_cast <GroupItem> (versionGroup);
    }

    if (S != nullptr) {
        if (auto CF = S->value <plist::String> ("COMPILER_FLAGS")) {
            _compilerFlags = pbxsetting::Type::ParseList(CF->value());
        }

        if (auto A = S->value <plist::Array> ("ATTRIBUTES")) {
            for (size_t n = 0; n < A->count(); n++) {
                if (auto AA = A->value <plist::String> (n)) {
                    _attributes.push_back(AA->value());
                }
            }
        }
    }

    return true;
}
