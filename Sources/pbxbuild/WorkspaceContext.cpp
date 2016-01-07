/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/WorkspaceContext.h>
#include <libutil/FSUtil.h>
#include <libutil/md5.h>

using pbxbuild::WorkspaceContext;
using libutil::FSUtil;

WorkspaceContext::
WorkspaceContext(
    std::string const &basePath,
    std::string const &derivedDataName,
    xcworkspace::XC::Workspace::shared_ptr const &workspace,
    pbxproj::PBX::Project::shared_ptr const &project,
    std::vector<xcscheme::SchemeGroup::shared_ptr> const &schemeGroups,
    std::unordered_map<std::string, pbxproj::PBX::Project::shared_ptr> const &projects) :
    _basePath       (basePath),
    _derivedDataName(derivedDataName),
    _workspace      (workspace),
    _project        (project),
    _schemeGroups   (schemeGroups),
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
    /* Normalize the path in case it has relative components. */
    std::string resolvedProjectPath = FSUtil::NormalizePath(projectPath);

    auto PI = _projects.find(resolvedProjectPath);
    if (PI != _projects.end()) {
        return PI->second;
    } else {
        // TODO(grp): Limit this to just subprojects of existing projects.
        pbxproj::PBX::Project::shared_ptr project = pbxproj::PBX::Project::Open(resolvedProjectPath);
        if (project != nullptr) {
            _projects.insert({ resolvedProjectPath, project });
        } else {
            fprintf(stderr, "warning: unable to load project at %s\n", projectPath.c_str());
        }
        return project;
    }
}

xcscheme::XC::Scheme::shared_ptr WorkspaceContext::
scheme(std::string const &name) const
{
    for (xcscheme::SchemeGroup::shared_ptr const &schemeGroup : _schemeGroups) {
        if (xcscheme::XC::Scheme::shared_ptr const &scheme = schemeGroup->scheme(name)) {
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
    /*
     * This algorithm is documented here:
     * https://samdmarshall.com/blog/xcode_deriveddata_hashes.html
     */

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
            xcworkspace::XC::Group::shared_ptr group = std::static_pointer_cast<xcworkspace::XC::Group>(item);
            for (xcworkspace::XC::GroupItem::shared_ptr const &child : group->items()) {
                IterateWorkspaceItem(child, cb);
            }
            break;
        }
        case xcworkspace::XC::GroupItem::kTypeFileRef: {
            xcworkspace::XC::FileRef::shared_ptr fileRef = std::static_pointer_cast<xcworkspace::XC::FileRef>(item);
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
    std::vector<xcscheme::SchemeGroup::shared_ptr> schemeGroups;

    /*
     * Add the schemes from the workspace itself.
     */
    xcscheme::SchemeGroup::shared_ptr workspaceGroup = xcscheme::SchemeGroup::Open(workspace->basePath(), workspace->projectFile(), workspace->name());
    if (workspaceGroup != nullptr) {
        schemeGroups.push_back(workspaceGroup);
    }

    /*
     * Load all the projects in the workspace (and schemes inside those projects).
     */
    IterateWorkspaceFiles(workspace, [&](xcworkspace::XC::FileRef::shared_ptr const &ref) {
        std::string path = ref->resolve(workspace);
        pbxproj::PBX::Project::shared_ptr project = pbxproj::PBX::Project::Open(path);
        if (project != nullptr) {
            projects.insert({ project->projectFile(), project });

            /*
             * Load the schemes inside this project.
             */
            xcscheme::SchemeGroup::shared_ptr projectGroup = xcscheme::SchemeGroup::Open(project->basePath(), project->projectFile(), project->name());
            if (projectGroup != nullptr) {
                schemeGroups.push_back(projectGroup);
            }
        }
    });

    // TODO(grp): Load nested projects here.

    std::string derivedDataName = workspace->name() + "-" + DerivedDataHash(workspace->projectFile());

    return WorkspaceContext(workspace->basePath(), derivedDataName, workspace, nullptr, schemeGroups, projects);
}

WorkspaceContext WorkspaceContext::
Project(pbxproj::PBX::Project::shared_ptr const &project)
{
    /*
     * Add the schemes from the project itself.
     */
    xcscheme::SchemeGroup::shared_ptr group = xcscheme::SchemeGroup::Open(project->basePath(), project->projectFile(), project->name());

    /*
     * The root is a project, so add it to the projects map so it can be found in project lookups later.
     */
    std::unordered_map<std::string, pbxproj::PBX::Project::shared_ptr> projects = {
        { project->projectFile(), project },
    };

    // TODO(grp): Load nested projects here.

    std::string derivedDataName = project->name() + "-" + DerivedDataHash(project->projectFile());

    return WorkspaceContext(project->basePath(), derivedDataName, nullptr, project, { group }, projects);
}
