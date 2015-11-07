/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <builtin/Registry.h>
#include <builtin/copy/Driver.h>

using builtin::Registry;
using builtin::Driver;

Registry::
Registry(std::unordered_map<std::string, std::shared_ptr<Driver>> const &drivers) :
    _drivers(drivers)
{
}

Registry::
~Registry()
{
}

std::shared_ptr<Driver> Registry::
driver(std::string const &name)
{
    auto const &it = _drivers.find(name);
    if (it != _drivers.end()) {
        return it->second;
    } else {
        return nullptr;
    }
}

Registry Registry::
Create(std::vector<std::shared_ptr<Driver>> const &drivers)
{
    std::unordered_map<std::string, std::shared_ptr<Driver>> driverMap;
    for (std::shared_ptr<Driver> const &driver : drivers) {
        driverMap.insert({ driver->name(), driver });
    }
    return Registry(driverMap);
}

Registry Registry::
Default()
{
    return Create({
        std::make_shared<builtin::copy::Driver>(),
    });
}
