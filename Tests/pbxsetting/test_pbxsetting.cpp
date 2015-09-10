
#include <gtest/gtest.h>
#include <pbxsetting/Condition.h>
#include <pbxsetting/Setting.h>
#include <pbxsetting/Value.h>
#include <pbxsetting/Level.h>
#include <pbxsetting/Environment.h>

using namespace pbxsetting;

TEST(pbxsetting, ConditionCreate)
{
    Condition cond = Condition(std::unordered_map<std::string, std::string>({ { "arch", "*" } }));
    ASSERT_EQ(cond.values().size(), 1);
    ASSERT_NE(cond.values().find("arch"), cond.values().end());
    EXPECT_EQ(cond.values().find("arch")->second, "*");

    Condition multiple = Condition(std::unordered_map<std::string, std::string>({{ "arch", "*" }, { "sdk", "*" }}));
    ASSERT_EQ(multiple.values().size(), 2);
    ASSERT_NE(multiple.values().find("arch"), multiple.values().end());
    EXPECT_EQ(multiple.values().find("arch")->second, "*");
    ASSERT_NE(multiple.values().find("sdk"), multiple.values().end());
    EXPECT_EQ(multiple.values().find("sdk")->second, "*");
}

TEST(pbxsetting, ConditionMatchSingle)
{
    Condition arch_armv7 = Condition(std::unordered_map<std::string, std::string>({ { "arch", "armv7" } }));
    Condition arch_i386 = Condition(std::unordered_map<std::string, std::string>({ { "arch", "i386" } }));
    EXPECT_TRUE(arch_i386.match(arch_i386));
    EXPECT_TRUE(arch_armv7.match(arch_armv7));
    EXPECT_FALSE(arch_armv7.match(arch_i386));
    EXPECT_FALSE(arch_i386.match(arch_armv7));

    Condition arch_any = Condition(std::unordered_map<std::string, std::string>({ { "arch", "*" } }));
    EXPECT_TRUE(arch_any.match(arch_armv7));
    EXPECT_TRUE(arch_any.match(arch_i386));

    Condition arch_some1 = Condition(std::unordered_map<std::string, std::string>({ { "arch", "arm*" } }));
    Condition arch_some2 = Condition(std::unordered_map<std::string, std::string>({ { "arch", "*m*" } }));
    Condition arch_some3 = Condition(std::unordered_map<std::string, std::string>({ { "arch", "arm*" } }));
    Condition arch_some4 = Condition(std::unordered_map<std::string, std::string>({ { "arch", "arm*" } }));
    EXPECT_TRUE(arch_some1.match(arch_armv7));
    EXPECT_TRUE(arch_some2.match(arch_armv7));
    EXPECT_TRUE(arch_some3.match(arch_armv7));
    EXPECT_TRUE(arch_some4.match(arch_armv7));
    EXPECT_FALSE(arch_some1.match(arch_i386));
    EXPECT_FALSE(arch_some2.match(arch_i386));
    EXPECT_FALSE(arch_some3.match(arch_i386));
    EXPECT_FALSE(arch_some4.match(arch_i386));
}

TEST(pbxsetting, ConditionMatchMultiple)
{
    Condition arch = Condition(std::unordered_map<std::string, std::string>({ { "arch", "armv7" } }));
    Condition arch_sdk = Condition(std::unordered_map<std::string, std::string>({ { "arch", "armv7" }, { "sdk", "macosx" } }));
    EXPECT_TRUE(arch.match(arch_sdk));
    EXPECT_FALSE(arch_sdk.match(arch));
}

