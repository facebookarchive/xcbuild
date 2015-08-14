// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxproj/PlistHelpers.h>

namespace pbxproj {

plist::Dictionary const *
PlistDictionaryGetPBXObject(plist::Dictionary const *dict,
                            std::string const &key,
                            std::string const &isa)
{
    if (isa.empty())
        return nullptr;

    plist::Dictionary const *object = dict->value <plist::Dictionary> (key);
    if (object == nullptr)
        return nullptr;

    plist::String const *isaObject = object->value <plist::String> ("isa");
    if (isaObject != nullptr && isaObject->value() == isa)
        return object;
    else
        return nullptr;
}

plist::Dictionary const *
PlistDictionaryGetIndirectPBXObject(plist::Dictionary const *objects,
                                    plist::Dictionary const *dict,
                                    std::string const &key,
                                    std::string const &isa,
                                    std::string *id)
{
    auto ID = dict->value <plist::String> (key);
    if (ID == nullptr)
        return nullptr;

    if (id != nullptr) {
        *id = ID->value();
    }
    return PlistDictionaryGetPBXObject(objects, ID->value(), isa);
}

}
