// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_PBX_BuildRule_h
#define __pbxproj_PBX_BuildRule_h

#include <pbxproj/PBX/Object.h>

namespace pbxproj { namespace PBX {

class BuildRule : public Object {
public:
    typedef std::shared_ptr <BuildRule> shared_ptr;
    typedef std::vector <shared_ptr> vector;

private:
    std::string   _compilerSpec;
    std::string   _filePatterns;
    std::string   _fileType;
    std::string   _script;
    string_vector _outputFiles;
    bool          _isEditable;

public:
    BuildRule();

public:
    inline std::string const &compilerSpec() const
    { return _compilerSpec; }
    
public:
    inline std::string const &filePatterns() const
    { return _filePatterns; }

    inline std::string const &fileType() const
    { return _fileType; }

public:
    inline std::string const &script() const
    { return _script; }

public:
    inline string_vector const &outputFiles() const
    { return _outputFiles; }
    inline string_vector &outputFiles()
    { return _outputFiles; }

public:
    inline bool isEditable() const
    { return _isEditable; }

public:
    bool parse(Context &context, plist::Dictionary const *dict);

public:
    static inline char const *Isa()
    { return ISA::PBXBuildRule; }
};

} }

#endif  // !__pbxproj_PBX_BuildRule_h
