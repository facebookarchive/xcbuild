// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_XC_Action_h
#define __pbxproj_XC_Action_h

#include <pbxproj/Base.h>

namespace pbxproj { namespace XC {

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

#endif  // !__pbxproj_XC_Action_h
