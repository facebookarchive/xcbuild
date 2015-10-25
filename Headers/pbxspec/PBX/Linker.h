// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxspec_PBX_Linker_h
#define __pbxspec_PBX_Linker_h

#include <pbxspec/PBX/Tool.h>

namespace pbxspec { namespace PBX {

class Linker : public Tool {
public:
    typedef std::shared_ptr <Linker> shared_ptr;
    typedef std::vector <shared_ptr> vector;

protected:
    libutil::string_vector _binaryFormats;
    pbxsetting::Value      _dependencyInfoFile;
    bool                   _supportsInputFileList;

protected:
    Linker();

public:
    virtual ~Linker();

public:
    inline char const *type() const override
    { return Linker::Type(); }

public:
    inline Linker::shared_ptr const &base() const
    { return reinterpret_cast <Linker::shared_ptr const &> (Tool::base()); }

public:
    inline libutil::string_vector const &architectures() const
    { return _architectures; }

public:
    inline libutil::string_vector const &binaryFormats() const
    { return _binaryFormats; }

public:
    inline pbxsetting::Value const &dependencyInfoFile() const
    { return _dependencyInfoFile; }

public:
    inline bool supportsInputFileList() const
    { return _supportsInputFileList; }

protected:
    bool inherit(Specification::shared_ptr const &base) override;
    bool inherit(Tool::shared_ptr const &base) override;
    virtual bool inherit(Linker::shared_ptr const &base);

protected:
    friend class Specification;
    bool parse(Context *context, plist::Dictionary const *dict) override;

protected:
    static Linker::shared_ptr Parse(Context *context, plist::Dictionary const *dict);

public:
    static inline char const *Type()
    { return Types::Linker; }
};

} }

#endif  // !__pbxspec_PBX_Linker_h
