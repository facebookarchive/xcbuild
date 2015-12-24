/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/DependencyResolver.h>
#include <pbxbuild/TargetEnvironment.h>

#define DEPENDENCY_RESOLVER_LOGGING 0

using pbxbuild::DependencyResolver;
using pbxbuild::BuildContext;
using pbxbuild::WorkspaceContext;
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
        fprintf(stderr, "warning: not able to get target environment for target %s %s\n", target->blueprintIdentifier().c_str(), target->name().c_str());
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
    std::unordered_set<pbxproj::PBX::Target::shared_ptr> *positional;
    std::unordered_map<std::string, pbxproj::PBX::Target::shared_ptr> *productPathToTarget;
};

static void
AddDependencies(DependenciesContext const &context, pbxproj::PBX::Target::shared_ptr const &target);

static void
AddImplicitDependencies(DependenciesContext const &context, pbxproj::PBX::Target::shared_ptr const &target)
{
    std::unordered_set<pbxproj::PBX::Target::shared_ptr> dependencies;

    for (pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase : target->buildPhases()) {
        // TODO(grp): Only include appropriate build phases for this action.
        // TODO(grp): This may be incomplete: is it possible to include proxied files in other phases?
        if (buildPhase->type() != pbxproj::PBX::BuildPhase::kTypeFrameworks && buildPhase->type() != pbxproj::PBX::BuildPhase::kTypeCopyFiles) {
            continue;
        }

        for (pbxproj::PBX::BuildFile::shared_ptr const &file : buildPhase->files()) {
            switch (file->fileRef()->type()) {
                case pbxproj::PBX::GroupItem::kTypeReferenceProxy: {
                    /* A implicit dependency referencing the product of another target through a direct reference to that target's product. */
                    pbxproj::PBX::ReferenceProxy::shared_ptr proxy = std::static_pointer_cast <pbxproj::PBX::ReferenceProxy> (file->fileRef());

                    pbxproj::PBX::Target::shared_ptr proxiedTarget = ResolveContainerItemProxy(context.buildEnvironment, context.context, target, proxy->remoteRef(), true);
                    if (proxiedTarget != nullptr) {
                        dependencies.insert(proxiedTarget);

#if DEPENDENCY_RESOLVER_LOGGING
                        fprintf(stderr, "debug: implicit dependency: %s %s -> %s %s\n", target->blueprintIdentifier().c_str(), target->name().c_str(), proxiedTarget->blueprintIdentifier().c_str(), proxiedTarget->name().c_str());
#endif

                        /* Recursively search for implicit & explicit dependencies. */
                        AddDependencies(context, proxiedTarget);
                    } else {
                        fprintf(stderr, "warning: was not able to load target for implicit dependency %s (from %s)\n", proxy->remoteRef()->remoteInfo().c_str(), proxy->name().c_str());
                    }
                    break;
                }
                case pbxproj::PBX::GroupItem::kTypeFileReference: {
                    /* A implicit dependency referencing the product of another target through a filesystem path. */
                    pbxproj::PBX::FileReference::shared_ptr fileReference = std::static_pointer_cast<pbxproj::PBX::FileReference>(file->fileRef());
                    pbxsetting::Value path = fileReference->resolve();

                    auto it = context.productPathToTarget->find(path.raw());
                    if (it != context.productPathToTarget->end()) {
                        pbxproj::PBX::Target::shared_ptr dependentTarget = it->second;
                        dependencies.insert(dependentTarget);

#if DEPENDENCY_RESOLVER_LOGGING
                        fprintf(stderr, "debug: implicit dependency: %s %s -> %s %s\n", target->blueprintIdentifier().c_str(), target->name().c_str(), dependentTarget->blueprintIdentifier().c_str(), dependentTarget->name().c_str());
#endif

                        /* Recursively search for implicit & explicit dependencies. */
                        AddDependencies(context, dependentTarget);
                    }
                    break;
                }
                default: {
                    /* Any other type of build file isn't relevant. */
                    break;
                }
            }
        }
    }

    /* If there's no build action, this is a legacy context which always parallelizes builds. */
    if (context.buildAction == nullptr || context.buildAction->parallelizeBuildables()) {
        /* If builds are parallelized, use the actual dependencies for ordering. */
        context.graph->insert(target, dependencies);
    }
}

static void
AddExplicitDependencies(DependenciesContext const &context, pbxproj::PBX::Target::shared_ptr const &target)
{
    std::unordered_set<pbxproj::PBX::Target::shared_ptr> dependencies;

    for (pbxproj::PBX::TargetDependency::shared_ptr const &dependency : target->dependencies()) {
        if (dependency->target() != nullptr) {
            /* A dependency for another target in the same project. */
            dependencies.insert(dependency->target());

#if DEPENDENCY_RESOLVER_LOGGING
            fprintf(stderr, "debug: explicit dependency: %s %s -> %s %s\n", target->blueprintIdentifier().c_str(), target->name().c_str(), dependency->blueprintIdentifier().c_str(), dependency->target()->name().c_str());
#endif

            /* Recursively search for implicit & explicit dependencies. */
            AddDependencies(context, dependency->target());
        } else if (dependency->targetProxy() != nullptr) {
            /* A dependency referencing a target in another project. Get that target. */
            pbxproj::PBX::Target::shared_ptr proxiedTarget = ResolveContainerItemProxy(context.buildEnvironment, context.context, target, dependency->targetProxy(), false);
            if (proxiedTarget != nullptr) {
                dependencies.insert(proxiedTarget);

#if DEPENDENCY_RESOLVER_LOGGING
                fprintf(stderr, "debug: explicit dependency: %s %s -> %s %s\n", target->blueprintIdentifier().c_str(), target->name().c_str(), proxiedTarget->blueprintIdentifier().c_str(), proxiedTarget->name().c_str());
#endif

                /* Recursively search for implicit & explicit dependencies. */
                AddDependencies(context, proxiedTarget);
            } else {
                fprintf(stderr, "warning: was not able to load target for explicit dependency %s\n", dependency->targetProxy()->remoteInfo().c_str());
            }
        }
    }

    /* If there's no build action, this is a legacy context which always parallelizes builds. */
    if (context.buildAction == nullptr || context.buildAction->parallelizeBuildables()) {
        /* If builds are parallelized, use the actual dependencies for ordering. */
        context.graph->insert(target, dependencies);
    }
}

