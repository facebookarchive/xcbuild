// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __xcscheme_XC_AdditionalOption_h
#define __xcscheme_XC_AdditionalOption_h

#include <xcscheme/Base.h>

namespace xcscheme { namespace XC {

class AdditionalOption {
public:
    typedef std::shared_ptr <AdditionalOption> shared_ptr;
    typedef std::vector <shared_ptr> vector;

private:
    bool        _isEnabled;
    std::string _key;
    std::string _value;

public:
    AdditionalOption();

public:
    inline bool isEnabled() const
    { return _isEnabled; }

public:
    inline std::string const &key() const
    { return _key; }
    inline std::string const &value() const
    { return _value; }

public:
    inline std::pair <std::string, std::string> pair() const
    { return std::make_pair(key(), value()); }

public:
    bool parse(plist::Dictionary const *dict);
};

} }

#endif  // !__xcscheme_XC_AdditionalOption_h
