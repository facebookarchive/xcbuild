// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __xcscheme_XC_TestableReference_h
#define __xcscheme_XC_TestableReference_h

#include <xcscheme/XC/BuildableReference.h>
#include <xcscheme/XC/Test.h>

namespace xcscheme { namespace XC {

class TestableReference {
public:
    typedef std::shared_ptr <TestableReference> shared_ptr;
    typedef std::vector <shared_ptr> vector;

private:
    bool                           _skipped;
    BuildableReference::shared_ptr _buildableReference;
    Test::vector                   _skippedTests;

public:
    TestableReference();

public:
    inline bool skipped() const
    { return _skipped; }

public:
    inline BuildableReference::shared_ptr const &buildableReference() const
    { return _buildableReference; }
    inline BuildableReference::shared_ptr &buildableReference()
    { return _buildableReference; }

public:
    inline Test::vector const &skippedTests() const
    { return _skippedTests; }
    inline Test::vector &skippedTests()
    { return _skippedTests; }

public:
    bool parse(plist::Dictionary const *dict);
};

} }

#endif  // !__xcscheme_XC_TestableReference_h
