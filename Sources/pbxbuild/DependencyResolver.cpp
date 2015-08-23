// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/DependencyResolver.h>
#include <list>

using pbxbuild::DependencyResolver;
using pbxbuild::SchemeContext;
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

template<typename T>
class Graph {
private:
    std::map<T, std::vector<T>> _contents;

public:
    void insert(T node, std::vector<T> const &children)
    {
        auto it = _contents.find(node);
        if (it == _contents.end()) {
            _contents.insert(std::make_pair(node, children));
        } else {
            std::vector<T> &existing = it->second;
            existing.insert(existing.end(), children.begin(), children.end());
        }
    }

    std::vector<T> children(T node) const
    {
        auto it = _contents.get(node);
        if (it != _contents.end()) {
            return it->second;
        } else {
            return std::vector<T>();
        }
    }

    std::vector<T> ordered(void) const
    {
        std::vector<T> result;

        std::list<T> toExplore;
        std::transform(_contents.begin(), _contents.end(), std::back_inserter(toExplore), [](auto const &pair) {
            return pair.first;
        });

        std::unordered_set<T> inProgress;
        std::unordered_set<T> explored;

        while (!toExplore.empty()) {
            T node = toExplore.front();
            if (explored.find(node) != explored.end()) {
                toExplore.pop_front();
                continue;
            }

            inProgress.insert(node);

            size_t stack = toExplore.size();
            for (T const &child : _contents.find(node)->second) {
                if (inProgress.find(child) != inProgress.end()) {
                    fprintf(stderr, "Cycle detected!\n");
                    return std::vector<T>();
                }

                if (explored.find(child) == explored.end()) {
                    toExplore.push_front(child);
                    break;
                }
            }

            if (stack == toExplore.size()) {
                toExplore.pop_front();
                inProgress.erase(node);
                explored.insert(node);
                result.push_back(node);
            }
        }

        assert(inProgress.empty());
        return result;
    }
};

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
    Graph<pbxproj::PBX::Target::shared_ptr> *graph;
    BuildAction::shared_ptr buildAction;
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

    context.graph->insert(target, dependencies);
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

    context.graph->insert(target, dependencies);
}

static void
AddDependencies(DependenciesContext const &context, pbxproj::PBX::Project::shared_ptr const &project, pbxproj::PBX::Target::shared_ptr const &target)
{
    if (context.buildAction->buildImplicitDependencies()) {
        AddImplicitDependencies(context, project, target);
    }

    AddExplicitDependencies(context, project, target);
}

std::vector<pbxproj::PBX::Target::shared_ptr> DependencyResolver::
resolveDependencies(void) const
{
    BuildAction::shared_ptr buildAction = _context->scheme()->buildAction();
    if (buildAction == nullptr) {
        fprintf(stderr, "Couldn't get build action.");
        return std::vector<pbxproj::PBX::Target::shared_ptr>();
    }

    Graph<pbxproj::PBX::Target::shared_ptr> graph;

    std::vector<pbxproj::PBX::Target::shared_ptr> positional;
    for (BuildActionEntry::shared_ptr const &entry : buildAction->buildActionEntries()) {
        pbxproj::PBX::Project::shared_ptr project = nullptr;
        pbxproj::PBX::Target::shared_ptr target = ResolveBuildableReference(_context, entry->buildableReference(), &project);

        if (target != nullptr) {
            if (!buildAction->parallelizeBuildables()) {
                graph.insert(target, positional);
            }
            positional.push_back(target);

            DependenciesContext dependenciesContext = {
                .context = _context,
                .graph = &graph,
                .buildAction = buildAction,
            };
            AddDependencies(dependenciesContext, project, target);
        }
    }

    return graph.ordered();
}