TEST(pbxsetting, ValueSimple)
{
    Value explicit_empty1 = Value::Empty();
    ASSERT_EQ(explicit_empty1.entries().size(), 0);

    Value explicit_empty2 = Value::String("");
    ASSERT_EQ(explicit_empty2.entries().size(), 0);

    Value explicit_string = Value::String("test");
    ASSERT_EQ(explicit_string.entries().size(), 1);
    ASSERT_EQ(explicit_string.entries().at(0).type, Value::Entry::String);
    EXPECT_EQ(explicit_string.entries().at(0).string, "test");

    Value explicit_variable = Value::Variable("VARIABLE");
    EXPECT_EQ(explicit_variable.entries().size(), 1);
    ASSERT_EQ(explicit_variable.entries().at(0).type, Value::Entry::Value);
    ASSERT_EQ(explicit_variable.entries().at(0).value->entries().size(), 1);
    ASSERT_EQ(explicit_variable.entries().at(0).value->entries().at(0).type, Value::Entry::String);
    EXPECT_EQ(explicit_variable.entries().at(0).value->entries().at(0).string, "VARIABLE");

    Value empty = Value::Parse("");
    ASSERT_EQ(empty.entries().size(), 0);

    Value basic = Value::Parse("value");
    ASSERT_EQ(basic.entries().size(), 1);
    ASSERT_EQ(basic.entries().at(0).type, Value::Entry::String);
    EXPECT_EQ(basic.entries().at(0).string, "value");

    Value variable1 = Value::Parse("$(VARIABLE)");
    EXPECT_EQ(variable1.entries().size(), 1);
    ASSERT_EQ(variable1.entries().at(0).type, Value::Entry::Value);
    ASSERT_EQ(variable1.entries().at(0).value->entries().size(), 1);
    ASSERT_EQ(variable1.entries().at(0).value->entries().at(0).type, Value::Entry::String);
    EXPECT_EQ(variable1.entries().at(0).value->entries().at(0).string, "VARIABLE");

    Value variable2 = Value::Parse("${VARIABLE}");
    ASSERT_EQ(variable2.entries().size(), 1);
    ASSERT_EQ(variable2.entries().at(0).type, Value::Entry::Value);
    ASSERT_EQ(variable2.entries().at(0).value->entries().size(), 1);
    ASSERT_EQ(variable2.entries().at(0).value->entries().at(0).type, Value::Entry::String);
    EXPECT_EQ(variable2.entries().at(0).value->entries().at(0).string, "VARIABLE");

    Value variable3 = Value::Parse("$VARIABLE");
    ASSERT_EQ(variable3.entries().size(), 1);
    ASSERT_EQ(variable3.entries().at(0).type, Value::Entry::Value);
    ASSERT_EQ(variable3.entries().at(0).value->entries().size(), 1);
    ASSERT_EQ(variable3.entries().at(0).value->entries().at(0).type, Value::Entry::String);
    EXPECT_EQ(variable3.entries().at(0).value->entries().at(0).string, "VARIABLE");

    Value nested1 = Value::Parse("$($(NAME))");
    ASSERT_EQ(nested1.entries().size(), 1);
    ASSERT_EQ(nested1.entries().at(0).type, Value::Entry::Value);
    ASSERT_EQ(nested1.entries().at(0).value->entries().size(), 1);
    ASSERT_EQ(nested1.entries().at(0).value->entries().at(0).type, Value::Entry::Value);
    EXPECT_EQ(nested1.entries().at(0).value->entries().at(0).value->entries().size(), 1);
    ASSERT_EQ(nested1.entries().at(0).value->entries().at(0).value->entries().at(0).type, Value::Entry::String);
    ASSERT_EQ(nested1.entries().at(0).value->entries().at(0).value->entries().at(0).string, "NAME");

    Value partial1 = Value::Parse("$(open");
    ASSERT_EQ(partial1.entries().size(), 1);
    ASSERT_EQ(partial1.entries().at(0).type, Value::Entry::String);
    EXPECT_EQ(partial1.entries().at(0).string, "$(open");

    Value partial2 = Value::Parse("$((open)");
    ASSERT_EQ(partial2.entries().size(), 1);
    ASSERT_EQ(partial2.entries().at(0).type, Value::Entry::Value);
    ASSERT_EQ(partial2.entries().at(0).value->entries().size(), 1);
    ASSERT_EQ(partial2.entries().at(0).value->entries().at(0).type, Value::Entry::String);
    EXPECT_EQ(partial2.entries().at(0).value->entries().at(0).string, "(open");

    Value partial3 = Value::Parse("$");
    ASSERT_EQ(partial3.entries().size(), 1);
    ASSERT_EQ(partial3.entries().at(0).type, Value::Entry::String);
    EXPECT_EQ(partial3.entries().at(0).string, "$");

    Value partial4 = Value::Parse("$$");
    ASSERT_EQ(partial4.entries().size(), 1);
    ASSERT_EQ(partial4.entries().at(0).type, Value::Entry::String);
    EXPECT_EQ(partial4.entries().at(0).string, "$$");
}

