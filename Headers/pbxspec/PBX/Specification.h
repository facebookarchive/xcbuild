// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxspec_PBX_Specification_h
#define __pbxspec_PBX_Specification_h

#include <pbxspec/PBX/Object.h>

namespace pbxspec { class Manager; }

namespace pbxspec { namespace PBX {

class Specification : public pbxproj::PBX::Object {
public:
    typedef std::shared_ptr <Specification> shared_ptr;
    typedef std::vector <shared_ptr> vector;

protected:
    Specification::shared_ptr _base;
    bool                      _isDefault;
    std::string               _clazz;
    std::string               _identifier;
    std::string               _name;
    std::string               _description;
    std::string               _vendor;
    std::string               _version;

protected:
    Specification(std::string const &isa, bool isDefault);

public:
    virtual char const *type() const = 0;

public:
    inline Specification::shared_ptr const &base() const
    { return _base; }

public:
    inline bool isDefault() const
    { return _isDefault; }

public:
    inline std::string const &clazz() const
    { return _clazz; }
    inline std::string const &identifier() const
    { return _identifier; }
    inline std::string const &name() const
    { return _name; }
    inline std::string const &description() const
    { return _description; }
    inline std::string const &vendor() const
    { return _vendor; }
    inline std::string const &version() const
    { return _version; }

protected:
    virtual bool parse(std::shared_ptr<Manager> manager, plist::Dictionary const *dict);

protected:
    virtual bool inherit(Specification::shared_ptr const &base);

public:
    inline std::string const &isa() const
    { return Object::isa(); }
    bool isa(std::string const &isa) const override;

public:
    friend class pbxspec::Manager;
    static bool Open(std::shared_ptr<Manager> manager, std::string const &filename);

private:
    static Specification::shared_ptr Parse(std::shared_ptr<Manager> manager, plist::Dictionary const *dict);
};

} }

#endif  // !__pbxspec_PBX_Specification_h
