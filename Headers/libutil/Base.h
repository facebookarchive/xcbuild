// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __libutil_Base_h
#define __libutil_Base_h

#include <algorithm>
#include <functional>
#include <cctype>
#include <cassert>
#include <locale>
#include <string>
#include <vector>
#include <memory>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <iostream>

namespace libutil {

typedef std::vector <std::string> string_vector;
typedef std::set <std::string> string_set;
typedef std::map <std::string, std::string> string_map;

// trim from start
static inline std::string &ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
static inline std::string &rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(),
                std::not1(std::ptr_fun<int, int>(std::isspace))).base(),
            s.end());
    return s;
}

// trim from both ends
static inline std::string &trim(std::string &s)
{
    return ltrim(rtrim(s));
}

}

#endif  // !__libutil_Base_h
