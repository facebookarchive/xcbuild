// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_XC_TestAction_h
#define __pbxproj_XC_TestAction_h

#include <pbxproj/XC/Action.h>
#include <pbxproj/XC/TestableReference.h>

namespace pbxproj { namespace XC {

class TestAction : public Action {
public:
    typedef std::shared_ptr <TestAction> shared_ptr;

private:
    std::string                    _selectedDebuggerIdentifier;
    std::string                    _selectedLauncherIdentifier;
    bool                           _shouldUseLaunchSchemeArgsEnv;
    TestableReference::vector      _testables;
    BuildableReference::shared_ptr _macroExpansion;

public:
    TestAction();

public:
    inline std::string const &selectedDebuggerIdentifier() const
    { return _selectedDebuggerIdentifier; }

    inline std::string const &selectedLauncherIdentifier() const
    { return _selectedLauncherIdentifier; }

public:
    inline bool shouldUseLaunchSchemeArgsEnv() const
    { return _shouldUseLaunchSchemeArgsEnv; }

public:
    inline TestableReference::vector const &testables() const
    { return _testables; }
    inline TestableReference::vector &testables()
    { return _testables; }

public:
    inline BuildableReference::shared_ptr const &macroExpansion() const
    { return _macroExpansion; }
    inline BuildableReference::shared_ptr &macroExpansion()
    { return _macroExpansion; }

public:
    bool parse(plist::Dictionary const *dict) override;
};

} }

#endif  // !__pbxproj_XC_TestAction_h
