// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/Tool/ToolEnvironment.h>
#include <sstream>

using pbxbuild::Tool::ToolEnvironment;
using libutil::FSUtil;

ToolEnvironment::
ToolEnvironment(pbxspec::PBX::Tool::shared_ptr const &tool, pbxsetting::Environment const &toolEnvironment, std::vector<std::string> const &inputs, std::vector<std::string> const &outputs) :
    _tool           (tool),
    _toolEnvironment(toolEnvironment),
    _inputs         (inputs),
    _outputs        (outputs)
{
}

ToolEnvironment::
~ToolEnvironment()
{
}

ToolEnvironment ToolEnvironment::
Create(pbxspec::PBX::Tool::shared_ptr const &tool, pbxsetting::Environment const &environment, std::vector<std::string> const &inputs, std::vector<std::string> const &outputs)
{
    // TODO(grp); Match inputs with allowed tool input file types.

    std::string input = (!inputs.empty() ? inputs.front() : "");
    std::string output = (!outputs.empty() ? outputs.front() : "");

    std::vector<pbxsetting::Setting> toolSettings = {
        pbxsetting::Setting::Parse("InputPath", input),
        pbxsetting::Setting::Parse("InputFileName", FSUtil::GetBaseName(input)),
        pbxsetting::Setting::Parse("InputFileBase", FSUtil::GetBaseNameWithoutExtension(input)),
        pbxsetting::Setting::Parse("InputFileRelativePath", input), // TODO(grp): Relative.
        pbxsetting::Setting::Parse("InputFileBaseUniquefier", ""), // TODO(grp): Uniqueify.
        pbxsetting::Setting::Parse("OutputPath", output),
        pbxsetting::Setting::Parse("OutputFileName", FSUtil::GetBaseName(output)),
        pbxsetting::Setting::Parse("OutputFileBase", FSUtil::GetBaseNameWithoutExtension(output)),
        // TODO(grp): OutputDir (including tool->outputDir())
        // TODO(grp): AdditionalContentFilePaths
        // TODO(grp): AdditionalFlags
        // TODO(grp): ProductResourcesDir
        // TODO(grp): BitcodeArch
        // TODO(grp): StaticAnalyzerModeNameDescription
        // TODO(grp): DependencyInfoFile (from tool->dependencyInfoFile())
        // TOOD(grp): CommandProgressByType
        pbxsetting::Setting::Parse("DerivedFilesDir", environment.resolve("DERIVED_FILES_DIR")),
    };

    pbxsetting::Environment toolEnvironment = environment;
    toolEnvironment.insertFront(tool->defaultSettings(), true);
    toolEnvironment.insertFront(pbxsetting::Level(toolSettings), false);

    return ToolEnvironment(tool, toolEnvironment, inputs, outputs);
}

