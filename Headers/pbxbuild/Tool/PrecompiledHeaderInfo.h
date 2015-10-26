// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_PrecompiledHeaderInfo_h
#define __pbxbuild_PrecompiledHeaderInfo_h

#include <pbxbuild/Base.h>

namespace pbxbuild {
namespace Tool {

class PrecompiledHeaderInfo {
private:
    std::string                        _prefixHeader;
    pbxspec::PBX::FileType::shared_ptr _fileType;
    std::vector<std::string>           _arguments;
    std::vector<std::string>           _relevantArguments;

public:
    PrecompiledHeaderInfo(std::string const &prefixHeader, pbxspec::PBX::FileType::shared_ptr const &fileType, std::vector<std::string> const &arguments, std::vector<std::string> const &relevantArguments);
    ~PrecompiledHeaderInfo();

public:
    std::string const &prefixHeader() const
    { return _prefixHeader; }
    pbxspec::PBX::FileType::shared_ptr const &fileType() const
    { return _fileType; }
    std::vector<std::string> const &arguments() const
    { return _arguments; }
    std::vector<std::string> const &relevantArguments() const
    { return _relevantArguments; }

public:
    std::string hash() const;
    std::string serialize() const;

public:
    pbxsetting::Value logicalOutputPath() const;
    pbxsetting::Value compileOutputPath() const;
    pbxsetting::Value serializedOutputPath() const;

public:
    static PrecompiledHeaderInfo
    Create(pbxspec::PBX::Compiler::shared_ptr const &compiler, std::string const &prefixHeader, pbxspec::PBX::FileType::shared_ptr const &fileType, std::vector<std::string> const &arguments);
};

}
}

#endif // !__pbxbuild_SearchPaths_h
