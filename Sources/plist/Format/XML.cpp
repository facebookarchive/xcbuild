/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <plist/Format/XML.h>

using plist::Format::Type;
using plist::Format::Encoding;
using plist::Format::Format;
using plist::Format::XML;
using plist::Object;

XML::
XML(Encoding encoding) :
    _encoding(encoding)
{
}

Type XML::
Type()
{
    return Type::XML;
}

template<>
std::unique_ptr<XML> Format<XML>::
Identify(std::vector<uint8_t> const &contents)
{
    /*
     * To identify XML document, we look for a <? or <!, ignoring
     * any whitespace or UTF BOM characters.
     */

    uint8_t last = '\0';

    for (std::vector<uint8_t>::const_iterator bp = contents.begin(); bp != contents.end(); ++bp) {
        /* Conceal zeroes for UTF-16/32 encodings. */
        if (*bp == 0 || isspace(*bp)) {
            bp++;
        } else if (last == 0 && *bp == '<') {
            last = *bp++;
        } else if (last == '<') {
            if (*bp == '?' || *bp == '!') {
                /* Found <? or <! */
            }

            Encoding encoding = Encodings::Detect(contents);
            return std::make_unique<XML>(XML::Create(encoding));
        } else if (bp - contents.begin() < 4) {
            /*
             * We conceal some BOM chars for UTF encodings in the first
             * four bytes.
             */
            switch (*bp) {
                case 0xfe: /* UTF-16/32 */
                case 0xff:
                case 0xef: /* UTF-8 */
                case 0xbb:
                case 0xbf:
                    bp++;
                    break;
                default:
                    return nullptr;
            }
        } else {
            return nullptr;
        }

        last = *bp;
    }

    return nullptr;
}

template<>
std::pair<Object *, std::string> Format<XML>::
Deserialize(std::vector<uint8_t> const &contents, XML const &format)
{
    return std::make_pair(nullptr, "not yet implemented");
}

template<>
std::pair<std::unique_ptr<std::vector<uint8_t>>, std::string> Format<XML>::
Serialize(Object *object, XML const &format)
{
    return std::make_pair(nullptr, "not yet implemented");
}

XML XML::
Create(Encoding encoding)
{
    return XML(encoding);
}
