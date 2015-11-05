/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/DependencyResolver.h>
#include <pbxbuild/TargetEnvironment.h>

using pbxbuild::DependencyResolver;
using pbxbuild::BuildContext;
using pbxbuild::BuildGraph;
using pbxbuild::BuildEnvironment;
using pbxbuild::TargetEnvironment;
using xcscheme::XC::Scheme;
using xcscheme::XC::BuildAction;
using xcscheme::XC::BuildActionEntry;
using libutil::FSUtil;

DependencyResolver::
DependencyResolver(BuildEnvironment const &buildEnvironment) :
    _buildEnvironment(buildEnvironment)
{
}

DependencyResolver::
~DependencyResolver()
{
}

static pbxproj::PBX::Target::shared_ptr
ResolveContainerItemProxy(BuildEnvironment const &buildEnvironment, BuildContext const &context, pbxproj::PBX::Target::shared_ptr const &target, pbxproj::PBX::ContainerItemProxy::shared_ptr const &proxy, bool productReference)
{
    pbxproj::PBX::FileReference::shared_ptr fileReference = proxy->containerPortal();
    if (fileReference == nullptr) {
        fprintf(stderr, "warning: not able to find file reference for proxy\n");
        return nullptr;
    }

    std::unique_ptr<TargetEnvironment> targetEnvironment = context.targetEnvironment(buildEnvironment, target);
    if (targetEnvironment == nullptr) {
        fprintf(stderr, "warning: not able to get target environment for target %s\n", target->name().c_str());
        return nullptr;
    }

    std::string path = targetEnvironment->environment().expand(fileReference->resolve());

    pbxproj::PBX::Project::shared_ptr project = context.workspaceContext()->project(path);
    if (productReference) {
        auto result = context.resolveProductIdentifier(project, proxy->remoteGlobalIDString());
        if (result == nullptr) {
            fprintf(stderr, "warning: not able to resolve product identifier %s in project %s\n", proxy->remoteGlobalIDString().c_str(), project ? project->name().c_str() : path.c_str());
            return nullptr;
        }

        return result->first;
    } else {
        return context.resolveTargetIdentifier(project, proxy->remoteGlobalIDString());
    }
}

struct DependenciesContext {
    BuildEnvironment buildEnvironment;
    BuildContext context;
    BuildGraph<pbxproj::PBX::Target::shared_ptr> *graph;
    BuildAction::shared_ptr buildAction;
    std::vector<pbxproj::PBX::Target::shared_ptr> *positional;
};

static void
AddDependencies(DependenciesContext const &context, pbxproj::PBX::Target::shared_ptr const &target);

static void
AddImplicitDependencies(DependenciesContext const &context, pbxproj::PBX::Target::shared_ptr const &target)
{
    std::vector<pbxproj::PBX::Target::shared_ptr> dependencies;

    for (pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase : target->buildPhases()) {
        // TODO(grp): Only include appropriate build phases for this action.
        // TODO(grp): This may be incomplete: is it possible to include proxied files in other phases?
        if (buildPhase->type() != pbxproj::PBX::BuildPhase::kTypeFrameworks && buildPhase->type() != pbxproj::PBX::BuildPhase::kTypeCopyFiles) {
            continue;
        }

        for (pbxproj::PBX::BuildFile::shared_ptr const &file : buildPhase->files()) {
            if (file->fileRef()->type() != pbxproj::PBX::GroupItem::kTypeReferenceProxy) {
                continue;
            }

            pbxproj::PBX::ReferenceProxy::shared_ptr proxy = std::static_pointer_cast <pbxproj::PBX::ReferenceProxy> (file->fileRef());

            pbxproj::PBX::Target::shared_ptr proxiedTarget = ResolveContainerItemProxy(context.buildEnvironment, context.context, target, proxy->remoteRef(), true);
            if (proxiedTarget != nullptr) {
#if 0
                printf("implicit dependency: %s -> %s\n", target->name().c_str(), proxiedTarget->name().c_str());
#endif
                dependencies.push_back(proxiedTarget);
                AddDependencies(context, proxiedTarget);
            } else {
                fprintf(stderr, "warning: was not able to load target for implicit dependency %s (from %s)\n", proxy->remoteRef()->remoteInfo().c_str(), proxy->name().c_str());
            }
        }
    }

    if (context.buildAction == nullptr || context.buildAction->parallelizeBuildables()) {
        context.graph->insert(target, dependencies);
    }
}

