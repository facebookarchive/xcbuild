/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxspec_PBX_FileType_h
#define __pbxspec_PBX_FileType_h

#include <pbxsetting/pbxsetting.h>
#include <pbxspec/PBX/Specification.h>

namespace pbxspec { namespace PBX {

class FileType : public Specification {
public:
    typedef std::shared_ptr <FileType> shared_ptr;
    typedef std::vector <shared_ptr> vector;

public:
    class ComponentPart {
    public:
        typedef std::shared_ptr <ComponentPart> shared_ptr;
        typedef std::map <std::string, shared_ptr> map;

    protected:
        std::string            _type;
        std::string            _location;
        libutil::string_vector _identifiers;
        shared_ptr             _reference;

    protected:
        friend class FileType;
        ComponentPart();

    public:
        inline std::string const &type() const
        { return _type; }

    public:
        inline std::string const &location() const
        { return _location; }

    public:
        inline libutil::string_vector const &identifiers() const
        { return _identifiers; }

    public:
        inline shared_ptr const &reference() const
        { return _reference; }

    protected:
        void reset();
        bool parse(FileType *ftype, plist::Array const *array);
    };

public:
    class BuildPhaseInjection {
    protected:
        std::string       _buildPhase;
        std::string       _name;
        bool              _runOnlyForDeploymentPostprocessing;
        bool              _needsRunpathSearchPathForFrameworks;
        int               _dstSubfolderSpec;
        pbxsetting::Value _dstPath;

    protected:
        friend class FileType;
        BuildPhaseInjection();

    public:
        inline std::string const &buildPhase() const
        { return _buildPhase; }
        inline std::string const &name() const
        { return _name; }

    public:
        inline bool runOnlyForDeploymentPostprocessing() const
        { return _runOnlyForDeploymentPostprocessing; }
        inline bool needsRunpathSearchPathForFrameworks() const
        { return _needsRunpathSearchPathForFrameworks; }

    public:
        inline int dstSubfolderSpec() const
        { return _dstSubfolderSpec; }
        inline pbxsetting::Value const &dstPath() const
        { return _dstPath; }

