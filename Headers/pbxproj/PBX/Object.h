/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxproj_PBX_Object_h
#define __pbxproj_PBX_Object_h

#include <pbxproj/Context.h>

namespace pbxproj { namespace PBX {

class Object {
public:
    typedef std::shared_ptr <Object> shared_ptr;
    typedef std::vector <shared_ptr> vector;
    typedef std::map <std::string, shared_ptr> map;

private:
    std::string _isa;
    std::string _blueprintIdentifier;

protected:
    Object(std::string const &isa);

public:
    inline std::string const &blueprintIdentifier() const
    { return _blueprintIdentifier; }
    inline void setBlueprintIdentifier(std::string const &identifier)
    { _blueprintIdentifier = identifier; }

public:
    inline std::string const &isa() const
    { return _isa; }

public:
    virtual inline bool isa(std::string const &isa) const
    { return (_isa == isa); }

private:
    friend class pbxproj::Context;
    bool parseObject(Context &context, plist::Dictionary const *dict);

protected:
    virtual bool parse(Context &context, plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check);

public:
    template <typename T>
    inline bool isa() const
    { return (T::Isa() == isa()); }
};

} }

#endif  // !__pbxproj_PBX_Object_h
