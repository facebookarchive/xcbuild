// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxsdk/pbxsdk.h>

#include <cstring>
#include <cerrno>

using namespace pbxsdk;
using namespace libutil;


int
main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage: %s developerpath\n", argv[0]);
        return -1;
    }

    SDK::Platform::vector platforms;

    if (!SDK::Manager::GetPlatforms(argv[1], platforms)) {
        fprintf(stderr, "no SDK platforms found at %s\n", argv[1]);
        return -1;
    }

#if 1
    for (auto platform : platforms) {
        printf("%s SDKs:\n", platform->description().c_str());
        for (auto target : platform->targets()) {
            printf("\t%-32s-sdk %s\n",
                    target->displayName().c_str(),
                    target->canonicalName().c_str());
        }
        printf("\n");
    }
#endif

    for (auto platform : platforms) {
        for (auto target : platform->targets()) {
            printf("%s - %s (%s)\n",
                    target->bundleName().c_str(),
                    target->displayName().c_str(),
                    target->canonicalName().c_str());
            if (!target->version().empty()) {
                printf("SDKVersion: %s\n", target->version().c_str());
            }
            printf("Path: %s\n", target->path().c_str());
            if (!target->platform()->version().empty()) {
                printf("PlatformVersion: %s\n", target->platform()->version().c_str());
            }
            printf("PlatformPath: %s\n", target->platform()->path().c_str());
            if (auto product = target->product()) {
                if (!product->buildVersion().empty()) {
                    printf("ProductBuildVersion: %s\n", product->buildVersion().c_str());
                }
                if (!product->copyright().empty()) {
                    printf("ProductCopyright: %s\n", product->copyright().c_str());
                }
                if (!product->name().empty()) {
                    printf("ProductName: %s\n", product->name().c_str());
                }
                if (!product->userVisibleVersion().empty()) {
                    printf("ProductUserVisibleVersion: %s\n", product->userVisibleVersion().c_str());
                }
                if (!product->version().empty()) {
                    printf("ProductVersion: %s\n", product->version().c_str());
                }
            }
            printf("\n");
        }
    }

    return 0;
}
