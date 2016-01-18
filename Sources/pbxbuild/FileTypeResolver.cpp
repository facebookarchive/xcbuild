/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/FileTypeResolver.h>
#include <pbxbuild/DirectedGraph.h>
#include <fstream>

using pbxbuild::FileTypeResolver;
using pbxbuild::DirectedGraph;
using libutil::FSUtil;
using libutil::Wildcard;

static std::pair<bool, std::vector<pbxspec::PBX::FileType::shared_ptr>>
SortedFileTypes(std::vector<pbxspec::PBX::FileType::shared_ptr> const &fileTypes)
{
    DirectedGraph<pbxspec::PBX::FileType::shared_ptr> graph;

    for (pbxspec::PBX::FileType::shared_ptr const &fileType : fileTypes) {
        if (fileType->base() != nullptr) {
            graph.insert(fileType->base(), { fileType });
        }
        graph.insert(fileType, { });
    }

    return graph.ordered();
}

pbxspec::PBX::FileType::shared_ptr FileTypeResolver::
Resolve(pbxspec::Manager::shared_ptr const &specManager, std::vector<std::string> const &domains, std::string const &filePath)
{
    bool isReadable = FSUtil::TestForRead(filePath);
    bool isFolder = isReadable && FSUtil::TestForDirectory(filePath);

    std::string fileExtension = FSUtil::GetFileExtension(filePath);
    std::string fileName = FSUtil::GetBaseName(filePath);

    std::ifstream fileHandle;
    std::vector<uint8_t> fileContents;

    // Reverse first so more specific file types are processed first.
    std::vector<pbxspec::PBX::FileType::shared_ptr> const &fileTypes = specManager->fileTypes(domains);
    std::pair<bool, std::vector<pbxspec::PBX::FileType::shared_ptr>> result = SortedFileTypes(fileTypes);
    if (!result.first) {
        fprintf(stderr, "error: cycle creating file type graph\n");
        return nullptr;
    }

    std::vector<pbxspec::PBX::FileType::shared_ptr> const &sortedFileTypes = result.second;
    for (pbxspec::PBX::FileType::shared_ptr const &fileType : sortedFileTypes) {
        if (isReadable && fileType->isFolder() != isFolder) {
            continue;
        }

        bool empty = true;

        if (!fileType->extensions().empty()) {
            empty = false;
            bool matched = false;

            for (std::string const &extension : fileType->extensions()) {
                // TODO(grp): Is this correct? Needed for handling ".S" as ".s", but might be over-broad.
                if (strcasecmp(extension.c_str(), fileExtension.c_str()) == 0) {
                    matched = true;
                }
            }

            if (!matched) {
                continue;
            }
        }


        if (!fileType->prefix().empty()) {
            empty = false;
            bool matched = false;

            for (std::string const &prefix : fileType->prefix()) {
                if (fileName.find(prefix) == 0) {
                    matched = true;
                }
            }

            if (!matched) {
                continue;
            }
        }

        if (!fileType->filenamePatterns().empty()) {
            empty = false;
            bool matched = false;

            for (std::string const &pattern : fileType->filenamePatterns()) {
                if (Wildcard::Match(pattern, fileName)) {
                    matched = true;
                }
            }

            if (!matched) {
                continue;
            }
        }

        if (isReadable && !fileType->permissions().empty()) {
            empty = false;
            bool matched = false;

            std::string const &permissions = fileType->permissions();
            if (permissions == "read") {
                matched = isReadable;
            } else if (permissions == "write") {
                matched = FSUtil::TestForWrite(filePath);
            } else if (permissions == "executable") {
                matched = FSUtil::TestForExecute(filePath);
            } else {
                fprintf(stderr, "warning: unhandled permission %s\n", permissions.c_str());
            }

            if (!matched) {
                continue;
            }
        }

        // TODO(grp): Support TypeCodes. Not very important.

        if (isReadable && !fileType->magicWords().empty()) {
            empty = false;
            bool matched = false;

            for (std::vector<uint8_t> const &magicWord : fileType->magicWords()) {
                if (fileContents.size() < magicWord.size()) {
                    if (!fileHandle.is_open()) {
                        fileHandle.open(fileName, std::ios::in | std::ios::binary);
                        if (!fileHandle.good()) {
                            continue;
                        }
                    }

                    size_t offset = fileContents.size();
                    fileContents.resize(magicWord.size());
                    fileHandle.read((char *)&fileContents[offset], magicWord.size() - offset);
                    if (!fileHandle.good()) {
                        fileContents.resize(offset);
                        continue;
                    }
                }

                if (std::equal(magicWord.begin(), magicWord.end(), fileContents.begin())) {
                    matched = true;
                }
            }

            if (!matched) {
                continue;
            }
        }

        //
        // Matched no checks.
        //
        if (empty) {
            continue;
        }

        //
        // Matched all checks.
        //
        return fileType;
    }

    pbxspec::PBX::FileType::shared_ptr fileType = (isFolder ? specManager->fileType("folder", domains) : specManager->fileType("file", domains));
    return fileType;
}

pbxspec::PBX::FileType::shared_ptr FileTypeResolver::
Resolve(pbxspec::Manager::shared_ptr const &specManager, std::vector<std::string> const &domains, pbxproj::PBX::FileReference::shared_ptr const &fileReference, std::string const &filePath)
{
    if (!fileReference->explicitFileType().empty()) {
        if (pbxspec::PBX::FileType::shared_ptr const &fileType = specManager->fileType(fileReference->explicitFileType(), domains)) {
            return fileType;
        }
    }

    if (!fileReference->lastKnownFileType().empty()) {
        if (pbxspec::PBX::FileType::shared_ptr const &fileType = specManager->fileType(fileReference->lastKnownFileType(), domains)) {
            return fileType;
        }
    }

    return Resolve(specManager, domains, filePath);
}

pbxspec::PBX::FileType::shared_ptr FileTypeResolver::
Resolve(pbxspec::Manager::shared_ptr const &specManager, std::vector<std::string> const &domains, pbxproj::XC::VersionGroup::shared_ptr const &versionGroup, std::string const &filePath)
{
    if (!versionGroup->versionGroupType().empty()) {
        if (pbxspec::PBX::FileType::shared_ptr const &fileType = specManager->fileType(versionGroup->versionGroupType(), domains)) {
            return fileType;
        }
    }

    return Resolve(specManager, domains, filePath);
}
