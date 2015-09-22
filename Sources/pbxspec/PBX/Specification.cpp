// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/PBX/Specification.h>
#include <pbxspec/PBX/BuildPhase.h>
#include <pbxspec/PBX/BuildSystem.h>
#include <pbxspec/PBX/Compiler.h>
#include <pbxspec/PBX/FileType.h>
#include <pbxspec/PBX/Linker.h>
#include <pbxspec/PBX/PackageType.h>
#include <pbxspec/PBX/ProductType.h>
#include <pbxspec/PBX/PropertyConditionFlavor.h>
#include <pbxspec/PBX/Tool.h>
#include <pbxspec/Context.h>
#include <pbxspec/Manager.h>

using pbxspec::PBX::Specification;
using pbxspec::Manager;
using libutil::FSUtil;

Specification::Specification(std::string const &isa) :
    Object             (isa),
    _isGlobalDomainInUI(false)
{
}

bool Specification::
inherit(Specification::shared_ptr const &base)
{
    if (base == nullptr || base.get() == this)
        return false;

    _base               = base;
    _clazz              = base->clazz();
    _isGlobalDomainInUI = base->isGlobalDomainInUI();
    _name               = base->name();
    _description        = base->description();
    _vendor             = base->vendor();
    _version            = base->version();

    return true;
}

bool Specification::
parse(Context *context, plist::Dictionary const *dict)
{
    auto C  = dict->value <plist::String> ("Class");
    auto I  = dict->value <plist::String> ("Identifier");
    auto BO = dict->value <plist::String> ("BasedOn");
    auto DO = dict->value <plist::String> ("Domain");
    auto GD = dict->value <plist::Boolean> ("IsGlobalDomainInUI");
    auto N  = dict->value <plist::String> ("Name");
    auto D  = dict->value <plist::String> ("Description");
    auto V1 = dict->value <plist::String> ("Vendor");
    auto V2 = dict->value <plist::String> ("Version");

    if (C != nullptr) {
        _clazz = C->value();
    }

    if (DO != nullptr) {
        _domain = DO->value();
    } else {
        _domain = context->domain;
    }

    if (I != nullptr) {
        _identifier = I->value();
    }

    if (BO != nullptr) {
        auto basedOn           = BO->value();

        auto basedOnDomain     = Manager::AnyDomain();
        auto basedOnIdentifier = basedOn;

        auto colon = basedOn.find(':');
        if (colon != std::string::npos) {
            basedOnDomain     = basedOn.substr(0, colon);
            basedOnIdentifier = basedOn.substr(colon + 1);
        }

        auto base = context->manager->specification(type(), basedOnIdentifier, { basedOnDomain });
        if (base == nullptr) {
            fprintf(stderr, "error: cannot find base %s specification '%s:%s'\n",
                    type(), basedOnDomain.c_str(), basedOnIdentifier.c_str());
            return false;
        }

        //
        // Inherit all the values.
        //
        if (!inherit(base))
            return false;
    }

    if (GD != nullptr) {
        _isGlobalDomainInUI = GD->value();
    }

    if (N != nullptr) {
        _name = N->value();
    }

    if (D != nullptr) {
        _description = D->value();
    }

    if (V1 != nullptr) {
        _vendor = V1->value();
    }

    if (V2 != nullptr) {
        _version = V2->value();
    }

    return true;
}

Specification::shared_ptr Specification::
Parse(Context *context, plist::Dictionary const *dict)
{
    auto T = dict->value <plist::String> ("Type");
    if (T == nullptr)
        return nullptr;

    if (T->value() == Architecture::Type())
        return Architecture::Parse(context, dict);
    if (T->value() == BuildPhase::Type())
        return BuildPhase::Parse(context, dict);
    if (T->value() == BuildSystem::Type())
        return BuildSystem::Parse(context, dict);
    if (T->value() == Compiler::Type())
        return Compiler::Parse(context, dict);
    if (T->value() == FileType::Type())
        return FileType::Parse(context, dict);
    if (T->value() == Linker::Type())
        return Linker::Parse(context, dict);
    if (T->value() == PackageType::Type())
        return PackageType::Parse(context, dict);
    if (T->value() == ProductType::Type())
        return ProductType::Parse(context, dict);
    if (T->value() == PropertyConditionFlavor::Type())
        return PropertyConditionFlavor::Parse(context, dict);
    if (T->value() == Tool::Type())
        return Tool::Parse(context, dict);

    fprintf(stderr, "error: specification type '%s' not supported\n",
            T->value().c_str());

    return nullptr;
}

bool Specification::
Open(Context *context, std::string const &filename)
{
    if (filename.empty()) {
        errno = EINVAL;
        return false;
    }

    std::string realPath = FSUtil::ResolvePath(filename);
    if (realPath.empty()) {
        return false;
    }

    //
    // Parse property list
    //
    plist::Object *plist = plist::Object::Parse(filename);
    if (plist == nullptr) {
        return false;
    }

#if 0
    plist->dump(stdout);
#endif

    //
    // If this is a dictionary, then it's a single specification,
    // if it's an array then multiple specifications are present.
    //
    if (auto dict = plist::CastTo <plist::Dictionary> (plist)) {
        auto spec = Parse(context, dict);
        if (spec != nullptr) {
            //
            // Add specification to manager.
            //
            context->manager->addSpecification(spec);
        }

        plist->release();
        return (spec != nullptr);
    } else if (auto array = plist::CastTo <plist::Array> (plist)) {
        size_t errors = 0;
        size_t count  = array->count();
        for (size_t n = 0; n < count; n++) {
            if (auto dict = array->value <plist::Dictionary> (n)) {
                auto spec = Parse(context, dict);
                if (spec == nullptr) {
                    errors++;
                } else {
                    context->manager->addSpecification(spec);
                }
            }
        }

        plist->release();
        return (errors < count);
    }

    fprintf(stderr, "error: specification file '%s' does not contain "
            "a dictionary nor an array", filename.c_str());
    plist->release();
    return false;
}

bool Specification::
isa(std::string const &isa) const
{
    if (Object::isa(isa))
        return true;

    if (auto base = this->base())
        return base->isa(isa);

    return false;
}

