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
        pbxsetting::Setting::Create("ProductResourcesDir", pbxsetting::Value::Variable("UNLOCALIZED_RESOURCES_FOLDER_PATH")),
        pbxsetting::Setting::Create("DerivedFilesDir", pbxsetting::Value::Variable("DERIVED_FILES_DIR")),
        // TODO(grp): AdditionalContentFilePaths
        // TODO(grp): AdditionalFlags
        // TODO(grp): BitcodeArch
        // TODO(grp): StaticAnalyzerModeNameDescription
        // TOOD(grp): CommandProgressByType
    };

    if (tool->type() == pbxspec::PBX::Compiler::Type()) {
        pbxspec::PBX::Compiler::shared_ptr const &compiler = std::static_pointer_cast<pbxspec::PBX::Compiler>(tool);

        std::vector<pbxsetting::Setting> compilerSettings = {
            pbxsetting::Setting::Create("OutputDir", (!compiler->outputDir().raw().empty() ? compiler->outputDir() : pbxsetting::Value::String(FSUtil::GetDirectoryName(output)))),
            pbxsetting::Setting::Create("DependencyInfoFile", compiler->dependencyInfoFile()),
        };
        toolSettings.insert(toolSettings.end(), compilerSettings.begin(), compilerSettings.end());
    }

    pbxsetting::Environment toolEnvironment = environment;
    toolEnvironment.insertFront(tool->defaultSettings(), true);
    toolEnvironment.insertFront(pbxsetting::Level(toolSettings), false);

    return ToolEnvironment(tool, toolEnvironment, inputs, outputs);
}

