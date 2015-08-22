// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxsetting/DefaultSettings.h>

// TODO(grp): How portable is this file?
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
extern char **environ;

using pbxsetting::DefaultSettings;
using pbxsetting::Level;
using pbxsetting::Setting;
using libutil::FSUtil;

static std::map<std::string, std::string>
environmentVariables(void)
{
    std::map<std::string, std::string> environment;

    for (char **current = environ; *current; current++) {
        std::string variable = *current;
        std::string::size_type offset = variable.find('=');

        std::string name = variable.substr(0, offset);
        std::string value = variable.substr(offset + 1);
        environment.insert(std::make_pair(name, value));
    }

    return environment;
}

Level DefaultSettings::
Environment(void)
{
    std::vector<Setting> settings;

    for (std::pair<std::string, std::string> const &variable : environmentVariables()) {
        // TODO(grp): Is this right? Should this be filtered at another level?
        if (variable.first.front() != '_') {
            Setting setting = Setting::Parse(variable.first, variable.second);
            settings.push_back(setting);
        }
    }

    uid_t uid = geteuid();
    settings.push_back(Setting::Parse("UID", std::to_string(uid)));

    struct passwd *passwd = getpwuid(uid);
    if (passwd != nullptr) {
        settings.push_back(Setting::Parse("USER", passwd->pw_name));
    }

    gid_t gid = getegid();
    settings.push_back(Setting::Parse("GID", std::to_string(gid)));

    struct group *group = getgrgid(gid);
    if (group != nullptr) {
        settings.push_back(Setting::Parse("GROUP", group->gr_name));
    }

    settings.push_back(Setting::Parse("USER_APPS_DIR", "$(HOME)/Applications"));
    settings.push_back(Setting::Parse("USER_LIBRARY_DIR", "$(HOME)/Library"));

    // FIXME(grp): This is definitely not portable.
    size_t len = confstr(_CS_DARWIN_USER_CACHE_DIR, NULL, 0);
    char *cache = (char *)malloc(len);
    confstr(_CS_DARWIN_USER_CACHE_DIR, cache, len);
    std::string cache_root = FSUtil::NormalizePath(std::string(cache) + "/com.apple.DeveloperTools/6.4-$(XCODE_PRODUCT_BUILD_VERSION)/Xcode");
    settings.push_back(Setting::Parse("CACHE_ROOT", cache_root));
    free(cache);

    // Seems identical to TEMP_FILE_DIR but with an 'S'.
    settings.push_back(Setting::Parse("TEMP_FILES_DIR", "$(TEMP_DIR)"));

    return Level(settings);
}

Level DefaultSettings::
Internal(void)
{
    std::vector<Setting> settings = {
        Setting::Parse("APPLE_INTERNAL_DEVELOPER_DIR", "$(APPLE_INTERNAL_DIR)/Developer"),
        Setting::Parse("APPLE_INTERNAL_DIR", "/AppleInternal"),
        Setting::Parse("APPLE_INTERNAL_DOCUMENTATION_DIR", "$(APPLE_INTERNAL_DIR)/Documentation"),
        Setting::Parse("APPLE_INTERNAL_LIBRARY_DIR", "$(APPLE_INTERNAL_DIR)/Library"),
        Setting::Parse("APPLE_INTERNAL_TOOLS", "$(APPLE_INTERNAL_DEVELOPER_DIR)/Tools"),
    };

    return Level(settings);
}

Level DefaultSettings::
Local(void)
{
    std::vector<Setting> settings = {
        Setting::Parse("LOCAL_ADMIN_APPS_DIR", "/Applications/Utilities"),
        Setting::Parse("LOCAL_APPS_DIR", "/Applications"),
        Setting::Parse("LOCAL_DEVELOPER_DIR", "/Library/Developer"),
        Setting::Parse("LOCAL_LIBRARY_DIR", "/Library"),
    };

    return Level(settings);
}

