// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_XC_ProfileAction_h
#define __pbxproj_XC_ProfileAction_h

#include <pbxproj/XC/Action.h>
#include <pbxproj/XC/BuildableReference.h>

namespace pbxproj { namespace XC {

class ProfileAction : public Action {
public:
    typedef std::shared_ptr <ProfileAction> shared_ptr;

private:
    bool                           _debugDocumentVersioning;
    bool                           _shouldUseLaunchSchemeArgsEnv;
    bool                           _useCustomWorkingDirectory;
    std::string                    _savedToolIdentifier;
    BuildableReference::shared_ptr _buildableProductRunnable;
 
public:
    ProfileAction();

public:
    inline bool debugDocumentVersioning() const
    { return _debugDocumentVersioning; }

public:
    inline bool shouldUseLaunchSchemeArgsEnv() const
    { return _shouldUseLaunchSchemeArgsEnv; }

public:
    inline bool useCustomWorkingDirectory() const
    { return _useCustomWorkingDirectory; }

public:
    inline std::string const &savedToolIdentifier() const
    { return _savedToolIdentifier; }

public:
    inline BuildableReference::shared_ptr const &buildableProductRunnable() const
    { return _buildableProductRunnable; }
    inline BuildableReference::shared_ptr &buildableProductRunnable()
    { return _buildableProductRunnable; }

public:
    bool parse(plist::Dictionary const *dict) override;
};

} }

#endif  // !__pbxproj_XC_ProfileAction_h
