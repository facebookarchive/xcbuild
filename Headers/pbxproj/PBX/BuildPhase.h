// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_PBX_BuildPhase_h
#define __pbxproj_PBX_BuildPhase_h

#include <pbxproj/PBX/BuildFile.h>

namespace pbxproj { namespace PBX {

class BuildPhase : public Object {
public:
    typedef std::shared_ptr <BuildPhase> shared_ptr;
    typedef std::vector <shared_ptr> vector;

public:
    enum Type {
        kTypeHeaders,
        kTypeSources,
        kTypeResources,
        kTypeFrameworks,
        kTypeCopyFiles,
        kTypeShellScript,
        kTypeAppleScript,
    };

    enum ActionMask {
        // TODO(grp): This is incomplete.
        kActionBuild   = 4,
        kActionInstall = 8,
    };

private:
    Type              _type;
    std::string       _name;
    BuildFile::vector _files;
    bool              _runOnlyForDeploymentPostprocessing;
    uint32_t          _buildActionMask;

protected:
    BuildPhase(std::string const &isa, Type type);

public:
    inline Type type() const
    { return _type; }

public:
    inline std::string const &name() const
    { return _name; }

public:
    inline BuildFile::vector const &files() const
    { return _files; }
    inline BuildFile::vector &files()
    { return _files; }

public:
    inline bool runOnlyForDeploymentPostprocessing() const
    { return _runOnlyForDeploymentPostprocessing; }

public:
    inline uint32_t buildActionMask() const
    { return _buildActionMask; }

public:
    virtual bool parse(Context &context, plist::Dictionary const *dict);
};

} }

#endif  // !__pbxproj_PBX_BuildPhase_h
