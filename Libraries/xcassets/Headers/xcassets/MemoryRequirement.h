/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef __xcassets_MemoryRequirement_h
#define __xcassets_MemoryRequirement_h

#include <ext/optional>
#include <string>

namespace xcassets {

/*
 * The minimum required amount of memory.
 */
enum class MemoryRequirement {
    Minimum1GB,
    Minimum2GB,
    Minimum4GB,
};

class MemoryRequirements {
private:
    MemoryRequirements();
    ~MemoryRequirements();

public:
    /*
     * Parse a matching memory requirement from a string, if valid.
     */
    static ext::optional<MemoryRequirement> Parse(std::string const &value);

    /*
     * Convert an memory requirement to a string.
     */
    static std::string String(MemoryRequirement memoryRequirement);
};

}

#endif // !__xcassets_MemoryRequirement_h
