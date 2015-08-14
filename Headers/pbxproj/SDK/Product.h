// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_SDK_Product_h
#define __pbxproj_SDK_Product_h

#include <pbxproj/Base.h>

namespace pbxproj { namespace SDK {

class Product {
public:
    typedef std::shared_ptr <Product> shared_ptr;

private:
    std::string _productName;
    std::string _productVersion;
    std::string _productUserVisibleVersion;
    std::string _productBuildVersion;
    std::string _productCopyright;

public:
    Product();

public:
    inline std::string const &name() const
    { return _productName; }
    inline std::string const &version() const
    { return _productVersion; }
    inline std::string const &userVisibleVersion() const
    { return _productUserVisibleVersion; }
    inline std::string const &buildVersion() const
    { return _productBuildVersion; }
    inline std::string const &copyright() const
    { return _productCopyright; }

public:
    static Product::shared_ptr Open(std::string const &path);

private:
    bool parse(plist::Dictionary const *dict);
};

} }

#endif  // !__pbxproj_SDK_Product_h
