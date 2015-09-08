// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_TargetBuildRules_h
#define __pbxbuild_TargetBuildRules_h

#include <pbxbuild/Base.h>
#include <pbxbuild/FileTypeResolver.h>

namespace pbxbuild {

class TargetBuildRules {
public:
    class BuildRule {
    public:
        typedef std::shared_ptr <BuildRule> shared_ptr;
        typedef std::vector <shared_ptr> vector;

    private:
        friend class pbxbuild::TargetBuildRules;
        std::string                    _filePatterns;
        pbxspec::PBX::FileType::vector _fileTypes;
        pbxspec::PBX::Tool::shared_ptr _tool;
        std::string                    _script;
        std::vector<pbxsetting::Value> _outputFiles;

    public:
        BuildRule(std::string const &filePatterns, pbxspec::PBX::FileType::vector const &fileTypes, pbxspec::PBX::Tool::shared_ptr const &tool, std::string const &script, std::vector<pbxsetting::Value> const &outputFiles);

    public:
        inline std::string const &filePatterns() const
        { return _filePatterns; }
        inline pbxspec::PBX::FileType::vector const &fileTypes() const
        { return _fileTypes; }

    public:
        inline pbxspec::PBX::Tool::shared_ptr const &tool() const
        { return _tool; }
        inline std::string const &script() const
        { return _script; }

    public:
        inline std::vector<pbxsetting::Value> const &outputFiles() const
        { return _outputFiles; }
    };

private:
    BuildRule::vector _buildRules;

private:
    TargetBuildRules(BuildRule::vector const &buildRules);

public:
    BuildRule::shared_ptr
    resolve(FileTypeResolver const &file) const;

public:
    static TargetBuildRules
    Create(pbxspec::Manager::shared_ptr const &specManager, std::string const &domain, pbxproj::PBX::Target::shared_ptr const &target);
};

}

#endif // !__pbxbuild_DependencyResolver_h
