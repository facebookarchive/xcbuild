// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxspec_PBX_Architecture_h
#define __pbxspec_PBX_Architecture_h

#include <pbxsetting/pbxsetting.h>
#include <pbxspec/PBX/Specification.h>

namespace pbxspec { namespace PBX {

class Architecture : public Specification {
public:
    typedef std::shared_ptr <Architecture> shared_ptr;
    typedef std::vector <shared_ptr> vector;

protected:
    libutil::string_vector _realArchitectures;
    std::string            _architectureSetting;
    std::string            _perArchBuildSettingName;
    std::string            _byteOrder;
    bool                   _listInEnum;
    int                    _sortNumber;

protected:
    Architecture();

public:
    virtual ~Architecture();

public:
    inline char const *type() const override
    { return Architecture::Type(); }

public:
    inline Architecture::shared_ptr const &base() const
    { return reinterpret_cast <Architecture::shared_ptr const &> (Specification::base()); }

public:
    inline libutil::string_vector const &realArchitectures() const
    { return _realArchitectures; }

public:
    inline std::string const &architectureSetting() const
    { return _architectureSetting; }
    inline std::string const &perArchBuildSettingName() const
    { return _perArchBuildSettingName; }

public:
    inline std::string const &byteOrder() const
    { return _byteOrder; }

public:
    inline bool listInEnum() const
    { return _listInEnum; }
    inline int sortNumber() const
    { return _sortNumber; }

public:
    pbxsetting::Setting defaultSetting(void) const;

protected:
    friend class Specification;
    bool parse(Context *context, plist::Dictionary const *dict) override;

protected:
    bool inherit(Specification::shared_ptr const &base) override;
    virtual bool inherit(Architecture::shared_ptr const &base);

protected:
    static Architecture::shared_ptr Parse(Context *context, plist::Dictionary const *dict);

public:
    static inline char const *Type()
    { return Types::Architecture; }
};

} }

#endif  // !__pbxspec_PBX_Architecture_h