    protected:
        bool parse(plist::Dictionary const *dict);
    };

protected:
    friend class ComponentPart;
    friend class BuildPhaseInjection;

protected:
    std::string            _uti;
    std::string            _language;
    std::string            _computerLanguage;
    std::string            _gccDialectName;
    std::string            _plistStructureDefinition;
    std::string            _permissions;
    libutil::string_vector _extensions;
    libutil::string_vector _mimeTypes;
    libutil::string_vector _typeCodes;
    libutil::string_vector _filenamePatterns;
    std::vector<std::vector<uint8_t>> _magicWords;
    libutil::string_vector _extraPropertyNames;
    libutil::string_vector _prefix;
    ComponentPart::map     _componentParts;
    std::vector<BuildPhaseInjection>  _buildPhaseInjectionsWhenEmbedding;
    bool                   _isTextFile;
    bool                   _isBuildPropertiesFile;
    bool                   _isSourceCode;
    bool                   _isPreprocessed;
    bool                   _isTransparent;
    bool                   _isDocumentation;
    bool                   _isEmbeddable;
    bool                   _isExecutable;
    bool                   _isExecutableWithGUI;
    bool                   _isApplication;
    bool                   _isBundle;
    bool                   _isLibrary;
    bool                   _isDynamicLibrary;
    bool                   _isStaticLibrary;
    bool                   _isFolder;
    bool                   _isWrappedFolder;
    bool                   _isFrameworkWrapper;
    bool                   _isStaticFrameworkWrapper;
    bool                   _isProjectWrapper;
    bool                   _isTargetWrapper;
    bool                   _isScannedForIncludes;
    bool                   _includeInIndex;
    bool                   _canSetIncludeInIndex;
    bool                   _requiresHardTabs;
    bool                   _containsNativeCode;
    bool                   _appliesToBuildRules;
    bool                   _changesCauseDependencyGraphInvalidation;
    std::string            _fallbackAutoroutingBuildPhase;
    bool                   _codeSignOnCopy;
    bool                   _removeHeadersOnCopy;
    bool                   _validateOnCopy;

protected:
    FileType();

public:
    virtual ~FileType();

public:
    inline char const *type() const override
    { return FileType::Type(); }

public:
    inline FileType::shared_ptr const &base() const
    { return reinterpret_cast <FileType::shared_ptr const &> (Specification::base()); }

public:
    inline std::string const &UTI() const
    { return _uti; }

public:
    inline libutil::string_vector const &extensions() const
    { return _extensions; }
    inline libutil::string_vector const &MIMETypes() const
    { return _mimeTypes; }
    inline libutil::string_vector const &typeCodes() const
    { return _typeCodes; }
    inline libutil::string_vector const &filenamePatterns() const
    { return _filenamePatterns; }
    std::vector<std::vector<uint8_t>> const &magicWords() const
    { return _magicWords; }

public:
    inline std::string const &language() const
    { return _language; }
    inline std::string const &computerLanguage() const
    { return _computerLanguage; }

public:
    inline std::string const &GCCDialectName() const
    { return _gccDialectName; }

public:
    inline std::string const &plistStructureDefinition() const
    { return _plistStructureDefinition; }

public:
    inline bool isTextFile() const
    { return _isTextFile; }
    inline bool isTransparent() const
    { return _isTransparent; }

public:
    inline bool isBuildPropertiesFile() const
    { return _isBuildPropertiesFile; }

public:
    inline bool isEmbeddable() const
    { return _isEmbeddable; }

public:
    inline bool isExecutable() const
    { return _isExecutable; }
    inline bool isExecutableWithGUI() const
    { return _isExecutableWithGUI; }
    inline bool containsNativeCode() const
    { return _containsNativeCode; }

public:
    inline bool isApplication() const
    { return _isApplication; }

public:
    inline bool isBundle() const
    { return _isBundle; }

public:
    inline bool isLibrary() const
    { return _isLibrary; }
    inline bool isDynamicLibrary() const
    { return _isDynamicLibrary; }
    inline bool isStaticLibrary() const
    { return _isStaticLibrary; }

public:
    inline bool isFolder() const
    { return _isFolder; }
    inline bool isWrappedFolder() const
    { return _isWrappedFolder; }

public:
    inline bool isFrameworkWrapper() const
    { return _isFrameworkWrapper; }
    inline bool isStaticFrameworkWrapper() const
    { return _isStaticFrameworkWrapper; }

public:
    inline bool isProjectWrapper() const
    { return _isProjectWrapper; }

public:
    inline bool isTargetWrapper() const
    { return _isTargetWrapper; }

public:
    inline libutil::string_vector const &extraPropertyNames() const
    { return _extraPropertyNames; }
    inline ComponentPart::map const &componentParts() const
    { return _componentParts; }

public:
    inline std::vector<BuildPhaseInjection> const &buildPhaseInjectionsWhenEmbedding() const
    { return _buildPhaseInjectionsWhenEmbedding; }

public:
    inline bool isSourceCode() const
    { return _isSourceCode; }
    inline bool isPreprocessed() const
    { return _isPreprocessed; }
    inline bool isScannedForIncludes() const
    { return _isScannedForIncludes; }
    inline bool includeInIndex() const
    { return _includeInIndex; }
    inline bool canSetIncludeInIndex() const
    { return _canSetIncludeInIndex; }

public:
    inline bool isDocumentation() const
    { return _isDocumentation; }

public:
    inline bool requiresHardTabs() const
    { return _requiresHardTabs; }

public:
    inline std::string const &permissions() const
    { return _permissions; }

public:
    inline libutil::string_vector const &prefix() const
    { return _prefix; }

public:
    inline bool appliesToBuildRules() const
    { return _appliesToBuildRules; }
    inline bool changesCauseDependencyGraphInvalidation() const
    { return _changesCauseDependencyGraphInvalidation; }
    inline std::string const &fallbackAutoroutingBuildPhase() const
    { return _fallbackAutoroutingBuildPhase; }

public:
    inline bool codeSignOnCopy() const
    { return _codeSignOnCopy; }
    inline bool removeHeadersOnCopy() const
    { return _removeHeadersOnCopy; }
    inline bool validateOnCopy() const
    { return _validateOnCopy; }

protected:
    friend class Specification;
    bool parse(Context *context, plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check) override;

protected:
    bool inherit(Specification::shared_ptr const &base) override;
    virtual bool inherit(FileType::shared_ptr const &base);

protected:
    static FileType::shared_ptr Parse(Context *context, plist::Dictionary const *dict);

public:
    static inline char const *Type()
    { return Types::FileType; }
};

} }

#endif  // !__pbxspec_PBX_FileType_h
