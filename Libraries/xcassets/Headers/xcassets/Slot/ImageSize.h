/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __xcassets_Slot_ImageSize_h
#define __xcassets_Slot_ImageSize_h

#include <ext/optional>
#include <vector>
#include <string>

namespace xcassets {
namespace Slot {

/*
 * The density ratio of a target device.
 */
class ImageSize {
private:
    std::vector<double> _value;

private:
    ImageSize(std::vector<double> imageSize);

public:
    /*
     * The represented image size.
     */
    std::vector<double> value() const
    { return _value; }

public:
    /*
     * Parse a matching image scale from a string, if valid.
     */
    static ext::optional<ImageSize> Parse(std::string const &value);

    /*
     * Convert an image size to a string.
     */
    static std::string String(ImageSize imageSize);
};

}
}

#endif // !__xcassets_Slot_ImageSize_h
