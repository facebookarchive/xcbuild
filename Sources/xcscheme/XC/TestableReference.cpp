// Copyright 2013-present Facebook. All Rights Reserved.

#include <xcscheme/XC/TestableReference.h>

using xcscheme::XC::TestableReference;

TestableReference::TestableReference() :
    _skipped(false)
{
}

bool TestableReference::
parse(plist::Dictionary const *dict)
{
    auto S   = dict->value <plist::Boolean> ("skipped");
    auto BR  = dict->value <plist::Dictionary> ("BuildableReference");
    auto ST  = dict->value <plist::Dictionary> ("SkippedTests");

    if (S != nullptr) {
        _skipped = S->value();
    }

    if (BR != nullptr) {
        _buildableReference = std::make_shared <BuildableReference> ();
        if (!_buildableReference->parse(BR))
            return false;
    }

    if (ST != nullptr) {
        if (auto Td = ST->value <plist::Dictionary> ("Test")) {
            auto T = std::make_shared <Test> ();
            if (!T->parse(Td))
                return false;

            _skippedTests.push_back(T);
        } else if (auto Ta = ST->value <plist::Array> ("Test")) {
            for (size_t n = 0; n < Ta->count(); n++) {
                auto Td = Ta->value <plist::Dictionary> (n);
                if (Td == nullptr)
                    continue;

                auto T = std::make_shared <Test> ();
                if (!T->parse(Td))
                    return false;

                _skippedTests.push_back(T);
            }
        }
    }

    return true;
}
