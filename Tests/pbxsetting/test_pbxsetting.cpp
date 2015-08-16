
#include <gtest/gtest.h>
#include <pbxsetting/Setting.h>

using namespace pbxsetting;

TEST(pbxsetting, Condition)
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

TEST(pbxsetting, Setting)
{
    Setting basic = Setting::Parse("OTHER_CFLAGS = -Werror");
    EXPECT_EQ(basic.name(), "OTHER_CFLAGS");
    EXPECT_EQ(basic.value(), "-Werror");
    EXPECT_EQ(basic.condition().values().size(), 0);

    Setting empty = Setting::Parse("CUSTOM = ");
    EXPECT_EQ(empty.name(), "CUSTOM");
    EXPECT_EQ(empty.value(), "");
    EXPECT_EQ(empty.condition().values().size(), 0);

    Setting spacing = Setting::Parse("  SETTING   =    ");
    EXPECT_EQ(spacing.name(), "SETTING");
    EXPECT_EQ(spacing.value(), "");
    EXPECT_EQ(spacing.condition().values().size(), 0);

    Setting var = Setting::Parse("WITH_VAR = $(inherited) $(SETTING)");
    EXPECT_EQ(var.name(), "WITH_VAR");
    EXPECT_EQ(var.value(), "$(inherited) $(SETTING)");
    EXPECT_EQ(var.condition().values().size(), 0);

    Setting var2 = Setting::Parse("WITH_VAR = ${inherited} ${SETTING}");
    EXPECT_EQ(var2.name(), "WITH_VAR");
    EXPECT_EQ(var2.value(), "${inherited} ${SETTING}");
    EXPECT_EQ(var2.condition().values().size(), 0);

    Setting nested = Setting::Parse("WITH_VAR = $(VALUE_$(SETTING))");
    EXPECT_EQ(nested.name(), "WITH_VAR");
    EXPECT_EQ(nested.value(), "$(VALUE_$(SETTING))");
    EXPECT_EQ(nested.condition().values().size(), 0);

    Setting cond = Setting::Parse("CONDITION[arch=*] = value");
    EXPECT_EQ(cond.name(), "CONDITION");
    EXPECT_EQ(cond.value(), "value");
    ASSERT_EQ(cond.condition().values().size(), 1);
    ASSERT_NE(cond.condition().values().find("arch"), cond.condition().values().end());
    EXPECT_EQ(cond.condition().values().find("arch")->second, "*");

    Setting cond2 = Setting::Parse("CONDITION[arch=*][sdk=*some*] = value");
    EXPECT_EQ(cond2.name(), "CONDITION");
    EXPECT_EQ(cond2.value(), "value");
    ASSERT_EQ(cond2.condition().values().size(), 2);
    ASSERT_NE(cond2.condition().values().find("arch"), cond2.condition().values().end());
    EXPECT_EQ(cond2.condition().values().find("arch")->second, "*");
    ASSERT_NE(cond2.condition().values().find("sdk"), cond2.condition().values().end());
    EXPECT_EQ(cond2.condition().values().find("sdk")->second, "*some*");

    Setting altcond2 = Setting::Parse("CONDITION[arch=*,sdk=ansdk*] = value");
    EXPECT_EQ(altcond2.name(), "CONDITION");
    EXPECT_EQ(altcond2.value(), "value");
    ASSERT_EQ(altcond2.condition().values().size(), 2);
    ASSERT_NE(altcond2.condition().values().find("arch"), altcond2.condition().values().end());
    EXPECT_EQ(altcond2.condition().values().find("arch")->second, "*");
    ASSERT_NE(altcond2.condition().values().find("sdk"), altcond2.condition().values().end());
    EXPECT_EQ(altcond2.condition().values().find("sdk")->second, "ansdk*");
}