static void
AddExplicitDependencies(DependenciesContext const &context, pbxproj::PBX::Target::shared_ptr const &target)
{
    std::vector<pbxproj::PBX::Target::shared_ptr> dependencies;

    for (pbxproj::PBX::TargetDependency::shared_ptr const &dependency : target->dependencies()) {
        if (dependency->target() != nullptr) {
#if 0
            printf("explicit dependency: %s -> %s\n", target->name().c_str(), dependency->target()->name().c_str());
#endif
            dependencies.push_back(dependency->target());
            AddDependencies(context, dependency->target());
        } else if (dependency->targetProxy() != nullptr) {
            pbxproj::PBX::Target::shared_ptr proxiedTarget = ResolveContainerItemProxy(context.buildEnvironment, context.context, target, dependency->targetProxy(), false);
            if (proxiedTarget != nullptr) {
#if 0
                printf("explicit dependency: %s -> %s\n", target->name().c_str(), proxiedTarget->name().c_str());
#endif
                dependencies.push_back(proxiedTarget);
                AddDependencies(context, proxiedTarget);
            } else {
                fprintf(stderr, "warning: was not able to load target for explicit dependency %s\n", dependency->targetProxy()->remoteInfo().c_str());
            }
        }
    }

    if (context.buildAction == nullptr || context.buildAction->parallelizeBuildables()) {
        context.graph->insert(target, dependencies);
    }
}

static void
AddDependencies(DependenciesContext const &context, pbxproj::PBX::Target::shared_ptr const &target)
{
    if (context.buildAction != nullptr && context.buildAction->buildImplicitDependencies()) {
        AddImplicitDependencies(context, target);
    }

    AddExplicitDependencies(context, target);

    if (context.buildAction != nullptr && !context.buildAction->parallelizeBuildables()) {
        context.graph->insert(target, *context.positional);
        context.positional->push_back(target);
    }
}

BuildGraph<pbxproj::PBX::Target::shared_ptr> DependencyResolver::
resolveSchemeDependencies(BuildContext const &context) const
{
    BuildGraph<pbxproj::PBX::Target::shared_ptr> graph;

    xcscheme::XC::Scheme::shared_ptr const &scheme = context.scheme();
    if (scheme == nullptr) {
        fprintf(stderr, "error: scheme not available\n");
        return graph;
    }

    BuildAction::shared_ptr buildAction = scheme->buildAction();
    if (buildAction == nullptr) {
        fprintf(stderr, "error: build action not available\n");
        return graph;
    }

    std::vector<pbxproj::PBX::Target::shared_ptr> positional;
    for (BuildActionEntry::shared_ptr const &entry : buildAction->buildActionEntries()) {
        // TODO(grp): Check the buildFor* flags against the BuildContext.
        if (!entry->buildForRunning()) {
            continue;
        }

        xcscheme::XC::BuildableReference::shared_ptr const &reference = entry->buildableReference();
        if (reference == nullptr) {
            fprintf(stderr, "warning: couldn't find buildable reference in scheme\n");
            continue;
        }

        std::string projectPath = reference->resolve(context.workspaceContext()->basePath());
        pbxproj::PBX::Project::shared_ptr project = context.workspaceContext()->project(projectPath);
        if (project == nullptr) {
            fprintf(stderr, "warning: couldn't find project in workspace for build action entry\n");
            continue;
        }

        pbxproj::PBX::Target::shared_ptr target = context.resolveTargetIdentifier(project, reference->blueprintIdentifier());
        if (target == nullptr) {
            fprintf(stderr, "warning: couldn't find buildable reference for build action entry\n");
            continue;
        }

        DependenciesContext dependenciesContext = {
            .buildEnvironment = _buildEnvironment,
            .context = context,
            .graph = &graph,
            .buildAction = buildAction,
            .positional = &positional,
        };
        AddDependencies(dependenciesContext, target);
    }

    return graph;
}

BuildGraph<pbxproj::PBX::Target::shared_ptr> DependencyResolver::
resolveLegacyDependencies(BuildContext const &context, bool allTargets, std::string const &targetName) const
{
    BuildGraph<pbxproj::PBX::Target::shared_ptr> graph;

    pbxproj::PBX::Project::shared_ptr const &project = context.workspaceContext()->project();
    if (project == nullptr) {
        fprintf(stderr, "error: cannot resolve legacy dependencies for workspace\n");
        return graph;
    }

    std::vector<pbxproj::PBX::Target::shared_ptr> positional;
    for (pbxproj::PBX::Target::shared_ptr const &target : project->targets()) {
        if (!allTargets) {
            if (!targetName.empty() && target->name() != targetName) {
                continue;
            } else if (target != project->targets().front()) {
                continue;
            }
        }

        DependenciesContext dependenciesContext = {
            .buildEnvironment = _buildEnvironment,
            .context = context,
            .graph = &graph,
            .buildAction = nullptr,
            .positional = &positional,
        };
        AddDependencies(dependenciesContext, target);
    }

    return graph;
}

