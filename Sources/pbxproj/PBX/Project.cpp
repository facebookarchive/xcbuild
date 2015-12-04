/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxproj/PBX/Project.h>
#include <pbxproj/PBX/AggregateTarget.h>
#include <pbxproj/PBX/LegacyTarget.h>
#include <pbxproj/PBX/NativeTarget.h>

using pbxproj::PBX::Project;
using pbxsetting::Level;
using pbxsetting::Setting;
using libutil::FSUtil;
using libutil::SysUtil;

Project::Project() :
    Object                 (Isa()),
    _hasScannedForEncodings(false)
{
}

Level Project::
settings(void) const
{
    std::vector<Setting> settings = {
        Setting::Parse("PROJECT", _name),
        Setting::Parse("PROJECT_NAME", _name),
        Setting::Parse("PROJECT_DIR", _basePath),
        Setting::Parse("PROJECT_FILE_PATH", _projectFile),
        Setting::Parse("SRCROOT", _basePath),
        Setting::Parse("DEVELOPMENT_LANGUAGE", _developmentRegion),
    };

    return Level(settings);
}

bool
Project::parse(Context &context, plist::Dictionary const *dict)
{
    std::string BCLID;
    std::string MGID;

    auto BCL  = context.indirect <XC::ConfigurationList> (dict, "buildConfigurationList", &BCLID);
    auto A    = dict->value <plist::Array> ("attributes");
    auto CV   = dict->value <plist::String> ("compatibilityVersion");
    auto DR   = dict->value <plist::String> ("developmentRegion");
    auto HSFE = dict->value <plist::String> ("hasScannedForEncodings");
    auto KR   = dict->value <plist::Array> ("knownRegions");
    auto MG   = context.indirect <Group> (dict, "mainGroup", &MGID);
    auto PDP  = dict->value <plist::String> ("projectDirPath");
    auto PR   = dict->value <plist::String> ("projectRoot");
    auto Ts   = dict->value <plist::Array> ("targets");

    if (BCL != nullptr) {
        _buildConfigurationList =
          context.parseObject(context.configurationLists, BCLID, BCL);
        if (!_buildConfigurationList) {
            abort();
            return false;
        }
    }

    if (CV != nullptr) {
        _compatibilityVersion = CV->value();
    }

    if (DR != nullptr) {
        _developmentRegion = DR->value();
    }

    if (HSFE != nullptr) {
        _hasScannedForEncodings = (pbxsetting::Type::ParseInteger(HSFE->value()) != 0);
    }

    if (KR != nullptr) {
        for (size_t n = 0; n < KR->count(); n++) {
            auto R = KR->value <plist::String> (n);
            if (R != nullptr) {
                _knownRegions.push_back(R->value());
            }
        }
    }

    if (MG != nullptr) {
        _mainGroup = context.parseObject(context.groups, MGID, MG);
        if (!_mainGroup) {
            abort();
            return false;
        }
    }

    if (PDP != nullptr) {
        _projectDirPath = PDP->value();
    }

    if (PR != nullptr) {
        _projectRoot = PR->value();
    }

    if (Ts != nullptr) {
        for (size_t n = 0; n < Ts->count(); n++) {
            std::string TID;
            if (auto Td = context.get <NativeTarget> (Ts->value(n), &TID)) {
                auto T = context.parseObject(context.nativeTargets, TID, Td);
                if (!T) {
                    abort();
                    return false;
                }

                _targets.push_back(T);
            } else if (auto Td = context.get <LegacyTarget> (Ts->value(n), &TID)) {
                auto T = context.parseObject(context.legacyTargets, TID, Td);
                if (!T) {
                    abort();
                    return false;
                }

                _targets.push_back(T);
            } else if (auto Td = context.get <AggregateTarget> (Ts->value(n), &TID)) {
                auto T = context.parseObject(context.aggregateTargets, TID, Td);
                if (!T) {
                    abort();
                    return false;
                }

                _targets.push_back(T);
            }
        }
    }

    return true;
}