TEST(pbxsetting, ValueComplex)
{
    Value mixed1 = Value::Parse("STRING_$(VARIABLE)");
    ASSERT_EQ(mixed1.entries().size(), 2);
    ASSERT_EQ(mixed1.entries().at(0).type, Value::Entry::String);
    EXPECT_EQ(mixed1.entries().at(0).string, "STRING_");
    ASSERT_EQ(mixed1.entries().at(1).type, Value::Entry::Value);
    ASSERT_EQ(mixed1.entries().at(1).value->entries().size(), 1);
    ASSERT_EQ(mixed1.entries().at(1).value->entries().at(0).type, Value::Entry::String);
    EXPECT_EQ(mixed1.entries().at(1).value->entries().at(0).string, "VARIABLE");

    Value mixed2 = Value::Parse("STRING_$(VARIABLE)_STRING");
    ASSERT_EQ(mixed2.entries().size(), 3);
    ASSERT_EQ(mixed2.entries().at(0).type, Value::Entry::String);
    EXPECT_EQ(mixed2.entries().at(0).string, "STRING_");
    ASSERT_EQ(mixed2.entries().at(1).type, Value::Entry::Value);
    ASSERT_EQ(mixed2.entries().at(1).value->entries().size(), 1);
    ASSERT_EQ(mixed2.entries().at(1).value->entries().at(0).type, Value::Entry::String);
    EXPECT_EQ(mixed2.entries().at(1).value->entries().at(0).string, "VARIABLE");
    ASSERT_EQ(mixed2.entries().at(2).type, Value::Entry::String);
    EXPECT_EQ(mixed2.entries().at(2).string, "_STRING");

    Value mixed3 = Value::Parse("STRING_$(VARIABLE)_STRING_$(VARIABLE)");
    ASSERT_EQ(mixed3.entries().size(), 4);
    ASSERT_EQ(mixed3.entries().at(0).type, Value::Entry::String);
    EXPECT_EQ(mixed3.entries().at(0).string, "STRING_");
    ASSERT_EQ(mixed3.entries().at(1).type, Value::Entry::Value);
    ASSERT_EQ(mixed3.entries().at(1).value->entries().size(), 1);
    ASSERT_EQ(mixed3.entries().at(1).value->entries().at(0).type, Value::Entry::String);
    EXPECT_EQ(mixed3.entries().at(1).value->entries().at(0).string, "VARIABLE");
    ASSERT_EQ(mixed3.entries().at(2).type, Value::Entry::String);
    EXPECT_EQ(mixed3.entries().at(2).string, "_STRING_");
    ASSERT_EQ(mixed3.entries().at(3).type, Value::Entry::Value);
    ASSERT_EQ(mixed3.entries().at(3).value->entries().size(), 1);
    ASSERT_EQ(mixed3.entries().at(3).value->entries().at(0).type, Value::Entry::String);
    EXPECT_EQ(mixed3.entries().at(3).value->entries().at(0).string, "VARIABLE");

    Value nested1 = Value::Parse("ONE_$(TWO_$(THREE))");
    ASSERT_EQ(nested1.entries().size(), 2);
    ASSERT_EQ(nested1.entries().at(0).type, Value::Entry::String);
    EXPECT_EQ(nested1.entries().at(0).string, "ONE_");
    ASSERT_EQ(nested1.entries().at(1).type, Value::Entry::Value);
    ASSERT_EQ(nested1.entries().at(1).value->entries().size(), 2);
    ASSERT_EQ(nested1.entries().at(1).value->entries().at(0).type, Value::Entry::String);
    EXPECT_EQ(nested1.entries().at(1).value->entries().at(0).string, "TWO_");
    ASSERT_EQ(nested1.entries().at(1).value->entries().at(1).type, Value::Entry::Value);
    EXPECT_EQ(nested1.entries().at(1).value->entries().at(1).value->entries().size(), 1);
    ASSERT_EQ(nested1.entries().at(1).value->entries().at(1).value->entries().at(0).type, Value::Entry::String);
    ASSERT_EQ(nested1.entries().at(1).value->entries().at(1).value->entries().at(0).string, "THREE");

    Value nested2 = Value::Parse("ONE_$(TWO_${THREE}_FOUR_$(FIVE)_SIX)_SEVEN");
    ASSERT_EQ(nested2.entries().size(), 3);
    ASSERT_EQ(nested2.entries().at(0).type, Value::Entry::String);
    EXPECT_EQ(nested2.entries().at(0).string, "ONE_");
    ASSERT_EQ(nested2.entries().at(1).type, Value::Entry::Value);
    ASSERT_EQ(nested2.entries().at(1).value->entries().size(), 5);
    ASSERT_EQ(nested2.entries().at(1).value->entries().at(0).type, Value::Entry::String);
    EXPECT_EQ(nested2.entries().at(1).value->entries().at(0).string, "TWO_");
    ASSERT_EQ(nested2.entries().at(1).value->entries().at(1).type, Value::Entry::Value);
    EXPECT_EQ(nested2.entries().at(1).value->entries().at(1).value->entries().size(), 1);
    ASSERT_EQ(nested2.entries().at(1).value->entries().at(1).value->entries().at(0).type, Value::Entry::String);
    ASSERT_EQ(nested2.entries().at(1).value->entries().at(1).value->entries().at(0).string, "THREE");
    ASSERT_EQ(nested2.entries().at(1).value->entries().at(2).type, Value::Entry::String);
    EXPECT_EQ(nested2.entries().at(1).value->entries().at(2).string, "_FOUR_");
    ASSERT_EQ(nested2.entries().at(1).value->entries().at(3).type, Value::Entry::Value);
    EXPECT_EQ(nested2.entries().at(1).value->entries().at(3).value->entries().size(), 1);
    ASSERT_EQ(nested2.entries().at(1).value->entries().at(3).value->entries().at(0).type, Value::Entry::String);
    ASSERT_EQ(nested2.entries().at(1).value->entries().at(3).value->entries().at(0).string, "FIVE");
    ASSERT_EQ(nested2.entries().at(1).value->entries().at(4).type, Value::Entry::String);
    EXPECT_EQ(nested2.entries().at(1).value->entries().at(4).string, "_SIX");
    ASSERT_EQ(nested2.entries().at(2).type, Value::Entry::String);
    EXPECT_EQ(nested2.entries().at(2).string, "_SEVEN");
}

