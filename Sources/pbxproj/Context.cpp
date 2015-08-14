// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxproj/Context.h>
#include <pbxproj/PBX/Project.h>

using pbxproj::Context;

void Context::
cacheObject(PBX::Object::shared_ptr const &O, std::string const &id)
{
    if (project == nullptr && O->isa <PBX::Project> ()) {
        project = reinterpret_cast <PBX::Project *> (O.get());
    }

    O->setBlueprintIdentifier(id);

    if (project != nullptr && project != O.get()) {
        project->cacheObject(O);
    }
}
