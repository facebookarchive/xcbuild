// Copyright 2013-present Facebook. All Rights Reserved.

#include <xcscheme/XC/BuildAction.h>

using xcscheme::XC::BuildAction;

BuildAction::BuildAction() :
    _buildImplicitDependencies(false),
    _parallelizeBuildables    (false)
{
}

bool BuildAction::
parse(plist::Dictionary const *dict)
{
    if (!Action::parse(dict))
        return false;

    auto BID = dict->value <plist::Boolean> ("buildImplicitDependencies");
    auto PB  = dict->value <plist::Boolean> ("parallelizeBuildables");

    if (BID != nullptr) {
        _buildImplicitDependencies = BID->value();
    }

    if (PB != nullptr) {
        _parallelizeBuildables = PB->value();
    }

    if (auto BAEs = dict->value <plist::Dictionary> ("BuildActionEntries")) {
        if (auto BAEd = BAEs->value <plist::Dictionary> ("BuildActionEntry")) {
            auto BAE = std::make_shared <BuildActionEntry> ();
            if (!BAE->parse(BAEd))
                return false;

            _buildActionEntries.push_back(BAE);
        } else if (auto BAEa = BAEs->value <plist::Array> ("BuildActionEntry")) {
            for (size_t n = 0; n < BAEa->count(); n++) {
                auto BAEd = BAEa->value <plist::Dictionary> (n);
                if (BAEd == nullptr)
                    continue;

                auto BAE = std::make_shared <BuildActionEntry> ();
                if (!BAE->parse(BAEd))
                    return false;

                _buildActionEntries.push_back(BAE);
            }
        }
    }
    
    if (auto PA = dict->value <plist::Dictionary> ("PreActions")) {
        if (auto EAd = PA->value <plist::Dictionary> ("ExecuteAction")) {
            auto EA = std::make_shared <ExecuteAction> ();
            if (!EA->parse(EAd))
                return false;

            _preActions.push_back(EA);
        } else if (auto EAa = PA->value <plist::Array> ("ExecuteAction")) {
            for (size_t n = 0; n < EAa->count(); n++) {
                auto EAd = EAa->value <plist::Dictionary> (n);
                if (EAd == nullptr)
                    continue;

                auto EA = std::make_shared <ExecuteAction> ();
                if (!EA->parse(EAd))
                    return false;

                _preActions.push_back(EA);
            }
        }
    }

    if (auto PA = dict->value <plist::Dictionary> ("PostActions")) {
        if (auto EAd = PA->value <plist::Dictionary> ("ExecuteAction")) {
            auto EA = std::make_shared <ExecuteAction> ();
            if (!EA->parse(EAd))
                return false;

            _postActions.push_back(EA);
        } else if (auto EAa = PA->value <plist::Array> ("ExecuteAction")) {
            for (size_t n = 0; n < EAa->count(); n++) {
                auto EAd = EAa->value <plist::Dictionary> (n);
                if (EAd == nullptr)
                    continue;

                auto EA = std::make_shared <ExecuteAction> ();
                if (!EA->parse(EAd))
                    return false;

                _postActions.push_back(EA);
            }
        }
    }

    return true;
}
