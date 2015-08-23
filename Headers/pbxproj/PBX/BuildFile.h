// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_PBX_BuildFile_h
#define __pbxproj_PBX_BuildFile_h

#include <pbxproj/PBX/FileReference.h>
#include <pbxproj/PBX/ReferenceProxy.h>

namespace pbxproj { namespace PBX {

class BuildFile : public Object {
public:
    typedef std::shared_ptr <BuildFile> shared_ptr;
    typedef std::vector <shared_ptr> vector;

private:
    FileReference::shared_ptr   _fileReference;
    ReferenceProxy::shared_ptr  _referenceProxy;
    plist::Object              *_settings;

public:
    BuildFile();
    ~BuildFile();

public:
    inline FileReference::shared_ptr const &fileReference() const
    { return _fileReference; }
    inline FileReference::shared_ptr &fileReference()
    { return _fileReference; }

public:
    inline ReferenceProxy::shared_ptr const &referenceProxy() const
    { return _referenceProxy; }
    inline ReferenceProxy::shared_ptr &referenceProxy()
    { return _referenceProxy; }

public:
    inline plist::Object const *settings() const
    { return _settings; }
    inline plist::Object *settings()
    { return _settings; }

public:
    bool parse(Context &context, plist::Dictionary const *dict);

public:
    static inline char const *Isa()
    { return ISA::PBXBuildFile; }
};

} }

#endif  // !__pbxproj_PBX_BuildFile_h
