/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxspec_PBX_Tool_h
#define __pbxspec_PBX_Tool_h

#include <pbxspec/PBX/Specification.h>
#include <pbxspec/PBX/PropertyOption.h>

namespace pbxspec { namespace PBX {

class Tool : public Specification {
public:
    typedef std::shared_ptr <Tool> shared_ptr;
    typedef std::vector <shared_ptr> vector;

protected:
    std::string                     _execPath;
    std::string                     _execDescription;
    std::string                     _execDescriptionForPrecompile;
    std::string                     _execDescriptionForCompile;
    std::string                     _execDescriptionForCreateBitcode;
    std::string                     _progressDescription;
    std::string                     _progressDescriptionForPrecompile;
    std::string                     _progressDescriptionForCompile;
    std::string                     _progressDescriptionForCreateBitcode;
    std::string                     _commandLine;
    std::string                     _commandInvocationClass;
    std::string                     _commandIdentifier;
    pbxsetting::Value               _ruleName;
    pbxsetting::Value               _ruleFormat;
    std::string                     _additionalInputFiles;
    std::string                     _builtinJambaseRuleName;
    libutil::string_vector          _fileTypes;
    libutil::string_vector          _inputFileTypes;
    libutil::string_vector          _architectures;
    libutil::string_vector          _outputs;
    std::unordered_set<std::string> _deletedProperties;
    libutil::string_map             _environmentVariables;
    std::vector<int>                _successExitCodes;
    plist::Object                  *_commandOutputParser;
    bool                            _isAbstract;
    bool                            _isArchitectureNeutral;
    bool                            _caresAboutInclusionDependencies;
    bool                            _synthesizeBuildRule;
    bool                            _shouldRerunOnError;
    bool                            _deeplyStatInputDirectories;
    bool                            _isUnsafeToInterrupt;
    int                             _messageLimit;
    PropertyOption::vector          _options;
    PropertyOption::used_map        _optionsUsed;

protected:
    Tool();

public:
    virtual ~Tool();

public:
    inline char const *type() const override
    { return Tool::Type(); }

public:
    inline Tool::shared_ptr const &base() const
    { return reinterpret_cast <Tool::shared_ptr const &> (Specification::base()); }

public:
    inline std::string const &execPath() const
    { return _execPath; }

public:
    inline std::string const &execDescription() const
    { return _execDescription; }
    inline std::string const &execDescriptionForPrecompile() const
    { return _execDescriptionForPrecompile; }
    inline std::string const &execDescriptionForCompile() const
    { return _execDescriptionForCompile; }
    inline std::string const &execDescriptionForCreateBitcode() const
    { return _execDescriptionForCreateBitcode; }

public:
    inline std::string const &progressDescription() const
    { return _progressDescription; }
    inline std::string const &progressDescriptionForPrecompile() const
    { return _progressDescriptionForPrecompile; }
    inline std::string const &progressDescriptionForCompile() const
    { return _progressDescriptionForCompile; }
    inline std::string const &progressDescriptionForCreateBitcode() const
    { return _progressDescriptionForCreateBitcode; }

public:
    inline std::string const &commandLine() const
    { return _commandLine; }

public:
    inline std::string const &commandInvocationClass() const
    { return _commandInvocationClass; }
    inline std::string const &commandIdentifier() const
    { return _commandIdentifier; }

public:
    inline pbxsetting::Value const &ruleName() const
    { return _ruleName; }
    inline pbxsetting::Value const &ruleFormat() const
    { return _ruleFormat; }
    inline std::string const &builtinJambaseRuleName() const
    { return _builtinJambaseRuleName; }

public:
    inline std::string const &additionalInputFiles() const
    { return _additionalInputFiles; }

public:
    inline libutil::string_vector const &fileTypes() const
    { return _fileTypes; }
    inline libutil::string_vector const &inputFileTypes() const
    { return _inputFileTypes; }

public:
    inline libutil::string_vector const &architectures() const
    { return _architectures; }

public:
    inline libutil::string_vector const &outputs() const
    { return _outputs; }

public:
    inline libutil::string_map const &environmentVariables() const
    { return _environmentVariables; }

public:
    inline std::vector<int> const &successExitCodes() const
    { return _successExitCodes; }

public:
    inline plist::Object const *commandOutputParser() const
    { return _commandOutputParser; }

public:
    inline bool isAbstract() const
    { return _isAbstract; }

public:
    inline bool isArchitectureNeutral() const
    { return _isArchitectureNeutral; }

public:
    inline bool caresAboutInclusionDependencies() const
    { return _caresAboutInclusionDependencies; }

public:
    inline bool synthesizeBuildRule() const
    { return _synthesizeBuildRule; }

public:
    inline bool shouldRerunOnError() const
    { return _shouldRerunOnError; }

public:
    inline bool deeplyStatInputDirectories() const
    { return _deeplyStatInputDirectories; }

public:
    inline bool isUnsafeToInterrupt() const
    { return _isUnsafeToInterrupt; }

public:
    inline int messageLimit() const
    { return _messageLimit; }

public:
    inline PropertyOption::vector const &options() const
    { return _options; }

public:
    inline std::unordered_set<std::string> const &deletedProperties() const
    { return _deletedProperties; }

public:
    pbxsetting::Level defaultSettings(void) const;

protected:
    friend class Specification;
    bool parse(Context *context, plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check) override;

protected:
    bool inherit(Specification::shared_ptr const &base) override;
    virtual bool inherit(Tool::shared_ptr const &base);

protected:
    static Tool::shared_ptr Parse(Context *context, plist::Dictionary const *dict);

public:
    static inline char const *Type()
    { return Types::Tool; }
};

} }

#endif  // !__pbxspec_PBX_Tool_h
