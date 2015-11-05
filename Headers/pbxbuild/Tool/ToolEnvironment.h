/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_ToolEnvironment_h
#define __pbxbuild_ToolEnvironment_h

#include <pbxbuild/Base.h>

namespace pbxbuild {
namespace Tool {

class ToolEnvironment {
private:
    pbxspec::PBX::Tool::shared_ptr _tool;
    pbxsetting::Environment        _toolEnvironment;
    std::vector<std::string>       _inputs;
    std::vector<std::string>       _outputs;

public:
    ToolEnvironment(pbxspec::PBX::Tool::shared_ptr const &tool, pbxsetting::Environment const &toolEnvironment, std::vector<std::string> const &inputs, std::vector<std::string> const &outputs);
    ~ToolEnvironment();

public:
    pbxspec::PBX::Tool::shared_ptr const &tool() const
    { return _tool; }
    pbxsetting::Environment const &toolEnvironment() const
    { return _toolEnvironment; }
    std::vector<std::string> const &inputs() const
    { return _inputs; }
    std::vector<std::string> const &outputs() const
    { return _outputs; }

public:
    static ToolEnvironment
    Create(pbxspec::PBX::Tool::shared_ptr const &tool, pbxsetting::Environment const &environment, std::vector<std::string> const &inputs, std::vector<std::string> const &outputs);
};

}
}

#endif // !__pbxbuild_ToolEnvironment_h
