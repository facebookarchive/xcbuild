// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_XC_LaunchAction_h
#define __pbxproj_XC_LaunchAction_h

#include <pbxproj/XC/Action.h>
#include <pbxproj/XC/BuildableReference.h>
#include <pbxproj/XC/LocationScenarioReference.h>
#include <pbxproj/XC/CommandLineArgument.h>
#include <pbxproj/XC/AdditionalOption.h>

namespace pbxproj { namespace XC {

class LaunchAction : public Action {
public:
    typedef std::shared_ptr <LaunchAction> shared_ptr;

private:
    bool                                  _allowLocationSimulation;
    bool                                  _debugDocumentVersioning;
    bool                                  _ignoresPersistentStateOnLaunch;
    bool                                  _useCustomWorkingDirectory;
    uint32_t                              _launchStyle;
    std::string                           _selectedDebuggerIdentifier;
    std::string                           _selectedLauncherIdentifier;
    BuildableReference::shared_ptr        _buildableProductRunnable;
    LocationScenarioReference::shared_ptr _locationSecenarioReference;
    CommandLineArgument::vector           _commandLineArguments;
    AdditionalOption::vector              _additionalOptions;

public:
    LaunchAction();

public:
    inline bool allowLocationSimulation() const
    { return _allowLocationSimulation; }

public:
    inline bool debugDocumentVersioning() const
    { return _debugDocumentVersioning; }

public:
    inline bool ignoresPersistentStateOnLaunch() const
    { return _ignoresPersistentStateOnLaunch; }

public:
    inline bool useCustomWorkingDirectory() const
    { return _useCustomWorkingDirectory; }

public:
    inline uint32_t launchStyle() const
    { return _launchStyle; }

public:
    inline std::string const &selectedDebuggerIdentifier() const
    { return _selectedDebuggerIdentifier; }

    inline std::string const &selectedLauncherIdentifier() const
    { return _selectedLauncherIdentifier; }

public:
    inline BuildableReference::shared_ptr const &buildableProductRunnable() const
    { return _buildableProductRunnable; }
    inline BuildableReference::shared_ptr &buildableProductRunnable()
    { return _buildableProductRunnable; }

public:
    inline LocationScenarioReference::shared_ptr const &locationScenarioReference() const
    { return _locationSecenarioReference; }
    inline LocationScenarioReference::shared_ptr &locationScenarioReference()
    { return _locationSecenarioReference; }

public:
    inline CommandLineArgument::vector const &commandLineArguments() const
    { return _commandLineArguments; }
    inline CommandLineArgument::vector &commandLineArguments()
    { return _commandLineArguments; }

public:
    inline AdditionalOption::vector const &additionalOptions() const
    { return _additionalOptions; }
    inline AdditionalOption::vector &additionalOptions()
    { return _additionalOptions; }

public:
    bool parse(plist::Dictionary const *dict) override;
};

} }

#endif  // !__pbxproj_XC_LaunchAction_h
