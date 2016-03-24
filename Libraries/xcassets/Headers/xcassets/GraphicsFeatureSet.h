/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef __xcassets_GraphicsFeatureSet_h
#define __xcassets_GraphicsFeatureSet_h

#include <ext/optional>
#include <string>

namespace xcassets {

/*
 * Available graphics features.
 */
enum class GraphicsFeatureSet {
    MetalFamily1Version1,
    MetalFamily1Version2,
    MetalFamily2Version1,
    MetalFamily2Version2,
    MetalFamily3Version1,
};

class GraphicsFeatureSets {
private:
    GraphicsFeatureSets();
    ~GraphicsFeatureSets();

public:
    /*
     * Parse a matching graphics feature set from a string, if valid.
     */
    static ext::optional<GraphicsFeatureSet> Parse(std::string const &value);

    /*
     * Convert an graphics feature set to a string.
     */
    static std::string String(GraphicsFeatureSet graphicsFeatureSet);
};

}

#endif // !__xcassets_GraphicsFeatureSet_h