Project::shared_ptr Project::
Open(std::string const &path)
{
    if (path.empty()) {
        errno = EINVAL;
        fprintf(stderr, "error: project path is empty\n");
        return nullptr;
    }

    std::string projectFileName = path + "/project.pbxproj";
    if (!FSUtil::TestForRead(projectFileName.c_str())) {
        fprintf(stderr, "error: project file %s is not readable\n", projectFileName.c_str());
        return nullptr;
    }

    std::string realPath = FSUtil::ResolvePath(projectFileName);
    if (realPath.empty()) {
        fprintf(stderr, "error: project file %s is not resolvable\n", projectFileName.c_str());
        return nullptr;
    }

    //
    // Parse property list
    //
    auto result = plist::Format::Any::Read(projectFileName);
    if (result.first == nullptr) {
        fprintf(stderr, "error: project file %s is not parseable: %s\n", projectFileName.c_str(), result.second.c_str());
        return nullptr;
    }

    plist::Dictionary *plist = plist::CastTo<plist::Dictionary>(result.first.get());
    if (plist == nullptr) {
        fprintf(stderr, "error: project file %s is not a dictionary\n", projectFileName.c_str());
        return nullptr;
    }

    //
    // Fetch basic objects
    //
    auto archiveVersion = plist->value("archiveVersion");
    if (archiveVersion == nullptr) {
        fprintf(stderr, "error: project file %s is not parseable (no archive version)\n", projectFileName.c_str());
        return nullptr;
    } else {
        auto archiveVersionInteger = plist::Integer::Coerce(archiveVersion);
        if (archiveVersionInteger == nullptr) {
            fprintf(stderr, "error: project file %s is not parseable (unknown archive version)\n", projectFileName.c_str());
            return nullptr;
        }

        if (archiveVersionInteger->value() > 1) {
            fprintf(stderr, "warning: archive version %u may be unsupported\n",
                    static_cast <unsigned> (archiveVersionInteger->value()));
        }
    }

    auto objectVersion = plist->value("objectVersion");
    if (objectVersion == nullptr) {
        fprintf(stderr, "error: project file %s is not parseable (no object version)\n", projectFileName.c_str());
        return nullptr;
    } else {
        auto objectVersionInteger = plist::Integer::Coerce(objectVersion);
        if (objectVersionInteger == nullptr) {
            fprintf(stderr, "error: project file %s is not parseable (unknown object version)\n", projectFileName.c_str());
            return nullptr;
        }

        if (objectVersionInteger->value() > 46) {
            fprintf(stderr, "warning: object version %u may be unsupported\n",
                    static_cast <unsigned> (objectVersionInteger->value()));
        }
    }

    auto objects = plist->value <plist::Dictionary> ("objects");
    if (objects == nullptr) {
        return nullptr;
    }

    errno = 0;

    //
    // Initialize context
    //
    Context context;
    context.objects = objects;

    //
    // Fetch the project dictionary (root object)
    //
    std::string PID;
    auto P = context.indirect <Project> (plist, "rootObject", &PID);
    if (P == nullptr) {
        fprintf(stderr, "error: unable to parse project\n");
        return nullptr;
    }

    //
    // Parse the project dictionary and create the project object.
    //
    auto project = context.parseObject(context.projects, PID, P);

    //
    // Save some useful info
    //
    project->_projectFile = FSUtil::GetDirectoryName(realPath);

    size_t slash = project->_projectFile.rfind('/');
    size_t dot   = project->_projectFile.rfind('.');

    project->_basePath    = project->_projectFile.substr(0, slash);
    project->_name        = project->_projectFile.substr(slash + 1, dot - slash - 1);

    //
    // Transfer all file references from cache.
    //
    for (auto I : context.fileReferences) {
        project->_fileReferences.push_back(I.second);
    }

    return project;
}
