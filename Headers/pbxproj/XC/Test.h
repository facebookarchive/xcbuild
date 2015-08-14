// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_XC_Test_h
#define __pbxproj_XC_Test_h

#include <pbxproj/Base.h>

namespace pbxproj { namespace XC {

class Test {
public:
    typedef std::shared_ptr <Test> shared_ptr;
    typedef std::vector <shared_ptr> vector;

private:
    std::string _identifier;

public:
    Test();

public:
    inline std::string const &identifier() const
    { return _identifier; }

public:
    bool parse(plist::Dictionary const *dict);
};

} }

#endif  // !__pbxproj_XC_Test_h
