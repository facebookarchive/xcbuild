// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __xcscheme_XC_ActionContent_h
#define __xcscheme_XC_ActionContent_h

#include <xcscheme/XC/BuildableReference.h>

namespace xcscheme { namespace XC {

class ActionContent {
public:
    typedef std::shared_ptr <ActionContent> shared_ptr;

private:
    std::string                    _title;
    std::string                    _scriptText;
    BuildableReference::shared_ptr _environmentBuildable;

public:
    ActionContent();

public:
    inline std::string const &title() const
    { return _title; }
    
public:
    inline std::string const &scriptText() const
    { return _scriptText; }

public:
    inline BuildableReference::shared_ptr const &environmentBuildable() const
    { return _environmentBuildable; }
    inline BuildableReference::shared_ptr &environmentBuildable()
    { return _environmentBuildable; }

public:
    bool parse(plist::Dictionary const *dict);
};

} }

#endif  // !__xcscheme_XC_ActionContent_h
