#ifndef __pbxproj_PBX_Project_h
#define __pbxproj_PBX_Project_h

#include <xcscheme/XC/Scheme.h>
#include <xcscheme/XC/BuildableReference.h>
#include <pbxproj/PBX/Object.h>
#include <pbxproj/PBX/Group.h>
#include <pbxproj/PBX/Target.h>
#include <pbxproj/XC/ConfigurationList.h>

namespace pbxproj { namespace PBX {

class Project : public Object {
public:
    typedef std::shared_ptr <Project> shared_ptr;
    typedef std::vector <shared_ptr> vector;

private:
    std::string                        _projectFile;
    std::string                        _basePath;
    std::string                        _name;
    Object::map                        _blueprints;
    xcscheme::XC::Scheme::vector       _schemes;
    xcscheme::XC::Scheme::shared_ptr   _defaultScheme;

private:
    XC::ConfigurationList::shared_ptr  _buildConfigurationList;
    std::string                        _compatibilityVersion;
    std::string                        _developmentRegion;
    bool                               _hasScannedForEncodings;
    string_vector                      _knownRegions;
    Group::shared_ptr                  _mainGroup;
    std::string                        _projectDirPath;
    std::string                        _projectRoot;
    Target::vector                     _targets;
    FileReference::vector              _fileReferences;

public:
    Project();

public:
    static shared_ptr Open(std::string const &path);

public:
    inline XC::ConfigurationList::shared_ptr const &buildConfigurationList() const
    { return _buildConfigurationList; }
    inline XC::ConfigurationList::shared_ptr &buildConfigurationList()
    { return _buildConfigurationList; }

public:
    inline std::string const &compatibilityVersion() const
    { return _compatibilityVersion; }

public:
    inline std::string const &developmentRegion() const
    { return _developmentRegion; }

public:
    inline bool hasScannedForEncodings() const
    { return _hasScannedForEncodings; }

public:
    inline string_vector const &knownRegions() const
    { return _knownRegions; }
    inline string_vector &knownRegions()
    { return _knownRegions; }

public:
    inline Group::shared_ptr const &mainGroup() const
    { return _mainGroup; }
    inline Group::shared_ptr &mainGroup()
    { return _mainGroup; }

public:
    inline std::string const &projectDirPath() const
    { return _projectDirPath; }
    inline std::string const &projectRoot() const
    { return _projectRoot; }

public:
    inline Target::vector const &targets() const
    { return _targets; }
    inline Target::vector &targets()
    { return _targets; }

public:
    inline std::string const &name() const
    { return _name; }
    inline std::string const &projectFile() const
    { return _projectFile; }
    inline std::string const &basePath() const
    { return _basePath; }

protected:
    friend class pbxproj::Context;
    inline void cacheObject(Object::shared_ptr const &object)
    { _blueprints[object->blueprintIdentifier()] = object; }

public:
    inline Object::map const &blueprints() const
    { return _blueprints; }
    inline Object::map &blueprints()
    { return _blueprints; }

public:
    inline xcscheme::XC::Scheme::vector const &schemes() const
    { return _schemes; }
    inline xcscheme::XC::Scheme::vector &schemes()
    { return _schemes; }

public:
    inline xcscheme::XC::Scheme::shared_ptr const &defaultScheme() const
    { return _defaultScheme; }
    inline xcscheme::XC::Scheme::shared_ptr &defaultScheme()
    { return _defaultScheme; }

public:
    inline FileReference::vector const &fileReferences() const
    { return _fileReferences; }
    inline FileReference::vector &fileReferences()
    { return _fileReferences; }

public:
    inline Object::shared_ptr resolveBuildableReference(xcscheme::XC::BuildableReference::shared_ptr const &BR) const
    {
        if (!BR || BR->blueprintIdentifier().empty())
            return Object::shared_ptr();

        auto I = _blueprints.find(BR->blueprintIdentifier());
        if (I == _blueprints.end())
            return Object::shared_ptr();
        else
            return I->second;
    }

public:
    pbxsetting::Level settings(void) const;

public:
    bool parse(Context &context, plist::Dictionary const *dict);

public:
    static inline char const *Isa()
    { return ISA::PBXProject; }
};

} }

#endif  // !__pbxproj_PBX_Project_h
