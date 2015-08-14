// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_DependencyFile_h
#define __pbxbuild_DependencyFile_h

#include <pbxbuild/Base.h>

namespace pbxbuild {

class DependencyFile {
public:
    typedef std::shared_ptr <DependencyFile> shared_ptr;
    typedef std::vector <shared_ptr> vector;

private:
    std::string   _source;
    std::string   _object;
    string_vector _dependencies;

public:
    DependencyFile(std::string const &source, std::string const &object = std::string()) :
        _source(source),
        _object(object)
    { }

public:
    inline void add(std::string const &dependency)
    { _dependencies.push_back(dependency); }

public:
    inline std::string const &source() const
    { return _source; }

public:
    inline std::string const &object() const
    { return _object; }

public:
    inline string_vector const &dependencies() const
    { return _dependencies; }
};

}

#endif  // !__pbxbuild_DependencyFile_h
