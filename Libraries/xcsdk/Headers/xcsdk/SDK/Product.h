/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __xcsdk_SDK_Product_h
#define __xcsdk_SDK_Product_h

#include <xcsdk/Base.h>

namespace libutil { class Filesystem; };

namespace xcsdk { namespace SDK {

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
    static Product::shared_ptr Open(libutil::Filesystem const *filesystem, std::string const &path);

private:
    bool parse(plist::Dictionary const *dict);
};

} }

#endif  // !__xcsdk_SDK_Product_h
