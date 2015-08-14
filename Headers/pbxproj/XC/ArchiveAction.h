// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_XC_ArchiveAction_h
#define __pbxproj_XC_ArchiveAction_h

#include <pbxproj/XC/Action.h>

namespace pbxproj { namespace XC {

class ArchiveAction : public Action {
public:
    typedef std::shared_ptr <ArchiveAction> shared_ptr;

private:
    bool _revealArchiveInOrganizer;

public:
    ArchiveAction();

public:
    inline bool revealArchiverInOrganize() const
    { return _revealArchiveInOrganizer; }

public:
    bool parse(plist::Dictionary const *dict) override;
};

} }

#endif  // !__pbxproj_XC_ArchiveAction_h
