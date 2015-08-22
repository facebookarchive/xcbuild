// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_Context_h
#define __pbxproj_Context_h

#include <pbxproj/PlistHelpers.h>
#include <pbxproj/ISA.h>

namespace pbxproj {

//
// Forward declarations
//
namespace PBX {

class Object;
class Project;
class Group;
class VariantGroup;
class FileReference;
class ReferenceProxy;
class NativeTarget;
class LegacyTarget;
class ContainerItemProxy;
class TargetDependency;
class BuildFile;
class BuildRule;
class HeadersBuildPhase;
class SourcesBuildPhase;
class ResourcesBuildPhase;
class FrameworksBuildPhase;
class CopyFilesBuildPhase;
class ShellScriptBuildPhase;

}

namespace XC {

class BuildConfiguration;
class ConfigurationList;
class VersionGroup;

}

struct Context {
public:
    //
    // Parsing context
    //
    plist::Dictionary const *objects;

    //
    // The main project
    //
    PBX::Project       *project;

    //
    // Cached values
    //
    std::map <std::string, std::shared_ptr <PBX::Project>>               projects;
    std::map <std::string, std::shared_ptr <PBX::FileReference>>         fileReferences;
    std::map <std::string, std::shared_ptr <PBX::ReferenceProxy>>        referenceProxies;
    std::map <std::string, std::shared_ptr <PBX::Group>>                 groups;
    std::map <std::string, std::shared_ptr <PBX::VariantGroup>>          variantGroups;
    std::map <std::string, std::shared_ptr <PBX::NativeTarget>>          nativeTargets;
    std::map <std::string, std::shared_ptr <PBX::LegacyTarget>>          legacyTargets;
    std::map <std::string, std::shared_ptr <PBX::TargetDependency>>      targetDependencies;
    std::map <std::string, std::shared_ptr <PBX::ContainerItemProxy>>    containerItemProxies;
    std::map <std::string, std::shared_ptr <PBX::BuildFile>>             buildFiles;
    std::map <std::string, std::shared_ptr <PBX::BuildRule>>             buildRules;
    std::map <std::string, std::shared_ptr <PBX::HeadersBuildPhase>>     headersBuildPhases;
    std::map <std::string, std::shared_ptr <PBX::SourcesBuildPhase>>     sourcesBuildPhases;
    std::map <std::string, std::shared_ptr <PBX::ResourcesBuildPhase>>   resourcesBuildPhases;
    std::map <std::string, std::shared_ptr <PBX::FrameworksBuildPhase>>  frameworksBuildPhases;
    std::map <std::string, std::shared_ptr <PBX::CopyFilesBuildPhase>>   copyFilesBuildPhases;
    std::map <std::string, std::shared_ptr <PBX::ShellScriptBuildPhase>> shellScriptBuildPhases;

    std::map <std::string, std::shared_ptr <XC::BuildConfiguration>>     buildConfigurations;
    std::map <std::string, std::shared_ptr <XC::ConfigurationList>>      configurationLists;
    std::map <std::string, std::shared_ptr <XC::VersionGroup>>           versionGroups;

public:
    Context()
    {
        project = nullptr;
    }

    inline void clear()
    {
        project = nullptr;
        projects.clear();
        fileReferences.clear();
        referenceProxies.clear();
        groups.clear();
        variantGroups.clear();
        nativeTargets.clear();
        targetDependencies.clear();
        containerItemProxies.clear();
        buildFiles.clear();
        buildRules.clear();
        headersBuildPhases.clear();
        sourcesBuildPhases.clear();
        resourcesBuildPhases.clear();
        frameworksBuildPhases.clear();
        copyFilesBuildPhases.clear();
        shellScriptBuildPhases.clear();

        buildConfigurations.clear();
        configurationLists.clear();
        versionGroups.clear();
    }

    //
    // Helper functions
    //
private:
    inline plist::Dictionary const *get(std::string const &key,
                                        std::string const &isa,
                                        std::string *id = nullptr) const
    {
        if (id != nullptr) {
            *id = key;
        }
        return PlistDictionaryGetPBXObject(objects, key, isa);
    }

public:
    template <typename T>
    inline plist::Dictionary const *get(std::string const &key,
                                       std::string *id = nullptr) const
    { return get(key, T::Isa(), id); }

private:
    inline plist::Dictionary const *get(plist::Object const *objectKey,
                                        std::string const &isa,
                                        std::string *id = nullptr) const
    {
        plist::String const *key = plist::CastTo <plist::String> (objectKey);
        if (key == nullptr)
            return nullptr;

        if (id != nullptr) {
            *id = key->value();
        }
        return PlistDictionaryGetPBXObject(objects, key->value(), isa);
    }

public:
    template <typename T>
    inline plist::Dictionary const *get(plist::Object const *objectKey,
                                        std::string *id = nullptr) const
    { return get(objectKey, T::Isa(), id); }

private:
    inline plist::Dictionary const *indirect(plist::Dictionary const *dict,
                                             std::string const &key,
                                             std::string const &isa,
                                            std::string *id = nullptr) const
    {
        return PlistDictionaryGetIndirectPBXObject(objects, dict, key, isa, id);
    }

public:
    template <typename T>
    inline plist::Dictionary const *indirect(plist::Dictionary const *dict,
                                             std::string const &key,
                                             std::string *id = nullptr) const
    {
        return indirect(dict, key, T::Isa(), id);
    }

private:
    inline plist::Dictionary const *indirect(plist::Dictionary const *dict,
                                             plist::Object const *objectKey,
                                             std::string const &isa,
                                             std::string *id = nullptr) const
    {
        plist::String const *key = plist::CastTo <plist::String> (objectKey);
        if (key == nullptr)
            return nullptr;
        else
            return PlistDictionaryGetIndirectPBXObject(objects, dict,
                    key->value(), isa, id);
    }

public:
    template <typename T>
    inline plist::Dictionary const *indirect(plist::Dictionary const *dict,
                                             plist::Object const *objectKey,
                                             std::string *id = nullptr) const
    {
        return indirect(dict, objectKey, T::Isa(), id);
    }

public:
    template <typename T>
    inline std::shared_ptr <T> parseObject(std::map <std::string, std::shared_ptr <T>> &cache,
                                           std::string const &id,
                                           plist::Dictionary const *dict)
    {
        auto I = cache.find(id);
        if (I != cache.end())
            return I->second;

        auto O = std::make_shared <T> ();
        cacheObject(O, id); // cache inside the project
        cache[id] = O; // cache local to the context
        if (!O->parse(*this, dict)) {
            cache.erase(id);
            return std::shared_ptr <T> ();
        }

        return O;
    }

    template <typename T>
    inline std::shared_ptr <T> parseObject(std::map <std::string, std::shared_ptr <T>> &cache,
                                           plist::Object const *objectId,
                                           plist::Dictionary const *dict)
    {
        plist::String const *id = plist::CastTo <plist::String> (objectId);
        if (id == nullptr)
            return nullptr;

        return parseObject(cache, id->value(), dict);
    }

private:
    void cacheObject(std::shared_ptr <PBX::Object> const &O, std::string const &id);
};

}

#endif  // !__pbxproj_Context_h
