/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <gtest/gtest.h>
#include <pbxsetting/Setting.h>

using pbxsetting::Setting;
using pbxsetting::Value;

TEST(Setting, Create)
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
