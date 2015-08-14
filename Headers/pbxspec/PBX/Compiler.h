// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxspec_PBX_Compiler_h
#define __pbxspec_PBX_Compiler_h

#include <pbxspec/PBX/Tool.h>

namespace pbxspec { namespace PBX {

class Compiler : public Tool {
public:
    typedef std::shared_ptr <Compiler> shared_ptr;
    typedef std::vector <shared_ptr> vector;

protected:
    std::string             _execCPlusPlusLinkerPath;
    std::string             _executionDescription;
    std::string             _sourceFileOption;
    std::string             _outputDir;
    std::string             _outputFileExtension;
    std::string             _commandResultsPostprocessor;
    std::string             _generatedInfoPlistContentFilePath;
    std::string             _dependencyInfoFile;
    libutil::string_vector  _dependencyInfoArgs;
    libutil::string_vector  _languages;
    libutil::string_vector  _optionConditionFlavors;
    libutil::string_vector  _patternsOfFlagsNotAffectingPrecomps;
    libutil::string_vector  _messageCategoryInfoOptions;
    libutil::string_vector  _synthesizeBuildRuleForBuildPhases;
    libutil::string_vector  _inputFileGroupings;
    libutil::string_vector  _fallbackTools;
    plist::Dictionary      *_overridingProperties;
    bool                    _useCPlusPlusCompilerDriverWhenBundlizing;
    bool                    _dashIFlagAcceptHeadermaps;
    bool                    _supportsHeadermaps;
    bool                    _supportsIsysroot;
    bool                    _supportsSeparateUserHeaderPaths;
    bool                    _supportsGeneratePreprocessedFile;
    bool                    _supportsGenerateAssemblyFile;
    bool                    _supportsAnalyzeFile;
    bool                    _supportsSerializedDiagnostics;
    bool                    _supportsPredictiveCompilation;
    bool                    _supportsMacOSXDeploymentTarget;
    bool                    _supportsMacOSXMinVersionFlag;
    bool                    _prunePrecompiledHeaderCache;
    bool                    _outputAreProducts;
    bool                    _outputAreSourceFiles;
    bool                    _softError;
    bool                    _deeplyStatInputDirectories;
    bool                    _dontProcessOutputs;
    bool                    _showInCompilerSelectionPopup;
    bool                    _showOnlySelfDefinedProperties;

protected:
    Compiler(bool isDefault);
    Compiler(bool isDefault, std::string const &isa);

public:
    virtual ~Compiler();

public:
    inline char const *type() const override
    { return Compiler::Type(); }

public:
    inline Compiler::shared_ptr const &base() const
    { return reinterpret_cast <Compiler::shared_ptr const &> (Tool::base()); }

public:
    inline std::string const &execCPlusPlusLinkerPath() const
    { return _execCPlusPlusLinkerPath; }

public:
    inline std::string const &executionDescription() const
    { return _executionDescription; }

public:
    inline std::string const &sourceFileOption() const
    { return _sourceFileOption; }

public:
    inline std::string const &outputDir() const
    { return _outputDir; }
    inline std::string const &outputFileExtension() const
    { return _outputFileExtension; }

public:
    inline std::string const &commandResultsPostprocessor() const
    { return _commandResultsPostprocessor; }

public:
    inline std::string const &generatedInfoPlistContentFilePath() const
    { return _generatedInfoPlistContentFilePath; }

public:
    inline std::string const &dependencyInfoFile() const
    { return _dependencyInfoFile; }
    inline libutil::string_vector const &dependencyInfoArgs() const
    { return _dependencyInfoArgs; }

public:
    inline libutil::string_vector const &languages() const
    { return _languages; }

public:
    inline libutil::string_vector const &optionConditionFlavors() const
    { return _optionConditionFlavors; }

public:
    inline libutil::string_vector const &patternsOfFlagsNotAffectingPrecomps() const
    { return _patternsOfFlagsNotAffectingPrecomps; }

public:
    inline libutil::string_vector const &messageCategoryInfoOptions() const
    { return _messageCategoryInfoOptions; }

public:
    inline libutil::string_vector const &synthesizeBuildRuleForBuildPhases() const
    { return _synthesizeBuildRuleForBuildPhases; }

public:
    inline libutil::string_vector const &inputFileGroupings() const
    { return _inputFileGroupings; }

public:
    inline libutil::string_vector const &fallbackTools() const
    { return _fallbackTools; }

public:
    inline plist::Dictionary const *overridingProperties() const
    { return _overridingProperties; }

public:
    inline bool useCPlusPlusCompilerDriverWhenBundlizing() const
    { return _useCPlusPlusCompilerDriverWhenBundlizing; }

public:
    inline bool dashIFlagAcceptHeadermaps() const
    { return _dashIFlagAcceptHeadermaps; }
    inline bool supportsHeadermaps() const
    { return _supportsHeadermaps; }
    inline bool supportsIsysroot() const
    { return _supportsIsysroot; }
    inline bool supportsSeparateUserHeaderPaths() const
    { return _supportsSeparateUserHeaderPaths; }
    inline bool supportsGeneratePreprocessedFile() const
    { return _supportsGeneratePreprocessedFile; }
    inline bool supportsGenerateAssemblyFile() const
    { return _supportsGenerateAssemblyFile; }
    inline bool supportsAnalyzeFile() const
    { return _supportsAnalyzeFile; }
    inline bool supportsSerializedDiagnostics() const
    { return _supportsSerializedDiagnostics; }
    inline bool supportsPredictiveCompilation() const
    { return _supportsPredictiveCompilation; }
    inline bool supportsMacOSXDeploymentTarget() const
    { return _supportsMacOSXDeploymentTarget; }
    inline bool supportsMacOSXMinVersionFlag() const
    { return _supportsMacOSXMinVersionFlag; }

public:
    inline bool prunePrecompiledHeaderCache() const
    { return _prunePrecompiledHeaderCache; }

public:
    inline bool outputAreProducts() const
    { return _outputAreProducts; }
    inline bool outputAreSourceFiles() const
    { return _outputAreSourceFiles; }

public:
    inline bool softError() const
    { return _softError; }

public:
    inline bool deeplyStatInputDirectories() const
    { return _deeplyStatInputDirectories; }

public:
    inline bool dontProcessOutputs() const
    { return _dontProcessOutputs; }

public:
    inline bool showInCompilerSelectionPopup() const
    { return _showInCompilerSelectionPopup; }
    inline bool showOnlySelfDefinedProperties() const
    { return _showOnlySelfDefinedProperties; }

protected:
    friend class Specification;
    bool parse(plist::Dictionary const *dict) override;

protected:
    bool inherit(Specification::shared_ptr const &base) override;
    bool inherit(Tool::shared_ptr const &base) override;
    virtual bool inherit(Compiler::shared_ptr const &base);

protected:
    static Compiler::shared_ptr Parse(plist::Dictionary const *dict);

public:
    static inline char const *Isa()
    { return ISA::PBXCompiler; }
    static inline char const *Type()
    { return Types::Compiler; }
};

} }

#endif  // !__pbxspec_PBX_Compiler_h
