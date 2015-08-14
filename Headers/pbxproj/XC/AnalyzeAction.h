// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_XC_AnalyzeAction_h
#define __pbxproj_XC_AnalyzeAction_h

#include <pbxproj/XC/Action.h>

namespace pbxproj { namespace XC {

class AnalyzeAction : public Action {
public:
    typedef std::shared_ptr <AnalyzeAction> shared_ptr;

public:
    AnalyzeAction();

public:
    bool parse(plist::Dictionary const *dict) override;
};

} }

#endif  // !__pbxproj_XC_AnalyzeAction_h
