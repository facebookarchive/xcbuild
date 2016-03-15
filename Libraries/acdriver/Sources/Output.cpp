/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <acdriver/Output.h>

using acdriver::Output;

Output::
Output() :
    _values(plist::Dictionary::New())
{
}

Output::
~Output()
{
}

void Output::
add(std::string const &key, std::unique_ptr<plist::Object> value, std::string const &text)
{
    _values->set(key, std::move(value));
    _texts.insert({ key, text });
}

ext::optional<std::vector<uint8_t>> Output::
serialize(Format format) const
{
    switch (format) {
        case Format::XML: {
            auto serialize = plist::Format::XML::Serialize(_values.get(), plist::Format::XML::Create(plist::Format::Encoding::UTF8));
            if (serialize.first == nullptr) {
                return ext::nullopt;
            }
            return *serialize.first;
        }
        case Format::Binary: {
            auto serialize = plist::Format::Binary::Serialize(_values.get(), plist::Format::Binary::Create());
            if (serialize.first == nullptr) {
                return ext::nullopt;
            }
            return *serialize.first;
        }
        case Format::Text: {
            std::string output;
            for (size_t n = 0; n < _values->count(); ++n) {
                std::string key = _values->key(n);
                std::string text = _texts.at(key);

                output += "/* " + key + " */\n";
                output += text;
                output += "\n";
            }
            return std::vector<uint8_t>(output.begin(), output.end());
        }
    }
}

