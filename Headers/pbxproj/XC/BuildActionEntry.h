// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_XC_BuildActionEntry_h
#define __pbxproj_XC_BuildActionEntry_h

#include <pbxproj/XC/BuildableReference.h>

namespace pbxproj { namespace XC {

class BuildActionEntry {
public:
    typedef std::shared_ptr <BuildActionEntry> shared_ptr;
    typedef std::vector <shared_ptr> vector;

private:
    bool                           _buildForAnalyzing;
    bool                           _buildForArchiving;
    bool                           _buildForProfiling;
    bool                           _buildForRunning;
    bool                           _buildForTesting;
    BuildableReference::shared_ptr _buildableReference;

public:
    BuildActionEntry();

public:
    inline bool buildForAnalyzing() const
    { return _buildForAnalyzing; }

    inline bool buildForArchiving() const
    { return _buildForArchiving; }

    inline bool buildForProfiling() const
    { return _buildForProfiling; }

    inline bool buildForRunning() const
    { return _buildForRunning; }

    inline bool buildForTesting() const
    { return _buildForTesting; }

public:
    inline BuildableReference::shared_ptr const &buildableReference() const
    { return _buildableReference; }
    inline BuildableReference::shared_ptr &buildableReference()
    { return _buildableReference; }

public:
    bool parse(plist::Dictionary const *dict);
};

} }

#endif  // !__pbxproj_XC_BuildActionEntry_h
