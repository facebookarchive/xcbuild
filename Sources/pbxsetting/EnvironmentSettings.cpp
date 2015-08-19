// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxsetting/EnvironmentSettings.h>

// TODO(grp): How portable is this file?
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
extern char **environ;

using pbxsetting::EnvironmentSettings;
using pbxsetting::Level;
using pbxsetting::Setting;

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

Level EnvironmentSettings::
Default(void)
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

    return Level(settings);
}

