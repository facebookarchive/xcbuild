// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_JSHelpers_h
#define __pbxproj_JSHelpers_h

#include <pbxproj/Base.h>

namespace pbxproj {

plist::Dictionary const *
PlistDictionaryGetPBXObject(plist::Dictionary const *dict,
                            std::string const &key,
                            std::string const &isa);

plist::Dictionary const *
PlistDictionaryGetIndirectPBXObject(plist::Dictionary const *objects,
                                    plist::Dictionary const *dict,
                                    std::string const &key,
                                    std::string const &isa,
                                    std::string *id);

}

#endif  // !__pbxproj_JSHelpers_h
