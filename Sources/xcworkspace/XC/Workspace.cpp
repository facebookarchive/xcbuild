/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <xcworkspace/XC/Workspace.h>
#include <xcworkspace/XC/Group.h>

using xcworkspace::XC::Workspace;
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
    Group G;

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
    for (GroupItem::shared_ptr &item : _items) {
        item->_parent = nullptr;
    }

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
    plist::Object *root = plist::Format::SimpleXML::Read(projectFileName).first;
    if (root == nullptr) {
        return nullptr;
    }

    plist::Dictionary *plist = plist::CastTo<plist::Dictionary>(root);
    if (plist == nullptr) {
        root->release();
        return nullptr;
    }

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

    return workspace;
}
