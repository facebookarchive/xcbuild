// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __xcscheme_XC_CommandLineArgument_h
#define __xcscheme_XC_CommandLineArgument_h

#include <xcscheme/Base.h>

namespace xcscheme { namespace XC {

class CommandLineArgument {
public:
    typedef std::shared_ptr <CommandLineArgument> shared_ptr;
    typedef std::vector <shared_ptr> vector;

private:
    bool        _isEnabled;
    std::string _argument;

public:
    CommandLineArgument();

public:
    inline bool isEnabled() const
    { return _isEnabled; }

public:
    inline std::string const &argument() const
    { return _argument; }

public:
    bool parse(plist::Dictionary const *dict);
};

} }

#endif  // !__xcscheme_XC_CommandLineArgument_h
