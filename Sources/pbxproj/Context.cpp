// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxproj/Context.h>
#include <pbxproj/PBX/Project.h>

using pbxproj::Context;

void Context::
cacheObject(PBX::Object::shared_ptr const &O, std::string const &id)
{
    if (project == nullptr && O->isa <PBX::Project> ()) {
        project = std::static_pointer_cast <PBX::Project> (O);
    }

    O->setBlueprintIdentifier(id);

    if (project != nullptr && project != O) {
        project->cacheObject(O);
    }
}
