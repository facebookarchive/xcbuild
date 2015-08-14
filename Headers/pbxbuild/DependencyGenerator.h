// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_DependencyGenerator_h
#define __pbxbuild_DependencyGenerator_h

#include <pbxbuild/DependencyFile.h>

namespace pbxbuild {

class DependencyGenerator {
private:
    DependencyFile::vector _files;

public:
    DependencyGenerator();

public:
    void clear();

public:
    int scan(std::string const &path,
            string_vector const &arguments = string_vector());
    int scan(std::string const &path,
            std::function <void(std::string const &)> const &callback);
    int scan(std::string const &path, string_vector const &arguments,
            std::function <void(std::string const &)> const &callback);

public:
    int scan(string_set const &paths,
            string_vector const &arguments = string_vector());
    int scan(string_set const &paths,
            std::function <void(std::string const &)> const &callback);
    int scan(string_set const &paths, string_vector const &arguments,
            std::function <void(std::string const &)> const &callback);
    
public:
    int scan(string_vector const &paths,
            string_vector const &arguments = string_vector());
    int scan(string_vector const &paths,
            std::function <void(std::string const &)> const &callback);
    int scan(string_vector const &paths, string_vector const &arguments,
            std::function <void(std::string const &)> const &callback);

public:
    inline DependencyFile::vector const &files() const
    { return _files; }
};

}

#endif  // !__pbxbuild_DependencyGenerator_h
