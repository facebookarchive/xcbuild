// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __xcscheme_XC_AnalyzeAction_h
#define __xcscheme_XC_AnalyzeAction_h

#include <xcscheme/XC/Action.h>

namespace xcscheme { namespace XC {

class AnalyzeAction : public Action {
public:
    typedef std::shared_ptr <AnalyzeAction> shared_ptr;

public:
    AnalyzeAction();

public:
    bool parse(plist::Dictionary const *dict) override;
};

} }

#endif  // !__xcscheme_XC_AnalyzeAction_h
