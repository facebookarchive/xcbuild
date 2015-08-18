// Copyright 2013-present Facebook. All Rights Reserved.

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
    std::string             _execPath;
    std::string             _execDescription;
    std::string             _progressDescription;
    std::string             _commandLine;
    std::string             _commandInvocationClass;
    plist::Object          *_ruleName;
    std::string             _ruleFormat;
    std::string             _additionalInputFiles;
    std::string             _builtinJambaseRuleName;
    libutil::string_vector  _fileTypes;
    libutil::string_vector  _inputFileTypes;
    libutil::string_vector  _architectures;
    libutil::string_vector  _outputs;
    libutil::string_vector  _deletedProperties;
    libutil::string_map     _environmentVariables;
    plist::Object          *_commandOutputParser;
    bool                    _isAbstract;
    bool                    _isArchitectureNeutral;
    bool                    _caresAboutInclusionDependencies;
    bool                    _synthesizeBuildRule;
    bool                    _shouldRerunOnError;
    PropertyOption::vector  _options;

protected:
    Tool(bool isDefault);
    Tool(bool isDefault, std::string const &isa);

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
    inline std::string const &execDescription() const
    { return _execDescription; }
    inline std::string const &progressDescription() const
    { return _progressDescription; }

public:
    inline std::string const &commandLine() const
    { return _commandLine; }

public:
    inline std::string const &commandInvocationClass() const
    { return _commandInvocationClass; }

public:
    inline plist::Object const *ruleName() const
    { return _ruleName; }
    inline std::string const &ruleFormat() const
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
    inline PropertyOption::vector const &options() const
    { return _options; }

public:
    inline libutil::string_vector const &deletedProperties() const
    { return _deletedProperties; }

protected:
    friend class Specification;
    bool parse(std::shared_ptr<Manager> manager, plist::Dictionary const *dict) override
    { return parse(manager, dict, true); }

protected:
    bool parse(std::shared_ptr<Manager> manager, plist::Dictionary const *dict, bool check);

protected:
    bool inherit(Specification::shared_ptr const &base) override;
    virtual bool inherit(Tool::shared_ptr const &base);

protected:
    static Tool::shared_ptr Parse(std::shared_ptr<Manager> manager, plist::Dictionary const *dict);

public:
    static inline char const *Isa()
    { return ISA::PBXTool; }
    static inline char const *Type()
    { return Types::Tool; }
};

} }

#endif  // !__pbxspec_PBX_Tool_h
