/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_Action_DefaultFormatter_h
#define __pbxbuild_Action_DefaultFormatter_h

#include <pbxbuild/Action/Formatter.h>

namespace pbxbuild {
namespace Action {

class DefaultFormatter : public Formatter {
private:
    bool _color;

public:
    DefaultFormatter(bool color);
    ~DefaultFormatter();

public:
    virtual std::string begin(BuildContext const &buildContext);
    virtual std::string success(BuildContext const &buildContext);
    virtual std::string failure(BuildContext const &buildContext, std::vector<Tool::Invocation const> const &failingInvocations);

public:
    virtual std::string beginTarget(BuildContext const &buildContext, pbxproj::PBX::Target::shared_ptr const &target);
    virtual std::string finishTarget(BuildContext const &buildContext, pbxproj::PBX::Target::shared_ptr const &target);

public:
    virtual std::string beginCheckDependencies(pbxproj::PBX::Target::shared_ptr const &target);
    virtual std::string finishCheckDependencies(pbxproj::PBX::Target::shared_ptr const &target);

public:
    virtual std::string beginWriteAuxiliaryFiles(pbxproj::PBX::Target::shared_ptr const &target);
    virtual std::string createAuxiliaryDirectory(std::string const &directory);
    virtual std::string writeAuxiliaryFile(std::string const &file);
    virtual std::string setAuxiliaryExecutable(std::string const &file);
    virtual std::string finishWriteAuxiliaryFiles(pbxproj::PBX::Target::shared_ptr const &target);

public:
    virtual std::string beginCreateProductStructure(pbxproj::PBX::Target::shared_ptr const &target);
    virtual std::string finishCreateProductStructure(pbxproj::PBX::Target::shared_ptr const &target);

public:
    virtual std::string beginInvocation(Tool::Invocation const &invocation, std::string const &executable);
    virtual std::string finishInvocation(Tool::Invocation const &invocation, std::string const &executable);

public:
    static std::shared_ptr<DefaultFormatter>
    Create(bool color);
};

}
}

#endif // !__pbxbuild_Action_DefaultFormatter_h