Level DefaultSettings::
System(void)
{
    std::vector<Setting> settings = {
        Setting::Parse("SYSTEM_ADMIN_APPS_DIR", "/Applications/Utilities"),
        Setting::Parse("SYSTEM_APPS_DIR", "/Applications"),
        Setting::Parse("SYSTEM_CORE_SERVICES_DIR", "/System/Library/CoreServices"),
        Setting::Parse("SYSTEM_DEMOS_DIR", "/Applications/Extras"),
        Setting::Parse("SYSTEM_DEVELOPER_APPS_DIR", "$(DEVELOPER_APPLICATIONS_DIR)"),
        Setting::Parse("SYSTEM_DEVELOPER_BIN_DIR", "$(DEVELOPER_BIN_DIR)"),
        Setting::Parse("SYSTEM_DEVELOPER_DEMOS_DIR", "$(DEVELOPER_DIR)/Applications/Utilities/Built Examples"),
        Setting::Parse("SYSTEM_DEVELOPER_DIR", "$(DEVELOPER_DIR)"),
        Setting::Parse("SYSTEM_DEVELOPER_DOC_DIR", "$(DEVELOPER_DIR)/ADC Reference Library"),
        Setting::Parse("SYSTEM_DEVELOPER_GRAPHICS_TOOLS_DIR", "$(DEVELOPER_DIR)/Applications/Graphics Tools"),
        Setting::Parse("SYSTEM_DEVELOPER_JAVA_TOOLS_DIR", "$(DEVELOPER_DIR)/Applications/Java Tools"),
        Setting::Parse("SYSTEM_DEVELOPER_PERFORMANCE_TOOLS_DIR", "$(DEVELOPER_DIR)/Applications/Performance Tools"),
        Setting::Parse("SYSTEM_DEVELOPER_RELEASENOTES_DIR", "$(DEVELOPER_DIR)/ADC Reference Library/releasenotes"),
        Setting::Parse("SYSTEM_DEVELOPER_TOOLS", "$(DEVELOPER_TOOLS_DIR)"),
        Setting::Parse("SYSTEM_DEVELOPER_TOOLS_DOC_DIR", "$(DEVELOPER_DIR)/ADC Reference Library/documentation/DeveloperTools"),
        Setting::Parse("SYSTEM_DEVELOPER_TOOLS_RELEASENOTES_DIR", "$(DEVELOPER_DIR)/ADC Reference Library/releasenotes/DeveloperTools"),
        Setting::Parse("SYSTEM_DEVELOPER_USR_DIR", "$(DEVELOPER_USR_DIR)"),
        Setting::Parse("SYSTEM_DEVELOPER_UTILITIES_DIR", "$(DEVELOPER_DIR)/Applications/Utilities"),
        Setting::Parse("SYSTEM_DOCUMENTATION_DIR", "/Library/Documentation"),
        Setting::Parse("SYSTEM_KEXT_INSTALL_PATH", "/System/Library/Extensions"),
        Setting::Parse("SYSTEM_LIBRARY_DIR", "/System/Library"),

        Setting::Parse("OS", "MACOS"),
        Setting::Parse("MAC_OS_X_PRODUCT_BUILD_VERSION", "14E46"),
        Setting::Parse("MAC_OS_X_VERSION_ACTUAL", "101004"),
        Setting::Parse("MAC_OS_X_VERSION_MAJOR", "101000"),
        Setting::Parse("MAC_OS_X_VERSION_MINOR", "1004"),
    };

    return Level(settings);
}

Level DefaultSettings::
Architecture(void)
{
    std::vector<Setting> settings = {
#if defined(__i386__) || defined(__x86_64__)
        Setting::Parse("NATIVE_ARCH_32_BIT", "i386"),
        Setting::Parse("NATIVE_ARCH_64_BIT", "x86_64"),
  #if defined(__x86_64__)
        Setting::Parse("NATIVE_ARCH_ACTUAL", "x86_64"),
  #else
        Setting::Parse("NATIVE_ARCH_ACTUAL", "i386"),
  #endif
#elif defined(__arm__) || defined(__arm64__)
        Setting::Parse("NATIVE_ARCH_32_BIT", "armv7"),
        Setting::Parse("NATIVE_ARCH_64_BIT", "arm64"),
  #if defined(__arm64__)
        Setting::Parse("NATIVE_ARCH_ACTUAL", "arm64"),
  #else
        Setting::Parse("NATIVE_ARCH_ACTUAL", "armv7"),
  #endif
#else
        Setting::Parse("NATIVE_ARCH_32_BIT", "UNKNOWN"),
        Setting::Parse("NATIVE_ARCH_64_BIT", "UNKNOWN"),
        Setting::Parse("NATIVE_ARCH_ACTUAL", "UNKNOWN"),
#endif
    };

    return Level(settings);
}

Level DefaultSettings::
Build(void)
{
    std::vector<Setting> settings = {
        Setting::Parse("XCODE_PRODUCT_BUILD_VERSION", "6E35b"),
        Setting::Parse("XCODE_VERSION_ACTUAL", "0640"),
        Setting::Parse("XCODE_VERSION_MAJOR", "0600"),
        Setting::Parse("XCODE_VERSION_MINOR", "0640"),
        Setting::Parse("XCODE_APP_SUPPORT_DIR", "$(DEVELOPER_LIBRARY_DIR)/Xcode"),
    };

    return Level(settings);
}

std::vector<Level> DefaultSettings::
Levels(void)
{
    return {
        Environment(),
        Internal(),
        Local(),
        System(),
        Architecture(),
        Build(),
    };
}
