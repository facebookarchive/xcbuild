/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxproj_XC_ConfigurationList_h
#define __pbxproj_XC_ConfigurationList_h

#include <pbxproj/XC/BuildConfiguration.h>

namespace pbxproj { namespace XC {

class ConfigurationList : public PBX::Object {
public:
    typedef std::shared_ptr <ConfigurationList> shared_ptr;

private:
    BuildConfiguration::vector _buildConfigurations;
    std::string                _defaultConfigurationName;
    bool                       _defaultConfigurationIsVisible;

public:
    ConfigurationList();

public:
    inline void setDefaultConfigurationName(std::string const &name)
    { _defaultConfigurationName = name; }
    inline std::string const &defaultConfigurationName() const
    { return _defaultConfigurationName; }

public:
    inline void setDefaultConfigurationIsVisible(bool visible)
    { _defaultConfigurationIsVisible = visible; }
    inline bool defaultConfigurationIsVisible() const
    { return _defaultConfigurationIsVisible; }

public:
    inline BuildConfiguration::vector::const_iterator begin() const
    { return _buildConfigurations.begin(); }
    inline BuildConfiguration::vector::const_iterator end() const
    { return _buildConfigurations.end(); }

    inline BuildConfiguration::vector::iterator begin()
    { return _buildConfigurations.begin(); }
    inline BuildConfiguration::vector::iterator end()
    { return _buildConfigurations.end(); }

public:
    bool parse(Context &context, plist::Dictionary const *dict);

public:
    static inline char const *Isa()
    { return ISA::XCConfigurationList; }
};

} }

#endif  // !__pbxproj_XC_ConfigurationList_h