TEST(pbxsetting, ValueAdd)
{
    Value empty = Value::Empty();
    Value string = Value::String("test");
    Value string2 = Value::String("string");
    Value variable = Value::Variable("VARIABLE");

    Value empty_string = empty + string;
    ASSERT_EQ(empty_string.entries().size(), 1);
    ASSERT_EQ(empty_string.entries().at(0).type, Value::Entry::String);
    EXPECT_EQ(empty_string.entries().at(0).string, "test");

    Value empty_variable = empty + variable;
    EXPECT_EQ(empty_variable.entries().size(), 1);
    ASSERT_EQ(empty_variable.entries().at(0).type, Value::Entry::Value);
    ASSERT_EQ(empty_variable.entries().at(0).value->entries().size(), 1);
    ASSERT_EQ(empty_variable.entries().at(0).value->entries().at(0).type, Value::Entry::String);
    EXPECT_EQ(empty_variable.entries().at(0).value->entries().at(0).string, "VARIABLE");

    Value string_variable = string + variable;
    ASSERT_EQ(string_variable.entries().size(), 2);
    ASSERT_EQ(string_variable.entries().at(0).type, Value::Entry::String);
    EXPECT_EQ(string_variable.entries().at(0).string, "test");
    ASSERT_EQ(string_variable.entries().at(1).type, Value::Entry::Value);
    ASSERT_EQ(string_variable.entries().at(1).value->entries().size(), 1);
    ASSERT_EQ(string_variable.entries().at(1).value->entries().at(0).type, Value::Entry::String);
    EXPECT_EQ(string_variable.entries().at(1).value->entries().at(0).string, "VARIABLE");

    Value string_empty_variable = string + empty + variable;
    ASSERT_EQ(string_empty_variable.entries().size(), 2);
    ASSERT_EQ(string_empty_variable.entries().at(0).type, Value::Entry::String);
    EXPECT_EQ(string_empty_variable.entries().at(0).string, "test");
    ASSERT_EQ(string_empty_variable.entries().at(1).type, Value::Entry::Value);
    ASSERT_EQ(string_empty_variable.entries().at(1).value->entries().size(), 1);
    ASSERT_EQ(string_empty_variable.entries().at(1).value->entries().at(0).type, Value::Entry::String);
    EXPECT_EQ(string_empty_variable.entries().at(1).value->entries().at(0).string, "VARIABLE");

    Value string_string = string + string2;
    ASSERT_EQ(string_string.entries().size(), 1);
    ASSERT_EQ(string_string.entries().at(0).type, Value::Entry::String);
    EXPECT_EQ(string_string.entries().at(0).string, "teststring");
}

