/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef __xcassets_TemplateRenderingIntent_h
#define __xcassets_TemplateRenderingIntent_h

#include <ext/optional>
#include <string>

namespace xcassets {

/*
 * The way an image is rendered.
 */
enum class TemplateRenderingIntent {
    Original,
    Template,
};

class TemplateRenderingIntents {
private:
    TemplateRenderingIntents();
    ~TemplateRenderingIntents();

public:
    /*
     * Parse a matching template rendering intent from a string, if valid.
     */
    static ext::optional<TemplateRenderingIntent> Parse(std::string const &value);

    /*
     * Convert an template rendering intent to a string.
     */
    static std::string String(TemplateRenderingIntent templateRenderingIntent);
};

}

#endif // !__xcassets_TemplateRenderingIntent_h

