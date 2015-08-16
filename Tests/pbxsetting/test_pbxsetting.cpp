
#include <gtest/gtest.h>
#include <pbxsetting/Setting.h>

using namespace pbxsetting;

TEST(pbxsetting, Setting)
{
    Setting basic = Setting::Parse("OTHER_CFLAGS = -Werror");
    EXPECT_EQ(basic.name(), "OTHER_CFLAGS");
    EXPECT_EQ(basic.value(), "-Werror");
}

