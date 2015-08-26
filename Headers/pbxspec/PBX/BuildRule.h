// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxspec_PBX_BuildRule_h
#define __pbxspec_PBX_BuildRule_h

#include <pbxspec/PBX/Specification.h>

namespace pbxspec { class Manager; }

namespace pbxspec { namespace PBX {

class BuildRule {
public:
    typedef std::shared_ptr <BuildRule> shared_ptr;
    typedef std::vector <shared_ptr> vector;

private:
    std::string            _name;
    libutil::string_vector _fileTypes;
    std::string            _compilerSpec;

protected:
    friend class pbxspec::Manager;
    BuildRule();
    BuildRule(libutil::string_vector const &fileTypes, std::string const &compilerSpec);

public:
    inline std::string const &name() const
    { return _name; }
    inline libutil::string_vector const &fileTypes() const
    { return _fileTypes; }
    inline std::string const &compilerSpec() const
    { return _compilerSpec; }

protected:
    friend class pbxspec::Manager;
    bool parse(plist::Dictionary const *dict);
};

} }

#endif  // !__pbxspec_PBX_BuildRule_h
