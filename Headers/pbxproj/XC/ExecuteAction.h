// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_XC_ExecuteAction_h
#define __pbxproj_XC_ExecuteAction_h

#include <pbxproj/XC/ActionContent.h>

namespace pbxproj { namespace XC {

class ExecuteAction {
public:
    typedef std::shared_ptr <ExecuteAction> shared_ptr;
    typedef std::vector <shared_ptr> vector;

private:
    std::string               _actionType;
    ActionContent::shared_ptr _actionContent;

public:
    ExecuteAction();

public:
    inline std::string const &actionType()
    { return _actionType; }

public:
    inline ActionContent::shared_ptr const &actionContent() const
    { return _actionContent; }
    inline ActionContent::shared_ptr &actionContent()
    { return _actionContent; }

public:
    bool parse(plist::Dictionary const *dict);
};

} }

#endif  // !__pbxproj_XC_ExecuteAction_h
