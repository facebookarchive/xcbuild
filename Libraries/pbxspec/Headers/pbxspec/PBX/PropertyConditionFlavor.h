/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxspec_PBX_PropertyConditionFlavor_h
#define __pbxspec_PBX_PropertyConditionFlavor_h

#include <pbxspec/PBX/Specification.h>

#include <memory>
#include <vector>
#include <ext/optional>

namespace plist { class Dictionary; }

namespace pbxspec { namespace PBX {

class PropertyConditionFlavor : public Specification {
public:
    typedef std::shared_ptr <PropertyConditionFlavor> shared_ptr;
    typedef std::vector <shared_ptr> vector;

protected:
    ext::optional<int> _precedence;

protected:
    PropertyConditionFlavor();

public:
    virtual ~PropertyConditionFlavor();

public:
    inline char const *type() const override
    { return PropertyConditionFlavor::Type(); }

public:
    inline PropertyConditionFlavor::shared_ptr base() const
    { return std::static_pointer_cast<PropertyConditionFlavor>(Specification::base()); }

public:
    inline ext::optional<int> precedence() const
    { return _precedence; }

protected:
    friend class Specification;
    bool parse(Context *context, plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check) override;

protected:
    bool inherit(Specification::shared_ptr const &base) override;
    virtual bool inherit(PropertyConditionFlavor::shared_ptr const &base);

protected:
    static PropertyConditionFlavor::shared_ptr Parse(Context *context, plist::Dictionary const *dict);

public:
    static inline char const *Type()
    { return Types::PropertyConditionFlavor; }
};

} }

#endif  // !__pbxspec_PBX_PropertyConditionFlavor_h
