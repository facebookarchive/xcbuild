// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_PBX_ShellScriptBuildPhase_h
#define __pbxproj_PBX_ShellScriptBuildPhase_h

#include <pbxproj/PBX/BuildPhase.h>

namespace pbxproj { namespace PBX {

class ShellScriptBuildPhase : public BuildPhase {
public:
    typedef std::shared_ptr <ShellScriptBuildPhase> shared_ptr;

private:
    std::string                    _name;
    std::string                    _shellPath;
    std::string                    _shellScript;
    std::vector<pbxsetting::Value> _inputPaths;
    std::vector<pbxsetting::Value> _outputPaths;
    bool                           _showEnvVarsInLog;

public:
    ShellScriptBuildPhase();

public:
    inline std::string const &name() const
    { return _name; }

public:
    inline std::string const &shellPath() const
    { return _shellPath; }

    inline std::string const &shellScript() const
    { return _shellScript; }

public:
    inline std::vector<pbxsetting::Value> const &inputPaths() const
    { return _inputPaths; }
    inline std::vector<pbxsetting::Value> &inputPaths()
    { return _inputPaths; }

public:
    inline std::vector<pbxsetting::Value> const &outputPaths() const
    { return _outputPaths; }
    inline std::vector<pbxsetting::Value> &outputPaths()
    { return _outputPaths; }

public:
    inline bool showEnvVarsInLog() const
    { return _showEnvVarsInLog; }

public:
    virtual bool parse(Context &context, plist::Dictionary const *dict);

public:
    static inline char const *Isa()
    { return ISA::PBXShellScriptBuildPhase; }
};

} }

#endif  // !__pbxproj_PBX_BuildPhase_h
