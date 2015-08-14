// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxsdk_SDK_Toolchain_h
#define __pbxsdk_SDK_Toolchain_h

#include <pbxsdk/Base.h>

namespace pbxsdk { namespace SDK {

class Toolchain {
public:
    typedef std::shared_ptr <Toolchain> shared_ptr;
    typedef std::vector <shared_ptr> vector;

private:
    std::string        _path;
    std::string        _identifier;

public:
    Toolchain();
    ~Toolchain();

public:
    inline std::string const &path() const
    { return _path; }

public:
    inline std::string const &identifier() const
    { return _identifier; }

public:
    static Toolchain::shared_ptr Open(std::string const &path);

private:
    bool parse(plist::Dictionary const *dict);
};

} }

#endif  // !__pbxsdk_SDK_Toolchain_h