TEST(pbxsetting, SettingCreate)
{
    Setting basic = Setting::Parse("OTHER_CFLAGS = -Werror");
    EXPECT_EQ(basic.name(), "OTHER_CFLAGS");
    EXPECT_EQ(basic.value(), Value::Parse("-Werror"));
    EXPECT_EQ(basic.condition().values().size(), 0);

    Setting empty = Setting::Parse("CUSTOM = ");
    EXPECT_EQ(empty.name(), "CUSTOM");
    EXPECT_EQ(empty.value(), Value::Parse(""));
    EXPECT_EQ(empty.condition().values().size(), 0);

    Setting spacing = Setting::Parse("  SETTING   =    ");
    EXPECT_EQ(spacing.name(), "SETTING");
    EXPECT_EQ(spacing.value(), Value::Parse(""));
    EXPECT_EQ(spacing.condition().values().size(), 0);

    Setting var = Setting::Parse("WITH_VAR = $(inherited) $(SETTING)");
    EXPECT_EQ(var.name(), "WITH_VAR");
    EXPECT_EQ(var.value(), Value::Parse("$(inherited) $(SETTING)"));
    EXPECT_EQ(var.condition().values().size(), 0);

    Setting var2 = Setting::Parse("WITH_VAR = ${inherited} ${SETTING}");
    EXPECT_EQ(var2.name(), "WITH_VAR");
    EXPECT_EQ(var2.value(), Value::Parse("${inherited} ${SETTING}"));
    EXPECT_EQ(var2.condition().values().size(), 0);

    Setting nested = Setting::Parse("WITH_VAR = $(VALUE_$(SETTING))");
    EXPECT_EQ(nested.name(), "WITH_VAR");
    EXPECT_EQ(nested.value(), Value::Parse("$(VALUE_$(SETTING))"));
    EXPECT_EQ(nested.condition().values().size(), 0);

    Setting cond = Setting::Parse("CONDITION[arch=*] = value");
    EXPECT_EQ(cond.name(), "CONDITION");
    EXPECT_EQ(cond.value(), Value::Parse("value"));
    ASSERT_EQ(cond.condition().values().size(), 1);
    ASSERT_NE(cond.condition().values().find("arch"), cond.condition().values().end());
    EXPECT_EQ(cond.condition().values().find("arch")->second, "*");

    Setting cond2 = Setting::Parse("CONDITION[arch=*][sdk=*some*] = value");
    EXPECT_EQ(cond2.name(), "CONDITION");
    EXPECT_EQ(cond2.value(), Value::Parse("value"));
    ASSERT_EQ(cond2.condition().values().size(), 2);
    ASSERT_NE(cond2.condition().values().find("arch"), cond2.condition().values().end());
    EXPECT_EQ(cond2.condition().values().find("arch")->second, "*");
    ASSERT_NE(cond2.condition().values().find("sdk"), cond2.condition().values().end());
    EXPECT_EQ(cond2.condition().values().find("sdk")->second, "*some*");

    Setting altcond2 = Setting::Parse("CONDITION[arch=*,sdk=ansdk*] = value");
    EXPECT_EQ(altcond2.name(), "CONDITION");
    EXPECT_EQ(altcond2.value(), Value::Parse("value"));
    ASSERT_EQ(altcond2.condition().values().size(), 2);
    ASSERT_NE(altcond2.condition().values().find("arch"), altcond2.condition().values().end());
    EXPECT_EQ(altcond2.condition().values().find("arch")->second, "*");
    ASSERT_NE(altcond2.condition().values().find("sdk"), altcond2.condition().values().end());
    EXPECT_EQ(altcond2.condition().values().find("sdk")->second, "ansdk*");
}

