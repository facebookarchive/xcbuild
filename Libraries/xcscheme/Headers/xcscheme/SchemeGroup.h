/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __xcscheme_SchemeGroup_h
#define __xcscheme_SchemeGroup_h

#include <xcscheme/Base.h>
#include <xcscheme/XC/Scheme.h>

namespace libutil { class Filesystem; }

namespace xcscheme {

class SchemeGroup {
public:
    typedef std::shared_ptr <SchemeGroup> shared_ptr;

private:
    std::string                      _basePath;
    std::string                      _path;
    std::string                      _name;
    xcscheme::XC::Scheme::vector     _schemes;
    xcscheme::XC::Scheme::shared_ptr _defaultScheme;

public:
    SchemeGroup();

public:
    inline std::string const &basePath() const
    { return _basePath; }
    inline std::string const &path() const
    { return _path; }
    inline std::string const &name() const
    { return _name; }

public:
    inline xcscheme::XC::Scheme::vector const &schemes() const
    { return _schemes; }
    inline xcscheme::XC::Scheme::vector &schemes()
    { return _schemes; }

public:
    inline xcscheme::XC::Scheme::shared_ptr const &defaultScheme() const
    { return _defaultScheme; }
    inline xcscheme::XC::Scheme::shared_ptr &defaultScheme()
    { return _defaultScheme; }

public:
    /* Finds a scheme inside the group. */
    xcscheme::XC::Scheme::shared_ptr scheme(std::string const &name) const;

public:
    static SchemeGroup::shared_ptr Open(libutil::Filesystem const *filesystem, std::string const &basePath, std::string const &path, std::string const &name);
};

}

#endif // !__xcscheme_SchemeGroup_h
