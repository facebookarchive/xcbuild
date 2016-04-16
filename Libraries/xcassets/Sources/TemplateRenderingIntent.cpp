/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <xcassets/TemplateRenderingIntent.h>

#include <cstdlib>

using xcassets::TemplateRenderingIntent;
using xcassets::TemplateRenderingIntents;

ext::optional<TemplateRenderingIntent> TemplateRenderingIntents::
Parse(std::string const &value)
{
    if (value == "original") {
        return TemplateRenderingIntent::Original;
    } else if (value == "template") {
        return TemplateRenderingIntent::Template;
    } else {
        fprintf(stderr, "warning: unknown template rendering intent %s\n", value.c_str());
        return ext::nullopt;
    }
}

std::string TemplateRenderingIntents::
String(TemplateRenderingIntent templateRenderingIntent)
{
    switch (templateRenderingIntent) {
        case TemplateRenderingIntent::Original:
            return "original";
        case TemplateRenderingIntent::Template:
            return "template";
    }

    abort();
}
