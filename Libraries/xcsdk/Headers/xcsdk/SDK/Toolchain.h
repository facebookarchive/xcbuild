/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __xcsdk_SDK_Toolchain_h
#define __xcsdk_SDK_Toolchain_h

#include <memory>
#include <string>
#include <vector>

namespace libutil { class Filesystem; };
namespace plist { class Dictionary; }

namespace xcsdk { namespace SDK {

class Manager;

class Toolchain {
public:
    typedef std::shared_ptr <Toolchain> shared_ptr;

private:
    std::string _path;
    std::string _name;
    std::string _identifier;

public:
    Toolchain();
    ~Toolchain();

public:
    inline std::string const &path() const
    { return _path; }

public:
    inline std::string const &name() const
    { return _name; }
    inline std::string const &identifier() const
    { return _identifier; }

public:
    std::vector<std::string> executablePaths() const;

public:
    static Toolchain::shared_ptr Open(libutil::Filesystem const *filesystem, std::string const &path);

public:
    static std::string DefaultIdentifier(void);

private:
    bool parse(plist::Dictionary const *dict);
};

} }

#endif  // !__xcsdk_SDK_Toolchain_h
