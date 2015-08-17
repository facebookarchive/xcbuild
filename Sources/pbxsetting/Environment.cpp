// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxsetting/Environment.h>

using pbxsetting::Environment;
using pbxsetting::Level;
using pbxsetting::Condition;
using pbxsetting::Setting;
using pbxsetting::Value;

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
                    result += ResolveAssignment(environment, condition, resolved);
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

