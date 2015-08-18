// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxspec_PBX_PackageType_h
#define __pbxspec_PBX_PackageType_h

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
    plist::Dictionary            *_defaultBuildSettings;
    ProductReference::shared_ptr  _productReference;

protected:
    PackageType(bool isDefault);

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
    inline plist::Dictionary const *defaultBuildSettings() const
    { return _defaultBuildSettings; }

protected:
    friend class Specification;
    bool parse(std::shared_ptr<Manager> manager, plist::Dictionary const *dict) override;

protected:
    bool inherit(Specification::shared_ptr const &base) override;
    virtual bool inherit(PackageType::shared_ptr const &base);

protected:
    static PackageType::shared_ptr Parse(std::shared_ptr<Manager> manager, plist::Dictionary const *dict);

public:
    static inline char const *Isa()
    { return ISA::PBXPackageType; }
    static inline char const *Type()
    { return Types::PackageType; }
};

} }

#endif  // !__pbxspec_PBX_PackageType_h
