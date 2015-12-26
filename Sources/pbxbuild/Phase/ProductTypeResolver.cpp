/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Phase/ProductTypeResolver.h>
#include <pbxbuild/Phase/Environment.h>
#include <pbxbuild/Phase/Context.h>
#include <pbxbuild/Tool/InfoPlistResolver.h>
#include <pbxbuild/Tool/TouchResolver.h>
#include <pbxbuild/Tool/ToolResolver.h>

namespace Phase = pbxbuild::Phase;
namespace Tool = pbxbuild::Tool;

Phase::ProductTypeResolver::
ProductTypeResolver(pbxspec::PBX::ProductType::shared_ptr const &productType) :
    _productType(productType)
{
}

Phase::ProductTypeResolver::
~ProductTypeResolver()
{
}

bool Phase::ProductTypeResolver::
resolve(Phase::Environment const &phaseEnvironment, Phase::Context *phaseContext) const
{
    Target::TargetEnvironment const &targetEnvironment = phaseEnvironment.targetEnvironment();
    BuildEnvironment const &buildEnvironment = phaseEnvironment.buildEnvironment();

    pbxsetting::Environment const &environment = targetEnvironment.environment();
    std::string productPath = environment.resolve("TARGET_BUILD_DIR") + "/" + environment.resolve("FULL_PRODUCT_NAME");

    /*
     * Copy and compose the info plist.
     */
    if (_productType->hasInfoPlist()) {
        if (pbxsetting::Type::ParseBoolean(environment.resolve("INFOPLIST_PREPROCESS"))) {
            // TODO(grp): Preprocess Info.plist using configuration from other build settings.
        }

        if (Tool::InfoPlistResolver const *infoPlistResolver = phaseContext->infoPlistResolver(phaseEnvironment)) {
            /* Create the Info.plist. Note that INFOPLIST_FILE is the input, and INFOPLIST_PATH is the output. */
            infoPlistResolver->resolve(&phaseContext->toolContext(), environment, environment.resolve("INFOPLIST_FILE"));
        } else {
            fprintf(stderr, "warning: could not find info plist tool\n");
        }
    }

    /*
     * Collect all existing tool outputs so the following tools can run last.
     */
    std::vector<std::string> outputs;
    for (ToolInvocation const &invocation : phaseContext->toolContext().invocations()) {
        outputs.insert(outputs.end(), invocation.outputs().begin(), invocation.outputs().end());
    }

    /*
     * Validate the product; specific checks are in the validation tool.
     */
    if (pbxsetting::Type::ParseBoolean(environment.resolve("VALIDATE_PRODUCT"))) {
        if (!_productType->validation().validationToolSpec().empty()) {
            std::string const &validationToolIdentifier = _productType->validation().validationToolSpec();
            if (Tool::ToolResolver const *toolResolver = phaseContext->toolResolver(phaseEnvironment, validationToolIdentifier)) {
                // TODO(grp): Run validation tool.
            } else {
                fprintf(stderr, "warning: could not find validation tool %s\n", validationToolIdentifier.c_str());
            }
        }
    }

    /*
     * Touch the final product to note the build's ultimate creation time.
     */
    if (_productType->isWrapper()) {
        if (Tool::TouchResolver const *touchResolver = phaseContext->touchResolver(phaseEnvironment)) {
            touchResolver->resolve(&phaseContext->toolContext(), environment, productPath, outputs);
        } else {
            fprintf(stderr, "warning: could not find touch tool\n");
        }
    }

    /*
     * Register with launch services. This is only relevant for OS X.
     */
    if (_productType->identifier() == "com.apple.product-type.application" && targetEnvironment.sdk()->platform()->name() == "macosx") {
        if (Tool::ToolResolver const *launchServicesResolver = phaseContext->toolResolver(phaseEnvironment, "com.apple.build-tasks.ls-register-url")) {
            // TODO(grp): Register with launch services. Note this needs the same dependencies as touch.
        } else {
            fprintf(stderr, "warning: could not find register tool\n");
        }
    }

    return true;
}
