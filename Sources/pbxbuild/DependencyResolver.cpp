// Copyright 2013-present Facebook. All Rights Reserved.

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

    if (productReference) {
        auto result = context.resolveProductIdentifier(context.project(path), proxy->remoteGlobalIDString());
        if (result == nullptr) {
            fprintf(stderr, "warning: not able to resolve product identifier %s in project %s\n", proxy->remoteGlobalIDString().c_str(), context.project(path) ? context.project(path)->name().c_str() : path.c_str());
            return nullptr;
        }

        return result->first;
    } else {
        return context.resolveTargetIdentifier(context.project(path), proxy->remoteGlobalIDString());
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
            pbxproj::PBX::ReferenceProxy::shared_ptr proxy = file->referenceProxy();
            if (proxy == nullptr) {
                continue;
            }

            pbxproj::PBX::Target::shared_ptr proxiedTarget = ResolveContainerItemProxy(context.buildEnvironment, context.context, target, proxy->remoteRef(), true);
            if (proxiedTarget != nullptr) {
                dependencies.push_back(proxiedTarget);
                AddDependencies(context, proxiedTarget);
            } else {
                fprintf(stderr, "warning: was not able to load target for implicit dependency %s (from %s)\n", proxy->remoteRef()->remoteInfo().c_str(), proxy->name().c_str());
            }
        }
    }

    if (context.buildAction->parallelizeBuildables()) {
        context.graph->insert(target, dependencies);
    }
}

static void
AddExplicitDependencies(DependenciesContext const &context, pbxproj::PBX::Target::shared_ptr const &target)
{
    std::vector<pbxproj::PBX::Target::shared_ptr> dependencies;

    for (pbxproj::PBX::TargetDependency::shared_ptr const &dependency : target->dependencies()) {
        if (dependency->target() != nullptr) {
            dependencies.push_back(dependency->target());
            AddDependencies(context, dependency->target());
        } else if (dependency->targetProxy() != nullptr) {
            pbxproj::PBX::Target::shared_ptr proxiedTarget = ResolveContainerItemProxy(context.buildEnvironment, context.context, target, dependency->targetProxy(), false);
            if (proxiedTarget != nullptr) {
                dependencies.push_back(proxiedTarget);
                AddDependencies(context, proxiedTarget);
            } else {
                fprintf(stderr, "warning: was not able to load target for explicit dependency %s\n", dependency->targetProxy()->remoteInfo().c_str());
            }
        }
    }

    if (context.buildAction->parallelizeBuildables()) {
        context.graph->insert(target, dependencies);
    }
}

static void
AddDependencies(DependenciesContext const &context, pbxproj::PBX::Target::shared_ptr const &target)
{
    if (context.buildAction->buildImplicitDependencies()) {
        AddImplicitDependencies(context, target);
    }

    AddExplicitDependencies(context, target);

    if (!context.buildAction->parallelizeBuildables()) {
        // FIXME(grp): This is inefficient, we just need this list.
        context.graph->insert(target, *context.positional);
        context.positional->push_back(target);
    }
}

BuildGraph<pbxproj::PBX::Target::shared_ptr> DependencyResolver::
resolveDependencies(BuildContext const &context) const
{
    BuildGraph<pbxproj::PBX::Target::shared_ptr> graph;

    BuildAction::shared_ptr buildAction = context.scheme()->buildAction();
    if (buildAction == nullptr) {
        fprintf(stderr, "Couldn't get build action.");
        return graph;
    }

    std::vector<pbxproj::PBX::Target::shared_ptr> positional;
    for (BuildActionEntry::shared_ptr const &entry : buildAction->buildActionEntries()) {
        // TODO(grp): Check the buildFor* flags against the BuildContext.
        if (!entry->buildForRunning()) {
            continue;
        }

        xcscheme::XC::BuildableReference::shared_ptr const &reference = entry->buildableReference();
        std::string projectPath = reference->resolve(context.workspace()->basePath());
        pbxproj::PBX::Target::shared_ptr target = context.resolveTargetIdentifier(context.project(projectPath), reference->blueprintIdentifier());
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

