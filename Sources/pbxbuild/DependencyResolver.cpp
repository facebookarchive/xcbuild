// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/DependencyResolver.h>
#include <pbxbuild/BuildGraph.h>

using pbxbuild::DependencyResolver;
using pbxbuild::SchemeContext;
using pbxbuild::BuildGraph;
using xcscheme::XC::Scheme;
using xcscheme::XC::BuildAction;
using xcscheme::XC::BuildActionEntry;
using libutil::FSUtil;

DependencyResolver::
DependencyResolver(SchemeContext::shared_ptr context) :
    _context(context)
{
}

DependencyResolver::
~DependencyResolver()
{
}

static pbxproj::PBX::Target::shared_ptr
ResolveTargetIdentifier(SchemeContext::shared_ptr const &context, std::string const &projectPath, std::string const &identifier, pbxproj::PBX::Project::shared_ptr *outProject = nullptr)
{
    pbxproj::PBX::Project::shared_ptr project;
    auto PI = context->projects().find(projectPath);
    if (PI != context->projects().end()) {
        project = PI->second;
    } else {
        project = pbxproj::PBX::Project::Open(projectPath);
        if (project == nullptr) {
            return nullptr;
        }
    }

    if (outProject != nullptr) {
        *outProject = project;
    }

    pbxproj::PBX::Target::shared_ptr foundTarget = nullptr;
    for (pbxproj::PBX::Target::shared_ptr const &target : project->targets()) {
        if (target->blueprintIdentifier() == identifier) {
            // Explicit dependency, identifier is a target.
            foundTarget = target;
            break;
        } else if (target->type() == pbxproj::PBX::Target::kTypeNative) {
            // Implicit dependency, identifier is a native target's product.
            pbxproj::PBX::NativeTarget const *nativeTarget = reinterpret_cast<pbxproj::PBX::NativeTarget const *>(target.get());
            if (nativeTarget->productReference() != nullptr && nativeTarget->productReference()->blueprintIdentifier() == identifier) {
                foundTarget = target;
                break;
            }
        }
    }

    if (foundTarget != nullptr) {
        context->projects()[projectPath] = project;
    }

    return foundTarget;
}

static pbxproj::PBX::Target::shared_ptr
ResolveBuildableReference(SchemeContext::shared_ptr const &context, xcscheme::XC::BuildableReference::shared_ptr const &reference, pbxproj::PBX::Project::shared_ptr *outProject)
{
    std::string path = reference->resolve(context->workspace()->basePath());
    return ResolveTargetIdentifier(context, path, reference->blueprintIdentifier(), outProject);
}

static pbxproj::PBX::Target::shared_ptr
ResolveContainerItemProxy(SchemeContext::shared_ptr const &context, pbxproj::PBX::Project::shared_ptr const &project, pbxproj::PBX::ContainerItemProxy::shared_ptr const &proxy, pbxproj::PBX::Project::shared_ptr *outProject)
{
    pbxproj::PBX::FileReference::shared_ptr fileReference = proxy->containerPortal();
    if (fileReference == nullptr) {
        return nullptr;
    }

    std::vector<pbxsetting::Level> levels = context->environment().assignment();
    levels.insert(levels.begin(), project->settings());
    levels.push_back(project->settings());
    pbxsetting::Environment projectEnvironment = pbxsetting::Environment(levels, levels);

    std::string path = projectEnvironment.expand(fileReference->resolve());
    return ResolveTargetIdentifier(context, path, proxy->remoteGlobalIDString(), outProject);
}

struct DependenciesContext {
    SchemeContext::shared_ptr context;
    BuildGraph *graph;
    BuildAction::shared_ptr buildAction;
    std::vector<pbxproj::PBX::Target::shared_ptr> *positional;
};

static void
AddDependencies(DependenciesContext const &context, pbxproj::PBX::Project::shared_ptr const &project, pbxproj::PBX::Target::shared_ptr const &target);

static void
AddImplicitDependencies(DependenciesContext const &context, pbxproj::PBX::Project::shared_ptr const &project, pbxproj::PBX::Target::shared_ptr const &target)
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

            pbxproj::PBX::Project::shared_ptr proxiedProject = nullptr;
            pbxproj::PBX::Target::shared_ptr proxiedTarget = ResolveContainerItemProxy(context.context, project, proxy->remoteRef(), &proxiedProject);
            if (proxiedTarget != nullptr) {
                dependencies.push_back(proxiedTarget);
                AddDependencies(context, proxiedProject, proxiedTarget);
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
AddExplicitDependencies(DependenciesContext const &context, pbxproj::PBX::Project::shared_ptr const &project, pbxproj::PBX::Target::shared_ptr const &target)
{
    std::vector<pbxproj::PBX::Target::shared_ptr> dependencies;

    for (pbxproj::PBX::TargetDependency::shared_ptr const &dependency : target->dependencies()) {
        if (dependency->target() != nullptr) {
            dependencies.push_back(dependency->target());
        } else if (dependency->targetProxy() != nullptr) {
            pbxproj::PBX::Project::shared_ptr proxiedProject = nullptr;
            pbxproj::PBX::Target::shared_ptr proxiedTarget = ResolveContainerItemProxy(context.context, project, dependency->targetProxy(), &proxiedProject);
            if (proxiedTarget != nullptr) {
                dependencies.push_back(proxiedTarget);
                AddDependencies(context, proxiedProject, proxiedTarget);
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
AddDependencies(DependenciesContext const &context, pbxproj::PBX::Project::shared_ptr const &project, pbxproj::PBX::Target::shared_ptr const &target)
{
    if (context.buildAction->buildImplicitDependencies()) {
        AddImplicitDependencies(context, project, target);
    }

    AddExplicitDependencies(context, project, target);

    if (!context.buildAction->parallelizeBuildables()) {
        // FIXME(grp): This is inefficient, we just need this list.
        context.graph->insert(target, *context.positional);
        context.positional->push_back(target);
    }
}

BuildGraph DependencyResolver::
resolveDependencies(void) const
{
    BuildGraph graph;

    BuildAction::shared_ptr buildAction = _context->scheme()->buildAction();
    if (buildAction == nullptr) {
        fprintf(stderr, "Couldn't get build action.");
        return graph;
    }

    std::vector<pbxproj::PBX::Target::shared_ptr> positional;
    for (BuildActionEntry::shared_ptr const &entry : buildAction->buildActionEntries()) {
        pbxproj::PBX::Project::shared_ptr project = nullptr;
        pbxproj::PBX::Target::shared_ptr target = ResolveBuildableReference(_context, entry->buildableReference(), &project);

        if (target != nullptr) {
            DependenciesContext dependenciesContext = {
                .context = _context,
                .graph = &graph,
                .buildAction = buildAction,
                .positional = &positional,
            };
            AddDependencies(dependenciesContext, project, target);
        }
    }

    return graph;
}

