// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxsdk_SDK_Manager_h
#define __pbxsdk_SDK_Manager_h

#include <pbxsdk/SDK/Platform.h>

namespace pbxsdk { namespace SDK {

class Manager {
public:
    typedef std::shared_ptr <Manager> shared_ptr;
    typedef std::vector <shared_ptr> vector;

private:
    std::string          _path;
    Platform::vector     _platforms;
    Toolchain::vector    _toolchains;

public:
    Manager();
    ~Manager();

public:
    inline Platform::vector const &platforms() const
    { return _platforms; }
    inline Toolchain::vector const &toolchains() const
    { return _toolchains; }

public:
    inline std::string const &path() const
    { return _path; }

public:
    static Manager::shared_ptr Open(std::string const &path);
};

} }

#endif  // !__pbxsdk_SDK_Manager_h
