/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxspec_PBX_PackageType_h
#define __pbxspec_PBX_PackageType_h

#include <pbxsetting/pbxsetting.h>
#include <pbxspec/PBX/Specification.h>

namespace pbxspec { namespace PBX {

class PackageType : public Specification {
public:
    typedef std::shared_ptr <PackageType> shared_ptr;
    typedef std::vector <shared_ptr> vector;

public:
    class ProductReference {
    public:
        typedef std::shared_ptr <ProductReference> shared_ptr;
        typedef std::vector <shared_ptr> vector;

    protected:
        std::string _name;
        std::string _fileType;
        bool        _isLaunchable;

    protected:
        friend class PackageType;
        ProductReference();

    public:
        inline std::string const &name() const
        { return _name; }

    public:
        inline std::string const &fileType() const
        { return _fileType; }

    public:
        inline bool isLaunchable() const
        { return _isLaunchable; }

    protected:
        bool parse(plist::Dictionary const *dict);
    };

protected:
    pbxsetting::Level             _defaultBuildSettings;
    ProductReference::shared_ptr  _productReference;

protected:
    PackageType();

public:
    virtual ~PackageType();

public:
    inline char const *type() const override
    { return PackageType::Type(); }

public:
    inline PackageType::shared_ptr const &base() const
    { return reinterpret_cast <PackageType::shared_ptr const &> (Specification::base()); }

public:
    inline ProductReference::shared_ptr const &productReference() const
    { return _productReference; }

public:
    inline pbxsetting::Level const &defaultBuildSettings() const
    { return _defaultBuildSettings; }

protected:
    friend class Specification;
    bool parse(Context *context, plist::Dictionary const *dict) override;

protected:
    bool inherit(Specification::shared_ptr const &base) override;
    virtual bool inherit(PackageType::shared_ptr const &base);

protected:
    static PackageType::shared_ptr Parse(Context *context, plist::Dictionary const *dict);

public:
    static inline char const *Type()
    { return Types::PackageType; }
};

} }

#endif  // !__pbxspec_PBX_PackageType_h
