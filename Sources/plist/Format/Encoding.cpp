/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <plist/Format/Encoding.h>

#include <codecvt>

using plist::Format::Encoding;
using plist::Format::Encodings;

Encoding Encodings::
Detect(std::vector<uint8_t> const &contents)
{
    /*
     * Check for a UTF-32 BOM. First as bytes overlap with UTF-16 LE.
     */
    if (contents.size() >= 4) {
        if (contents[0] == 0x00 && contents[1] == 0x00 &&
            contents[2] == 0xFE && contents[3] == 0xFF) {
            return Encoding::UTF32BE;
        }

        if (contents[0] == 0xFF && contents[1] == 0xFE &&
            contents[2] == 0x00 && contents[3] == 0x00) {
            return Encoding::UTF32LE;
        }
    }

    /*
     * Check for a UTF-16 BOM.
     */
    if (contents.size() >= 2) {
        if (contents[0] == 0xFE && contents[1] == 0xFF) {
            return Encoding::UTF16BE;
        }

        if (contents[0] == 0xFF && contents[1] == 0xFE) {
            return Encoding::UTF16LE;
        }
    }

    /* Any other encoding is assumed to be UTF-8. */
    return Encoding::UTF8;
}

std::vector<uint8_t> Encodings::
Convert(std::vector<uint8_t> const &contents, Encoding from, Encoding to)
{
    /* No conversion needed. */
    if (from == to) {
        return contents;
    }

    std::vector<uint8_t> result;

    if (from != Encoding::UTF8) {
        if (from == Encoding::UTF32LE || from == Encoding::UTF32BE) {
            // TODO(grp): Convert from UTF-32 to UTF-8.
        } else if (from == Encoding::UTF16LE || from == Encoding::UTF16BE) {
            // TODO(grp): Convert from UTF-16 to UTF-8.
        }
    }

    if (to != Encoding::UTF8) {
        if (to == Encoding::UTF32LE || to == Encoding::UTF32BE) {
            // TODO(grp): Convert from UTF-8 to UTF-32.
        } else if (to == Encoding::UTF16LE || to == Encoding::UTF16BE) {
            // TODO(grp): Convert from UTF-8 to UTF-16.
        }
    }

    return result;
}
