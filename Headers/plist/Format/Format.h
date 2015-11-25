/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __plist_Format_Format_h
#define __plist_Format_Format_h

#include <plist/Base.h>
#include <plist/Object.h>

#include <fstream>

namespace plist {
namespace Format {

template<typename T>
class Format {
protected:
    Format() { };
    ~Format() { };

public:
    static std::unique_ptr<T>
    Identify(std::vector<uint8_t> const &contents);

public:
    static std::pair<Object *, std::string>
    Deserialize(std::vector<uint8_t> const &contents, T const &format);

public:
    static std::pair<std::unique_ptr<std::vector<uint8_t>>, std::string>
    Serialize(Object const *object, T const &format);

public:
    static std::pair<Object *, std::string>
    Read(std::string const &path)
    {
        std::ifstream file = std::ifstream(path, std::ios::binary);
        std::vector<uint8_t> contents = std::vector<uint8_t>(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

        std::unique_ptr<T> format = Identify(contents);
        if (format == nullptr) {
            return std::make_pair(nullptr, "couldn't identify format");
        }

        return Deserialize(contents, *format);
    }

public:
    static std::pair<bool, std::string>
    Write(std::string const &path, Object const *object, T const &format)
    {
        auto result = Serialize(object, format);
        if (result.first == nullptr) {
            return std::make_pair(false, result.second);
        }

        std::ofstream file = std::ofstream(path, std::ios::binary);
        std::copy(result.first->begin(), result.first->end(), std::ostream_iterator<char>(file));

        return std::make_pair(true, std::string());
    }
};

}
}

#endif  // !__plist_Format_Format_h
