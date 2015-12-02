/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxspec_PBX_Specification_h
#define __pbxspec_PBX_Specification_h

#include <pbxspec/Base.h>
#include <pbxspec/Types.h>

namespace pbxspec { class Manager; }

namespace pbxspec { namespace PBX {

class Specification {
public:
    typedef std::shared_ptr <Specification> shared_ptr;
    typedef std::vector <shared_ptr> vector;

protected:
    Specification::shared_ptr _base;
    std::string               _clazz;
    std::string               _identifier;
    std::string               _domain;
    bool                      _isGlobalDomainInUI;
    std::string               _name;
    std::string               _description;
    std::string               _vendor;
    std::string               _version;

protected:
    Specification();

public:
    virtual char const *type() const = 0;

public:
    inline Specification::shared_ptr const &base() const
    { return _base; }

public:
    inline std::string const &clazz() const
    { return _clazz; }
    inline std::string const &identifier() const
    { return _identifier; }

public:
    inline std::string const &domain() const
    { return _domain; }
    inline bool isGlobalDomainInUI() const
    { return _isGlobalDomainInUI; }

public:
    inline std::string const &name() const
    { return _name; }
    inline std::string const &description() const
    { return _description; }
    inline std::string const &vendor() const
    { return _vendor; }
    inline std::string const &version() const
    { return _version; }

protected:
    virtual bool parse(Context *context, plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check);

protected:
    virtual bool inherit(Specification::shared_ptr const &base);

public:
    friend class pbxspec::Manager;
    static bool Open(Context *context, std::string const &filename);

private:
    static Specification::shared_ptr Parse(Context *context, plist::Dictionary const *dict);
};

} }

#endif  // !__pbxspec_PBX_Specification_h
