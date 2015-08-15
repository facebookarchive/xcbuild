// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __xcsdk_SDK_Toolchain_h
#define __xcsdk_SDK_Toolchain_h

#include <xcsdk/Base.h>

namespace xcsdk { namespace SDK {

class Manager;

class Toolchain {
public:
    typedef std::shared_ptr <Toolchain> shared_ptr;
    typedef std::vector <shared_ptr> vector;
    typedef std::map <std::string, shared_ptr> map;

private:
    std::weak_ptr<Manager> _manager;
    std::string            _path;
    std::string            _identifier;

public:
    Toolchain();
    ~Toolchain();

public:
    inline std::shared_ptr<Manager> const manager() const
    { return _manager.lock(); }

public:
    inline std::string const &path() const
    { return _path; }

public:
    inline std::string const &identifier() const
    { return _identifier; }

public:
    static Toolchain::shared_ptr Open(std::shared_ptr<Manager> manager, std::string const &path);

private:
    bool parse(plist::Dictionary const *dict);
};

} }

#endif  // !__xcsdk_SDK_Toolchain_h
