/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __libutil_Options_h
#define __libutil_Options_h

#include <libutil/Base.h>

namespace libutil {

struct Options {
private:
    Options();
    ~Options();

public:
    static std::pair<bool, std::string>
    NextString(std::string *result, std::vector<std::string> const &args, std::vector<std::string>::const_iterator *it, bool allowDuplicate = false);
    static std::pair<bool, std::string>
    NextInt(int *result, std::vector<std::string> const &args, std::vector<std::string>::const_iterator *it, bool allowDuplicate = false);
    static std::pair<bool, std::string>
    NextBool(bool *result, std::vector<std::string> const &args, std::vector<std::string>::const_iterator *it, bool allowDuplicate = false);
    static std::pair<bool, std::string>
    MarkBool(bool *result, std::string const &arg, bool allowDuplicate = false);

public:
    template<typename T>
    static std::pair<bool, std::string>
    Parse(T *options, std::vector<std::string> const &args)
    {
        std::string error;

        for (auto it = args.begin(); it != args.end(); ++it) {
            std::pair<bool, std::string> result = options->parseArgument(args, &it);
            if (!result.first) {
                return result;
            }
        }

        return std::make_pair<bool, std::string>(true, std::string());
    }
};

}

#endif  // !__libutil_Options_h
