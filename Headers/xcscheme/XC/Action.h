// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __xcscheme_XC_Action_h
#define __xcscheme_XC_Action_h

#include <xcscheme/Base.h>

namespace xcscheme { namespace XC {

class Action {
private:
    std::string _buildConfiguration;

public:
    Action();

public:
    inline std::string const &buildConfiguration() const
    { return _buildConfiguration; }

protected:
    virtual bool parse(plist::Dictionary const *dict);
};

} }

#endif  // !__xcscheme_XC_Action_h
