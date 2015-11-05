/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/WorkspaceContext.h>
#include <libutil/md5.h>

using pbxbuild::WorkspaceContext;

WorkspaceContext::
WorkspaceContext(
    std::string const &basePath,
    std::string const &derivedDataName,
    xcworkspace::XC::Workspace::shared_ptr const &workspace,
    pbxproj::PBX::Project::shared_ptr const &project,
    std::vector<xcscheme::XC::Scheme::shared_ptr> const &schemes,
    std::unordered_map<std::string, pbxproj::PBX::Project::shared_ptr> const &projects
) :
    _basePath       (basePath),
    _derivedDataName(derivedDataName),
    _workspace      (workspace),
    _project        (project),
    _schemes        (schemes),
    _projects       (projects)
{
}

WorkspaceContext::
~WorkspaceContext()
{
}

pbxproj::PBX::Project::shared_ptr WorkspaceContext::
project(std::string const &projectPath) const
{
    auto PI = _projects.find(projectPath);
    if (PI != _projects.end()) {
        return PI->second;
    } else {
        // TODO(grp): Limit this to just subprojects of existing projects.
        pbxproj::PBX::Project::shared_ptr project = pbxproj::PBX::Project::Open(projectPath);
        if (project != nullptr) {
            _projects.insert({ projectPath, project });
        } else {
            fprintf(stderr, "warning: unable to load project at %s\n", projectPath.c_str());
        }
        return project;
    }
}

xcscheme::XC::Scheme::shared_ptr WorkspaceContext::
scheme(std::string const &name) const
{
    for (xcscheme::XC::Scheme::shared_ptr const &scheme : _schemes) {
        if (scheme->name() == name) {
            return scheme;
        }
    }

    return nullptr;
}

static uint64_t
hton64(uint64_t v)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    v = ((v & 0x00000000000000FFULL) << 56) |
        ((v & 0x000000000000FF00ULL) << 40) |
        ((v & 0x0000000000FF0000ULL) << 24) |
        ((v & 0x00000000FF000000ULL) <<  8) |
        ((v & 0x000000FF00000000ULL) >>  8) |
        ((v & 0x0000FF0000000000ULL) >> 24) |
        ((v & 0x00FF000000000000ULL) >> 40) |
        ((v & 0xFF00000000000000ULL) >> 56);
#endif
    return v;
}

static std::string
DerivedDataHash(std::string const &path)
{
    md5_state_t state;
    md5_init(&state);
    md5_append(&state, reinterpret_cast<const md5_byte_t *>(path.data()), path.size());

    uint8_t digest[16];
    md5_finish(&state, reinterpret_cast<md5_byte_t *>(&digest));

    char hash_path[28];
    int counter;

    uint64_t first_value = hton64(*reinterpret_cast<uint64_t *>(&digest[0]));
    counter = 13;
    while (counter >= 0) {
        hash_path[counter] = 'a' + (first_value % 26);
        first_value /= 26;
        counter--;
    }

    uint64_t second_value = hton64(*reinterpret_cast<uint64_t *>(&digest[8]));
    counter = 27;
    while (counter > 13) {
        hash_path[counter] = 'a' + (second_value % 26);
        second_value /= 26;
        counter--;
    }

    return std::string(hash_path, 28);
}

static void
IterateWorkspaceItem(xcworkspace::XC::GroupItem::shared_ptr const &item, std::function<void(xcworkspace::XC::FileRef::shared_ptr const &)> const &cb)
{
    switch (item->type()) {
        case xcworkspace::XC::GroupItem::kTypeGroup: {
            xcworkspace::XC::Group::shared_ptr group = std::static_pointer_cast <xcworkspace::XC::Group> (item);
            for (xcworkspace::XC::GroupItem::shared_ptr const &child : group->items()) {
                IterateWorkspaceItem(child, cb);
            }
            break;
        }
        case xcworkspace::XC::GroupItem::kTypeFileRef: {
            xcworkspace::XC::FileRef::shared_ptr fileRef = std::static_pointer_cast <xcworkspace::XC::FileRef> (item);
            cb(fileRef);
            break;
        }
    }
}

static void
IterateWorkspaceFiles(xcworkspace::XC::Workspace::shared_ptr const &workspace, std::function<void(xcworkspace::XC::FileRef::shared_ptr const &)> const &cb)
{
    for (xcworkspace::XC::GroupItem::shared_ptr const &item : workspace->items()) {
        IterateWorkspaceItem(item, cb);
    }
}

WorkspaceContext WorkspaceContext::
Workspace(xcworkspace::XC::Workspace::shared_ptr const &workspace)
{
    std::unordered_map<std::string, pbxproj::PBX::Project::shared_ptr> projects;
    xcscheme::XC::Scheme::vector schemes;

    xcscheme::SchemeGroup::shared_ptr workspaceGroup = xcscheme::SchemeGroup::Open(workspace->projectFile(), workspace->name());
    if (workspaceGroup) {
        schemes.insert(schemes.end(), workspaceGroup->schemes().begin(), workspaceGroup->schemes().end());
    }

    IterateWorkspaceFiles(workspace, [&](xcworkspace::XC::FileRef::shared_ptr const &ref) {
        std::string path = ref->resolve(workspace);
        pbxproj::PBX::Project::shared_ptr project = pbxproj::PBX::Project::Open(path);
        if (project != nullptr) {
            projects.insert({ project->projectFile(), project });

            xcscheme::SchemeGroup::shared_ptr projectGroup = xcscheme::SchemeGroup::Open(project->projectFile(), project->name());
            if (projectGroup != nullptr) {
                schemes.insert(schemes.end(), projectGroup->schemes().begin(), projectGroup->schemes().end());
            }
        }
    });

    std::string derivedDataName = workspace->name() + "-" + DerivedDataHash(workspace->projectFile());

    return WorkspaceContext(workspace->basePath(), derivedDataName, workspace, nullptr, schemes, projects);
}

WorkspaceContext WorkspaceContext::
Project(pbxproj::PBX::Project::shared_ptr const &project)
{
    xcscheme::XC::Scheme::vector schemes;

    xcscheme::SchemeGroup::shared_ptr group = xcscheme::SchemeGroup::Open(project->projectFile(), project->name());
    if (group) {
        schemes.insert(schemes.end(), group->schemes().begin(), group->schemes().end());
    }

    std::unordered_map<std::string, pbxproj::PBX::Project::shared_ptr> projects = {
        { project->projectFile(), project },
    };

    std::string derivedDataName = project->name() + "-" + DerivedDataHash(project->projectFile());

    return WorkspaceContext(project->basePath(), derivedDataName, nullptr, project, schemes, projects);
}
