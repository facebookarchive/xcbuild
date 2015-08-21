// Copyright 2013-present Facebook. All Rights Reserved.

#include <xcworkspace/XC/Workspace.h>
#include <xcworkspace/XC/WorkspaceGroup.h>

using xcworkspace::XC::Workspace;
using xcscheme::XC::Scheme;
using libutil::FSUtil;
using libutil::SysUtil;

Workspace::Workspace()
{
}

bool Workspace::
parse(plist::Dictionary const *dict)
{
    //
    // The workspace is a group of itself, we add some extra checks
    // like "version" shall be 1.0.
    //
    WorkspaceGroup G;

    auto W = dict->value <plist::Dictionary> ("Workspace");
    if (W == nullptr)
        return false;

    auto V = W->value <plist::String> ("version");
    if (V != nullptr && V->value() != "1.0") {
        fprintf(stderr, "warning: xcode workspace version %s may be unsupported\n",
                V->value().c_str());
    }

    if (!G.parse(W))
        return false;

    //
    // Now move the contents
    //
    _items = std::move(G.items());

    return true;
}

Workspace::shared_ptr Workspace::
Open(std::string const &path)
{
    if (path.empty()) {
        errno = EINVAL;
        return nullptr;
    }

    std::string projectFileName = path + "/contents.xcworkspacedata";
    if (!FSUtil::TestForRead(projectFileName))
        return nullptr;

    std::string realPath = FSUtil::ResolvePath(projectFileName);
    if (realPath.empty())
        return nullptr;

    //
    // Parse property list
    //
    plist::Dictionary *plist = plist::Dictionary::ParseSimpleXML(projectFileName);
    if (plist == nullptr)
        return nullptr;

    //
    // Create the workspace object and parse the property list
    //
    auto workspace = std::make_shared <Workspace> ();
    if (workspace->parse(plist)) {
        //
        // Save some useful info
        //
        workspace->_projectFile = FSUtil::GetDirectoryName(realPath);

        size_t slash = workspace->_projectFile.rfind('/');
        size_t dot   = workspace->_projectFile.rfind('.');

        workspace->_basePath    = workspace->_projectFile.substr(0, slash);
        workspace->_name        = workspace->_projectFile.substr(slash + 1, dot - slash - 1);
    } else {
        workspace = nullptr;
    }

    plist->release();

    if (!workspace)
        return nullptr;

    //
    // Now open schemes, first shared and then current user.
    //
    std::string schemePath;
    
    schemePath = path + "/xcshareddata/xcschemes";
    FSUtil::EnumerateDirectory(schemePath, "*.xcscheme",
            [&](std::string const &filename) -> bool
            {
                std::string name = filename.substr(0, filename.find('.'));
                auto scheme = Scheme::Open(name, std::string(), schemePath + "/" + filename);
                if (!scheme) {
                    fprintf(stderr, "warning: failed parsing shared scheme '%s'\n", name.c_str());
                } else {
                    workspace->_schemes.push_back(scheme);
                }

                if (!workspace->_defaultScheme && name == workspace->name()) {
                    workspace->_defaultScheme = scheme;
                }
                return true;
            });

    std::string userName = SysUtil::GetUserName();
    if (!userName.empty()) {
        schemePath = path + "/xcuserdata/" + userName + ".xcuserdatad/xcschemes";
        FSUtil::EnumerateDirectory(schemePath, "*.xcscheme",
                [&](std::string const &filename) -> bool
                {
                    std::string name = filename.substr(0, filename.find('.'));
                    auto scheme = Scheme::Open(name, userName, schemePath + "/" + filename);
                    if (!scheme) {
                        fprintf(stderr, "warning: failed parsing user scheme '%s'\n", name.c_str());
                    } else {
                        workspace->_schemes.push_back(scheme);
                    }

                    if (!workspace->_defaultScheme && name == workspace->name()) {
                        workspace->_defaultScheme = scheme;
                    }
                    return true;
                });
    }

    if (!workspace->_schemes.empty() && !workspace->_defaultScheme) {
        workspace->_defaultScheme = workspace->_schemes[0];
    }

    return workspace;
}
