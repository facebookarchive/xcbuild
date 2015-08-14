// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_XC_BuildAction_h
#define __pbxproj_XC_BuildAction_h

#include <pbxproj/XC/Action.h>
#include <pbxproj/XC/BuildActionEntry.h>
#include <pbxproj/XC/ExecuteAction.h>

namespace pbxproj { namespace XC {

class BuildAction : public Action {
public:
    typedef std::shared_ptr <BuildAction> shared_ptr;

private:
    bool                     _buildImplicitDependencies;
    bool                     _parallelizeBuildables;
    BuildActionEntry::vector _buildActionEntries;
    ExecuteAction::vector    _preActions;
    ExecuteAction::vector    _postActions;

public:
    BuildAction();

public:
    inline bool buildImplicitDependencies() const
    { return _buildImplicitDependencies; }

public:
    inline bool parallelizeBuildables() const
    { return _parallelizeBuildables; }

public:
    inline BuildActionEntry::vector const &buildActionEntries() const
    { return _buildActionEntries; }
    inline BuildActionEntry::vector &buildActionEntries()
    { return _buildActionEntries; }

public:
    inline ExecuteAction::vector const &preActions() const
    { return _preActions; }
    inline ExecuteAction::vector &preActions()
    { return _preActions; }

public:
    inline ExecuteAction::vector const &postActions() const
    { return _postActions; }
    inline ExecuteAction::vector &postActions()
    { return _postActions; }

public:
    bool parse(plist::Dictionary const *dict) override;
};

} }

#endif  // !__pbxproj_XC_BuildAction_h