static void
AddDependencies(DependenciesContext const &context, pbxproj::PBX::Target::shared_ptr const &target)
{
    /* If there's no build action, this is a legacy context which always have implicit dependencies. */
    if (context.buildAction != nullptr && context.buildAction->buildImplicitDependencies()) {
        AddImplicitDependencies(context, target);
    }

    AddExplicitDependencies(context, target);

    /* If there's no build action, this is a legacy context which always parallelizes builds. */
    if (context.buildAction != nullptr && !context.buildAction->parallelizeBuildables()) {
        /*
         * Non-parallel targets are currently implemented by adding a dependency from this target
         * on all previous targets seen. This is inefficient: the space used in the graph is O(N^2).
         */
        context.graph->insert(target, *context.positional);
        context.positional->insert(target);
    }
}

static std::unordered_map<std::string, pbxproj::PBX::Target::shared_ptr>
BuildProductPathsToTargets(WorkspaceContext const &workspaceContext)
{
    std::unordered_map<std::string, pbxproj::PBX::Target::shared_ptr> productPathToTarget;

    /*
     * Build a mapping of product path -> target, in order to look up implicit dependencies
     * where the product paths match, but the dependency is not through a container portal.
     */
    for (auto const &pair : workspaceContext.projects()) {
        for (pbxproj::PBX::Target::shared_ptr const &target : pair.second->targets()) {
            if (target->type() != pbxproj::PBX::Target::kTypeNative) {
                /* Only native targets have products. */
                continue;
            }

            pbxproj::PBX::NativeTarget::shared_ptr nativeTarget = std::static_pointer_cast<pbxproj::PBX::NativeTarget>(target);
            pbxproj::PBX::FileReference::shared_ptr const &productReference = nativeTarget->productReference();

            if (productReference != nullptr) {
                /* Use the raw setting value because we can't resolve it yet. */
                pbxsetting::Value productPath = productReference->resolve();
#if DEPENDENCY_RESOLVER_LOGGING
                fprintf(stderr, "debug: product output: %s %s => %s\n", target->blueprintIdentifier().c_str(), target->name().c_str(), productPath.raw().c_str());
#endif
                productPathToTarget.insert({ productPath.raw(), nativeTarget });
            }
        }
    }

    return productPathToTarget;
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

    BuildAction::shared_ptr const &buildAction = scheme->buildAction();
    if (buildAction == nullptr) {
        fprintf(stderr, "error: build action not available\n");
        return graph;
    }

    /* Only create the product path mapping if we have implicit dependencies on to use it. */
    std::unordered_map<std::string, pbxproj::PBX::Target::shared_ptr> productPathToTarget;
    if (buildAction->buildImplicitDependencies()) {
        productPathToTarget = BuildProductPathsToTargets(*context.workspaceContext());
    }

    std::unordered_set<pbxproj::PBX::Target::shared_ptr> positional;
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
            .productPathToTarget = &productPathToTarget,
        };
        AddDependencies(dependenciesContext, target);
    }

#if DEPENDENCY_RESOLVER_LOGGING
    fprintf(stderr, "debug: scheme-based target ordering\n");
    for (pbxproj::PBX::Target::shared_ptr const &target : graph.ordered()) {
        fprintf(stderr, "debug: ordered target %s %s\n", target->blueprintIdentifier().c_str(), target->name().c_str());
    }
#endif

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

    auto productPathToTarget = BuildProductPathsToTargets(*context.workspaceContext());

    std::unordered_set<pbxproj::PBX::Target::shared_ptr> positional;
    for (pbxproj::PBX::Target::shared_ptr const &target : project->targets()) {
        if (!allTargets) {
            if (!targetName.empty() && target->name() != targetName) {
                /* Building a specific target, and not this one. */
                continue;
            } else if (target != project->targets().front()) {
                /* No specific target, build whatever the first target is. */
                continue;
            }
        }

        DependenciesContext dependenciesContext = {
            .buildEnvironment = _buildEnvironment,
            .context = context,
            .graph = &graph,
            .buildAction = nullptr,
            .positional = &positional,
            .productPathToTarget = &productPathToTarget,
        };
        AddDependencies(dependenciesContext, target);
    }

#if DEPENDENCY_RESOLVER_LOGGING
    fprintf(stderr, "debug: legacy target ordering\n");
    for (pbxproj::PBX::Target::shared_ptr const &target : graph.ordered()) {
        fprintf(stderr, "debug: ordered target %s %s\n", target->blueprintIdentifier().c_str(), target->name().c_str());
    }
#endif

    return graph;
}

