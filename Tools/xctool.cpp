// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxproj/pbxproj.h>

#include <cstring>
#include <cerrno>

using namespace pbxproj;
using namespace libutil;

void DumpItem(XC::Workspace const &W, XC::WorkspaceGroupItem const *item, size_t indent);
void DumpItems(XC::Workspace const &W, XC::WorkspaceGroupItem::vector const &items, size_t indent);

std::string
MakePath(XC::Workspace const &W, XC::WorkspaceGroup const *group,
        std::string const &location, bool real = true)
{
    std::string GL;

    if (group != nullptr) {
        GL = group->location();
    }

    if (real) {
        return FSUtil::ResolvePath(W.basePath() + "/" + GL + "/" + location);
    } else {
        if (GL.empty()) {
            return W.basePath() + "/" + location;
        } else {
            return W.basePath() + "/" + GL + "/" + location;
        }
    }
}

void
DumpFileRef(XC::Workspace const &W,
            XC::WorkspaceFileRef const *fref, size_t indent)
{
    printf("%*s%s [%s]\n", (int)(indent * 2), "",
            MakePath(W, nullptr, fref->location()).c_str(),
            fref->locationType().c_str());
}

void
DumpGroup(XC::Workspace const &W,
          XC::WorkspaceGroup const *group, size_t indent)
{
    printf("%*s[%s] (%s [%s])\n", (int)(indent * 2), "",
            group->name().c_str(),
            MakePath(W, nullptr, group->location()).c_str(),
            group->locationType().c_str());

    DumpItems(W, group->items(), indent + 1);
}

void
DumpItem(XC::Workspace const &W, XC::WorkspaceGroupItem const *item, size_t indent)
{
    switch (item->type()) {
        case XC::WorkspaceGroupItem::kTypeGroup:
            DumpGroup(W, static_cast <XC::WorkspaceGroup const *> (item), indent);
            break;
        case XC::WorkspaceGroupItem::kTypeFileRef:
            DumpFileRef(W, static_cast <XC::WorkspaceFileRef const *> (item), indent);
            break;
        default:
            break;
    }
}

void
DumpItems(XC::Workspace const &W, XC::WorkspaceGroupItem::vector const &items, size_t indent)
{
    for (auto item : items) {
        DumpItem(W, item.get(), indent);
    }
}

void
DumpItems(XC::Workspace const &W, size_t indent = 0)
{
    DumpItems(W, W.items(), indent);
}

void ForEachItems(XC::Workspace const &W, XC::WorkspaceGroup const *group,
        XC::WorkspaceGroupItem::vector const &items,
        std::function<void(XC::WorkspaceGroup const *, XC::WorkspaceFileRef const &)> const &cb);

void
ForEachFileRef(XC::Workspace const &W, XC::WorkspaceGroup const *group,
        XC::WorkspaceFileRef const *fref,
        std::function<void(XC::WorkspaceGroup const *, XC::WorkspaceFileRef const &)> const &cb)
{
    cb(group, *fref);
}

void
ForEachGroup(XC::Workspace const &W, XC::WorkspaceGroup const *group,
        std::function<void(XC::WorkspaceGroup const *, XC::WorkspaceFileRef const &)> const &cb)
{
    ForEachItems(W, group, group->items(), cb);
}

void
ForEachItem(XC::Workspace const &W, XC::WorkspaceGroup const *group,
        XC::WorkspaceGroupItem const *item,
        std::function<void(XC::WorkspaceGroup const *, XC::WorkspaceFileRef const &)> const &cb)
{
    switch (item->type()) {
        case XC::WorkspaceGroupItem::kTypeGroup:
            ForEachGroup(W, static_cast <XC::WorkspaceGroup const *> (item), cb);
            break;
        case XC::WorkspaceGroupItem::kTypeFileRef:
            ForEachFileRef(W, group, static_cast <XC::WorkspaceFileRef const *> (item), cb);
            break;
        default:
            break;
    }
}

void
ForEachItems(XC::Workspace const &W, XC::WorkspaceGroup const *group,
        XC::WorkspaceGroupItem::vector const &items,
        std::function<void(XC::WorkspaceGroup const *, XC::WorkspaceFileRef const &)> const &cb)
{
    for (auto item : items) {
        ForEachItem(W, group, item.get(), cb);
    }
}

void
ForEachFileRef(XC::Workspace const &W,
        std::function<void(XC::WorkspaceGroup const *, XC::WorkspaceFileRef const &)> const &cb)
{
    ForEachItems(W, nullptr, W.items(), cb);
}

int
main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage: %s filename.xcworkspace\n", argv[0]);
        return -1;
    }

    auto workspace = XC::Workspace::Open(argv[1]);
    if (!workspace) {
        fprintf(stderr, "error opening workspace (%s)\n",
                strerror(errno));
        return -1;
    }

#if 1
    fprintf(stderr, "parse ok\n");
    printf("Workspace File: %s\n", workspace->projectFile().c_str());
    printf("Base Path:      %s\n", workspace->basePath().c_str());
    printf("Name:           %s\n", workspace->name().c_str());

    printf("Schemes:\n");
    for (auto &I : workspace->schemes()) {
        printf("\t%s [%s]%s\n", I->name().c_str(),
                I->shared() ? "Shared" : I->owner().c_str(),
                I == workspace->defaultScheme() ? " (DEFAULT)" : "");
    }

    DumpItems(*workspace);

    PBX::Project::vector projects;
    ForEachFileRef(*workspace,
            [&](XC::WorkspaceGroup const *g, XC::WorkspaceFileRef const &fref)
            {
                std::string path = MakePath(*workspace, g, fref.location(), false);
                printf("opening %s\n", path.c_str());

                auto project = PBX::Project::Open(path);
                if (!project) {
                    printf("couldn't open %s\n", path.c_str());
                }
                projects.push_back(project);
            });

    projects.clear();
#endif

    //
    // Collect all schemes
    //
    XC::Scheme::vector schemes;
    schemes.insert(schemes.end(),
                   workspace->schemes().begin(), 
                   workspace->schemes().end());
    ForEachFileRef(*workspace,
            [&](XC::WorkspaceGroup const *g, XC::WorkspaceFileRef const &fref)
            {
                std::string path = MakePath(*workspace, g, fref.location(), false);
                auto project = PBX::Project::Open(path);
                if (project) {
                    schemes.insert(schemes.end(),
                                   project->schemes().begin(),
                                   project->schemes().end());
                }
            });

    printf("Information about workspace \"%s\":\n",
            workspace->name().c_str());
    if (schemes.empty()) {
        printf("\n%4sThis workspace contains no scheme.\n", "");
    } else {
        std::sort(schemes.begin(), schemes.end(),
                [](XC::Scheme::shared_ptr const &a, XC::Scheme::shared_ptr const &b) -> bool
                {
                    return ::strcasecmp(a->name().c_str(), b->name().c_str()) < 0;
                });

        auto I = std::unique(schemes.begin(), schemes.end(),
                [](XC::Scheme::shared_ptr const &a, XC::Scheme::shared_ptr const &b) -> bool
                {
                    return (a->path() == b->path());
                });
        schemes.resize(std::distance(schemes.begin(), I));

        printf("%4sSchemes:\n", "");
        for (auto scheme : schemes) {
            printf("%8s%s\n", "", scheme->name().c_str());
        }
        printf("\n");
    }

    return 0;
}
