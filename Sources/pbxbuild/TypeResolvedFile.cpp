// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/TypeResolvedFile.h>
#include <fstream>

using pbxbuild::TypeResolvedFile;
using libutil::FSUtil;
using libutil::Wildcard;

TypeResolvedFile::
TypeResolvedFile(std::string const &filePath, pbxspec::PBX::FileType::shared_ptr const &fileType) :
    _filePath(filePath),
    _fileType(fileType)
{
}

TypeResolvedFile::
~TypeResolvedFile()
{
}

std::unique_ptr<TypeResolvedFile> TypeResolvedFile::
Resolve(pbxspec::Manager::shared_ptr const &specManager, std::string const &filePath)
{
    bool isReadable = FSUtil::TestForRead(filePath);
    bool isFolder = isReadable && FSUtil::TestForDirectory(filePath);

    std::string fileExtension = FSUtil::GetFileExtension(filePath);
    std::string fileName = FSUtil::GetBaseName(filePath);

    std::ifstream fileHandle;
    std::vector<uint8_t> fileContents;

    for (pbxspec::PBX::FileType::shared_ptr const &fileType : specManager->fileTypes()) {
        if (isReadable && fileType->isFolder() != isFolder) {
            continue;
        }

        if (!fileType->extensions().empty()) {
            bool matched = false;

            for (std::string const &extension : fileType->extensions()) {
                if (extension == fileExtension) {
                    matched = true;
                }
            }

            if (!matched) {
                continue;
            }
        }


        if (!fileType->prefix().empty()) {
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
            bool matched = false;

            std::string const &permissions = fileType->permissions();
            if (permissions == "read") {
                matched = isReadable;
            } else if (permissions == "write") {
                matched = FSUtil::TestForWrite(filePath);
            } else if (permissions == "execute") {
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
        // Matched all checks.
        //
        return std::make_unique<TypeResolvedFile>(TypeResolvedFile(filePath, fileType));
    }

    pbxspec::PBX::FileType::shared_ptr fileType = (isFolder ? specManager->fileType("folder") : specManager->fileType("file"));
    return std::make_unique<TypeResolvedFile>(TypeResolvedFile(filePath, fileType));
}

std::unique_ptr<TypeResolvedFile> TypeResolvedFile::
Resolve(pbxspec::Manager::shared_ptr const &specManager, pbxproj::PBX::FileReference::shared_ptr const &fileReference, pbxsetting::Environment const &environment)
{
    std::string filePath = environment.expand(fileReference->resolve());

    if (!fileReference->explicitFileType().empty()) {
        if (pbxspec::PBX::FileType::shared_ptr const &fileType = specManager->fileType(fileReference->explicitFileType())) {
            return std::make_unique<TypeResolvedFile>(TypeResolvedFile(filePath, fileType));
        }
    }

    if (!fileReference->lastKnownFileType().empty()) {
        if (pbxspec::PBX::FileType::shared_ptr const &fileType = specManager->fileType(fileReference->lastKnownFileType())) {
            return std::make_unique<TypeResolvedFile>(TypeResolvedFile(filePath, fileType));
        }
    }

    return Resolve(specManager, filePath);
}