TEST(pbxsetting, Level)
{
    Level level = Level({
        Setting::Parse("ONE = one"),
        Setting::Parse("TWO = two"),
        Setting::Parse("TWO = 2"),
        Setting::Parse("THREE = three"),
        Setting::Parse("THREE[arch=armv7] = 3"),
    });
    EXPECT_EQ(level.settings().size(), 5);
    EXPECT_EQ(level.get("ONE", Condition::Empty()).first, true);
    EXPECT_EQ(level.get("TWO", Condition::Empty()).first, true);
    EXPECT_EQ(level.get("THREE", Condition::Empty()).first, true);
    EXPECT_EQ(level.get("FOUR", Condition::Empty()).first, false);
    EXPECT_EQ(level.get("TWO", Condition::Empty()).second, Value::Parse("2"));
    EXPECT_EQ(level.get("THREE", Condition::Empty()).second, Value::Parse("three"));
    Condition specific = Condition(std::unordered_map<std::string, std::string>({ { "arch", "armv7" } }));
    EXPECT_EQ(level.get("THREE", specific).second, Value::Parse("3"));
    // TODO(grp): Implement the proper lookup behavior here.
    // Condition unspecific = Condition(std::unordered_map<std::string, std::string>({ { "arch", "*" } }));
    // EXPECT_EQ(level.get("THREE", unspecific).second, Value::Parse("3"));
}

TEST(pbxsetting, EnvironmentLayering)
{
    Environment layered;
    layered.insertBack(Level({
        Setting::Parse("LAYERED = command line, $(LAYERED)"),
    }), false);
    layered.insertBack(Level({
        Setting::Parse("LAYERED = target, $(LAYERED)"),
    }), false);
    layered.insertBack(Level({
        Setting::Parse("LAYERED = project, $(LAYERED)"),
    }), false);
    layered.insertBack(Level({
        Setting::Parse("LAYERED = environment"),
    }), false);
    EXPECT_EQ(layered.resolve("LAYERED"), "command line, target, project, environment");
}

TEST(pbxsetting, EnvironmentStaggered)
{
    Environment staggered;
    staggered.insertBack(Level({
        Setting::Parse("LAYERED = command line, $(LAYERED)"),
    }), false);
    staggered.insertBack(Level({
        Setting::Parse("STAGGERED = $(CAPTION): $(LAYERED)"),
        Setting::Parse("LAYERED = target, $(LAYERED)"),
    }), false);
    staggered.insertBack(Level({
        Setting::Parse("LAYERED = project, $(LAYERED)"),
        Setting::Parse("CAPTION = evaluation order"),
    }), false);
    staggered.insertBack(Level({
        Setting::Parse("LAYERED = environment"),
    }), false);
    EXPECT_EQ(staggered.resolve("STAGGERED"), "evaluation order: command line, target, project, environment");
}

TEST(pbxsetting, EnvironmentStaggeredOverride)
{
    Environment staggered;
    staggered.insertBack(Level({
        Setting::Parse("LAYERED = command line, $(LAYERED)"),
    }), false);
    staggered.insertBack(Level({
        Setting::Parse("STAGGERED = $(CAPTION): $(LAYERED)"),
        Setting::Parse("LAYERED = target, $(LAYERED)"),
        Setting::Parse("CAPTION = order of evaluation"),
    }), false);
    staggered.insertBack(Level({
        Setting::Parse("LAYERED = project, $(LAYERED)"),
        Setting::Parse("CAPTION = evaluation order"),
    }), false);
    staggered.insertBack(Level({
        Setting::Parse("LAYERED = environment"),
    }), false);
    EXPECT_EQ(staggered.resolve("STAGGERED"), "order of evaluation: command line, target, project, environment");
}

