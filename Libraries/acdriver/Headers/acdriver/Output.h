/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef __acdriver_Output_h
#define __acdriver_Output_h

#include <plist/plist.h>
#include <ext/optional>

#include <memory>
#include <string>
#include <vector>

namespace acdriver {

/*
 * Represents the output of a command.
 */
class Output {
public:
    /*
     * Output formats supported.
     */
    enum class Format {
        XML,
        Binary,
        Text,
    };

private:
    std::unique_ptr<plist::Dictionary>           _values;
    std::unordered_map<std::string, std::string> _texts;

public:
    Output();
    ~Output();

public:
    /*
     * Add an output entry.
     */
    void add(
        std::string const &key,
        std::unique_ptr<plist::Object> value,
        std::string const &text);

    /*
     * Output encoded as the specified format.
     */
    ext::optional<std::vector<uint8_t>> serialize(Format format) const;
};

}

#endif // !__acdriver_Output_h
