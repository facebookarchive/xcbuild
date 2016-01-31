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
        return nullptr;
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

static void
LoadWorkspaceProjects(std::vector<pbxproj::PBX::Project::shared_ptr> *projects, xcworkspace::XC::Workspace::shared_ptr const &workspace)
{
    /*
     * Load all the projects in the workspace.
     */
    IterateWorkspaceFiles(workspace, [&](xcworkspace::XC::FileRef::shared_ptr const &ref) {
        std::string path = ref->resolve(workspace);

        pbxproj::PBX::Project::shared_ptr project = pbxproj::PBX::Project::Open(path);
        if (project != nullptr) {
            projects->push_back(project);
        }
    });
}

static void
LoadNestedProjects(std::vector<pbxproj::PBX::Project::shared_ptr> *projects, pbxsetting::Environment const &baseEnvironment, std::vector<pbxproj::PBX::Project::shared_ptr> const &rootProjects)
{
    std::vector<pbxproj::PBX::Project::shared_ptr> nestedProjects;

    /*
     * Load all nested projects recursively.
     */
    for (pbxproj::PBX::Project::shared_ptr const &project : rootProjects) {
        /*
         * Determine the settings environment to find the project paths. This may not be complete,
         * but it's unclear exactly what settings are available here. Notably, we don't yet know what
         * the configuration or what target to use, so just the project settings seems reasonable.
         */
        pbxsetting::Environment environment = baseEnvironment;
        environment.insertFront(project->settings(), false);

        /*
         * Find the nested projects.
         */
        for (pbxproj::PBX::Project::ProjectReference const &projectReference : project->projectReferences()) {
            pbxproj::PBX::FileReference::shared_ptr const &projectFileReference = projectReference.projectReference();
            std::string projectPath = environment.expand(projectFileReference->resolve());

            /*
             * Load the project.
             */
            pbxproj::PBX::Project::shared_ptr project = pbxproj::PBX::Project::Open(projectPath);
            if (project != nullptr) {
                nestedProjects.push_back(project);
            }
        }
    }

    /*
     * Append the nested projects. This has to be after the loop as `rootProjects` might alias `projects`.
     */
    projects->insert(projects->end(), nestedProjects.begin(), nestedProjects.end());

    if (!nestedProjects.empty()) {
        /*
         * Load nested projects of the nested projects.
         */
        LoadNestedProjects(projects, baseEnvironment, nestedProjects);
    }
}

static void
LoadProjectSchemes(std::vector<xcscheme::SchemeGroup::shared_ptr> *schemeGroups, std::vector<pbxproj::PBX::Project::shared_ptr> const &projects)
{
    /*
     * Load the schemes inside the projects.
     */
    for (pbxproj::PBX::Project::shared_ptr const &project : projects) {
        xcscheme::SchemeGroup::shared_ptr projectGroup = xcscheme::SchemeGroup::Open(project->basePath(), project->projectFile(), project->name());
        if (projectGroup != nullptr) {
            schemeGroups->push_back(projectGroup);
        }
    }
}

static std::unordered_map<std::string, pbxproj::PBX::Project::shared_ptr>
CreateProjectMap(std::vector<pbxproj::PBX::Project::shared_ptr> const &projects)
{
    std::unordered_map<std::string, pbxproj::PBX::Project::shared_ptr> projectsMap;

    for (pbxproj::PBX::Project::shared_ptr const &project : projects) {
        /* Normalize path so it can be found on lookup. */
        std::string normalizedPath = FSUtil::NormalizePath(project->projectFile());
        projectsMap.insert({ normalizedPath, project });
    }

    return projectsMap;
}

WorkspaceContext WorkspaceContext::
Workspace(pbxsetting::Environment const &baseEnvironment, xcworkspace::XC::Workspace::shared_ptr const &workspace)
{
    std::vector<pbxproj::PBX::Project::shared_ptr> projects;
    std::vector<xcscheme::SchemeGroup::shared_ptr> schemeGroups;

    /*
     * Add the schemes from the workspace itself.
     */
    xcscheme::SchemeGroup::shared_ptr workspaceGroup = xcscheme::SchemeGroup::Open(workspace->basePath(), workspace->projectFile(), workspace->name());
    if (workspaceGroup != nullptr) {
        schemeGroups.push_back(workspaceGroup);
    }

    /*
     * Load projects within the workspace.
     */
    LoadWorkspaceProjects(&projects, workspace);

    /*
     * Recursively load nested projects within those projects.
     */
    LoadNestedProjects(&projects, baseEnvironment, projects);

    /*
     * Load schemes for all projects, including nested projects.
     */
    LoadProjectSchemes(&schemeGroups, projects);

    /*
     * Determine the DerivedData path for the workspace.
     */
    std::string derivedDataName = workspace->name() + "-" + DerivedDataHash(workspace->projectFile());

    return WorkspaceContext(workspace->basePath(), derivedDataName, workspace, nullptr, schemeGroups, CreateProjectMap(projects));
}

WorkspaceContext WorkspaceContext::
Project(pbxsetting::Environment const &baseEnvironment, pbxproj::PBX::Project::shared_ptr const &project)
{
    std::vector<pbxproj::PBX::Project::shared_ptr> projects;
    std::vector<xcscheme::SchemeGroup::shared_ptr> schemeGroups;

    /*
     * The root is a project, so it should be in the projects list.
     */
    projects.push_back(project);

    /*
     * Recursively load nested projects within the project.
     */
    LoadNestedProjects(&projects, baseEnvironment, projects);

    /*
     * Load schemes for all projects, including the root and nested projects.
     */
    LoadProjectSchemes(&schemeGroups, projects);

    /*
     * Determine the DerivedData path for the root project.
     */
    std::string derivedDataName = project->name() + "-" + DerivedDataHash(project->projectFile());

    return WorkspaceContext(project->basePath(), derivedDataName, nullptr, project, schemeGroups, CreateProjectMap(projects));
}