TEST(pbxsetting, EnvironmentConcatenation)
{
    Environment concat;
    concat.insertBack(Level({
        Setting::Parse("CURRENT_PROJECT_VERSION_app = 15.3.9"),
        Setting::Parse("CURRENT_PROJECT_VERSION_xctest = 1.0.0"),
        Setting::Parse("CURRENT_PROJECT_VERSION = $(CURRENT_PROJECT_VERSION_$(WRAPPER_EXTENSION))"),
    }), false);
    concat.insertBack(Level({
        Setting::Parse("WRAPPER_EXTENSION = app"),
    }), false);
    EXPECT_EQ(concat.resolve("CURRENT_PROJECT_VERSION"), "15.3.9");
}

TEST(pbxsetting, EnvironmentInherited)
{
    Environment inherited;
    inherited.insertBack(Level({
        Setting::Parse("OTHER_LDFLAGS = $(inherited) -framework Security"),
    }), false);
    inherited.insertBack(Level({
        Setting::Parse("OTHER_LDFLAGS = -ObjC"),
    }), false);
    EXPECT_EQ(inherited.resolve("OTHER_LDFLAGS"), "-ObjC -framework Security");
}

TEST(pbxsetting, EnvironmentOperations)
{
    Environment environment;
    environment.insertBack(Level({
        Setting::Parse("IDENTIFIER", "$(COMPLEX:identifier)"),
        Setting::Parse("C99IDENTIFIER", "$(COMPLEX:c99extidentifier)"),
        Setting::Parse("RFC1034IDENTIFIER", "$(COMPLEX:rfc1034identifier)"),
        Setting::Parse("QUOTE", "$(COMPLEX:quote)"),
        Setting::Parse("LOWER", "$(BASIC:lower)"),
        Setting::Parse("UPPER", "$(BASIC:upper)"),
        Setting::Parse("BASE", "$(PATH:base)"),
        Setting::Parse("DIR", "$(PATH:dir)"),
        Setting::Parse("FILE", "$(PATH:file)"),
        Setting::Parse("SUFFIX", "$(PATH:suffix)"),
        Setting::Parse("MULTIPLE", "$(COMPLEX:identifier:upper)"),
    }), false);
    environment.insertBack(Level({
        Setting::Parse("BASIC", "Hello, world."),
        Setting::Parse("COMPLEX", "-_'hello%."),
        Setting::Parse("PATH", "/path/to/../file.ext"),
    }), false);
    EXPECT_EQ(environment.resolve("IDENTIFIER"), "___hello__");
    EXPECT_EQ(environment.resolve("C99IDENTIFIER"), "___hello__");
    EXPECT_EQ(environment.resolve("RFC1034IDENTIFIER"), "---hello--");
    EXPECT_EQ(environment.resolve("QUOTE"), "'-_'\"'\"'hello%.'");
    EXPECT_EQ(environment.resolve("LOWER"), "hello, world.");
    EXPECT_EQ(environment.resolve("UPPER"), "HELLO, WORLD.");
    EXPECT_EQ(environment.resolve("BASE"), "file");
    EXPECT_EQ(environment.resolve("DIR"), "/path/to/..");
    EXPECT_EQ(environment.resolve("FILE"), "file.ext");
    EXPECT_EQ(environment.resolve("SUFFIX"), ".ext");
    EXPECT_EQ(environment.resolve("MULTIPLE"), "___HELLO__");
}

TEST(pbxsetting, EnvironmentValue)
{
    Environment env;
    env.insertBack(Level({
        Setting::Parse("ONE = one"),
        Setting::Parse("TWO = two"),
    }), false);
    EXPECT_EQ(env.expand(Value::Parse("$(ONE)-$(TWO)")), "one-two");
}

TEST(pbxsetting, EnvironmentDefault)
{
    Environment env;
    env.insertBack(Level({
        Setting::Parse("ONE = one"),
        Setting::Parse("TWO = two"),
    }), true);
    env.insertFront(Level({
        Setting::Parse("ONE = 1"),
        Setting::Parse("THREE = three"),
    }), true);
    env.insertBack(Level({
        Setting::Parse("ONE = one1"),
        Setting::Parse("THREE = 3"),
    }), false);
    env.insertFront(Level({
        Setting::Parse("ONE = 1one"),
    }), false);
    EXPECT_EQ(env.resolve("ONE"), "1one");
    EXPECT_EQ(env.resolve("TWO"), "two");
    EXPECT_EQ(env.resolve("THREE"), "3");
}

