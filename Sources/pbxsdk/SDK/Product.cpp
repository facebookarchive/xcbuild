// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxsdk/SDK/Product.h>

using pbxsdk::SDK::Product;
using libutil::FSUtil;

Product::Product()
{
}

bool Product::
parse(plist::Dictionary const *dict)
{
    auto PN   = dict->value <plist::String> ("ProductName");
    auto PV   = dict->value <plist::String> ("ProductVersion");
    auto PUVV = dict->value <plist::String> ("ProductUserVisibleVersion");
    auto PBV  = dict->value <plist::String> ("ProductBuildVersion");
    auto PC   = dict->value <plist::String> ("ProductCopyright");

    if (PN != nullptr) {
        _productName = PN->value();
    }

    if (PV != nullptr) {
        _productVersion = PV->value();
    }

    if (PUVV != nullptr) {
        _productUserVisibleVersion = PUVV->value();
    }

    if (PBV != nullptr) {
        _productBuildVersion = PBV->value();
    }

    if (PC != nullptr) {
        _productCopyright = PC->value();
    }

    return true;
}

Product::shared_ptr Product::
Open(std::string const &path)
{
    if (path.empty()) {
        errno = EINVAL;
        return nullptr;
    }

    std::string settingsFileName = path + "/System/Library/CoreServices/SystemVersion.plist";
    if (!FSUtil::TestForRead(settingsFileName.c_str()))
        return nullptr;

    std::string realPath = FSUtil::ResolvePath(settingsFileName);
    if (realPath.empty())
        return nullptr;

    //
    // Parse property list
    //
    plist::Dictionary *plist = plist::Dictionary::Parse(settingsFileName);
    if (plist == nullptr)
        return nullptr;

    //
    // Parse the Product dictionary and create the object.
    //
    auto product = std::make_shared <Product> ();

    if (!product->parse(plist)) {
        product = nullptr;
    }

    //
    // Release the property list.
    //
    plist->release();

    return product;
}
