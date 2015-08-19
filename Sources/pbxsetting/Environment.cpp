// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxsetting/Environment.h>

using pbxsetting::Environment;
using pbxsetting::Level;
using pbxsetting::Condition;
using pbxsetting::Setting;
using pbxsetting::Value;
using libutil::FSUtil;

Environment::
Environment(std::vector<Level> const &assignment, std::vector<Level> const &inheritance) :
    _assignment(assignment),
    _inheritance(inheritance)
{
    assert(_assignment.size() == _inheritance.size());
}

Environment::
~Environment()
{
}

static std::string
ProcessOperation(std::string const &value, std::string const &operation)
{
    const std::string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const std::string digits = "0123456789";

    if (operation == "identifier" || operation == "c99extidentifier") {
        // TODO(grp): Support c99extidentifier correctly. Requires Unicode handling.

        const std::string begin = alphabet + "_";
        const std::string subsequent = begin + digits;

        std::string result = value;
        std::string::size_type offset = result.find_first_not_of(begin);
        while (offset != std::string::npos) {
            result[offset] = '_';
            offset = result.find_first_not_of(subsequent, offset);
        }

        return result;
    } else if (operation == "rfc1034identifier") {
        const std::string begin = alphabet;
        const std::string subsequent = alphabet + digits + "-";
        const std::string end = alphabet + digits;

        std::string result = value;
        for (std::string::iterator it = result.begin(), prev = result.end(), next = (it == result.end() ? it : std::next(it)); it != result.end(); prev = it, ++it, next = (it == result.end() ? it : std::next(it))) {
            // Cannot start or end with a dot.
            if (prev == result.end() || next == result.end()) {
                if (*it == '.') {
                    *it = '-';
                }
            }

            // Cannot have digit or hyphen after dot, or hyphen before dot.
            if (prev == result.end() || *prev == '.') {
                if (begin.find(*it) == std::string::npos) {
                    *it = '-';
                }
            } else if (next != result.end() && *next == '.') {
                if (subsequent.find(*it) == std::string::npos) {
                    *it = '-';
                }
            } else {
                if (end.find(*it) == std::string::npos) {
                    *it = '-';
                }
            }
        }

        return result;
    } else if (operation == "quote") {
        // FIXME(grp): This is (probably) valid, but not necessarily compatible. Algorithm from Python's shlex.quote().
        if (value.find_first_not_of(alphabet + digits + "@%_-+=:,./") == std::string::npos) {
            return value;
        } else {
            std::string result = value;
            std::string::size_type offset = 0;
            while ((offset = result.find("'", offset)) != std::string::npos) {
                result.replace(offset, 1, "'\"'\"'");
                offset += 5;
            }
            return "'" + result + "'";
        }
    } else if (operation == "lower") {
        std::string result = value;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    } else if (operation == "upper") {
        std::string result = value;
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        return result;
    } else if (operation == "standardizepath") {
        // FIXME(grp): How should this handle invalid / nonexistent paths?
        return FSUtil::ResolvePath(value);
    } else if (operation == "base") {
        std::string file = FSUtil::GetBaseName(value);
        return file.substr(0, file.rfind('.'));
    } else if (operation == "dir") {
        return FSUtil::GetDirectoryName(value);
    } else if (operation == "file") {
        return FSUtil::GetBaseName(value);
    } else if (operation == "suffix") {
        return "." + FSUtil::GetFileExtension(value);
    } else {
        fprintf(stderr, "warning: unknown build setting operation %s\n", operation.c_str());
        return value;
    }
}

struct InheritanceContext {
    std::string setting;
    std::vector<Level>::const_iterator it;
};

static std::string
ResolveAssignment(Environment const &environment, Condition const &condition, std::string const &setting);

static std::string
ResolveInheritance(Environment const &environment, Condition const &condition, InheritanceContext const &context);

static std::string
ResolveValue(Environment const &environment, Condition const &condition, Value const &value, InheritanceContext const &context);

static std::string
ResolveValue(Environment const &environment, Condition const &condition, Value const &value, InheritanceContext const &context)
{
    std::string result;
    for (auto entry : value.entries()) {
        switch (entry.type) {
            case Value::Entry::String: {
                result += entry.string;
                break;
            }
            case Value::Entry::Value: {
                std::string resolved = ResolveValue(environment, condition, *entry.value, context);
                if (resolved == context.setting || resolved == "inherited") {
                    result += ResolveInheritance(environment, condition, context);
                } else {
                    std::string setting = resolved;

                    std::string::size_type colon = resolved.find(':');
                    if (colon != std::string::npos) {
                        setting = resolved.substr(0, colon);
                    }

                    std::string value = ResolveAssignment(environment, condition, setting);

                    while (colon != std::string::npos) {
                        std::string::size_type next = resolved.find(':', colon + 1);

                        std::string operation = resolved.substr(colon + 1, next == std::string::npos ? next : next - colon - 1);
                        value = ProcessOperation(value, operation);

                        colon = next;
                    }

                    result += value;
                }
                break;
            }
        }
    }
    if (result.empty()) {
        result = ResolveInheritance(environment, condition, context);
    }
    return result;
}

static std::string
ResolveInheritance(Environment const &environment, Condition const &condition, InheritanceContext const &context)
{
    InheritanceContext ctx = context;
    for (++ctx.it; ctx.it != environment.inheritance().end(); ++ctx.it) {
        auto result = ctx.it->get(ctx.setting, condition);
        if (result.first) {
            return ResolveValue(environment, condition, result.second, ctx);
        }
    }

    return "";
}

static std::string
ResolveAssignment(Environment const &environment, Condition const &condition, std::string const &setting)
{
    InheritanceContext context = { .setting = setting, .it = environment.inheritance().begin() };

    for (auto it = environment.assignment().begin(); it != environment.assignment().end(); ++it) {
        auto result = it->get(setting, condition);
        if (result.first) {
            return ResolveValue(environment, condition, result.second, context);
        }
    }

    if (condition.values().empty()) {
        return "";
    } else {
        return ResolveAssignment(environment, Condition::Empty(), setting);
    }
}

std::string Environment::
resolve(std::string const &setting, Condition const &condition) const
{
    return ResolveAssignment(*this, condition, setting);
}

std::string Environment::
resolve(std::string const &setting) const
{
    return resolve(setting, Condition::Empty());
}

std::unordered_map<std::string, std::string> Environment::
computeValues(Condition const &condition) const
{
    std::unordered_map<std::string, std::string> values;

    for (Level const &level : _assignment) {
        for (Setting const &setting : level.settings()) {
            if (values.find(setting.name()) == values.end()) {
                values[setting.name()] = resolve(setting.name(), condition);
            }
        }
    }

    return values;
}

