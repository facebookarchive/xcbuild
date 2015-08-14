// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/DependencyGenerator.h>
#include <libutil/FSUtil.h>

//
// Let's use the C version to simplify things.
//
#include <clang-c/Index.h>

using pbxbuild::DependencyGenerator;
using libutil::FSUtil;

DependencyGenerator::DependencyGenerator()
{
}

void DependencyGenerator::
clear()
{
    _files.clear();
}

int DependencyGenerator::
scan(std::string const &path,
        std::function <void(std::string const &)> const &callback)
{
    return scan(path, string_vector(), callback);
}

int DependencyGenerator::
scan(std::string const &path, string_vector const &arguments)
{
    return scan(path, arguments, [](std::string const &) {});
}

int DependencyGenerator::
scan(std::string const &path, string_vector const &arguments,
        std::function <void(std::string const &)> const &callback)
{
    if (path.empty())
        return -1;

    string_vector paths;
    paths.push_back(path);

    return scan(paths, arguments, callback);
}

int DependencyGenerator::
scan(string_set const &paths,
        std::function <void(std::string const &)> const &callback)
{
    return scan(paths, string_vector(), callback);
}

int DependencyGenerator::
scan(string_set const &paths, string_vector const &arguments)
{
    return scan(paths, arguments, [](std::string const &) {});
}

int DependencyGenerator::
scan(string_set const &paths, string_vector const &arguments,
        std::function <void(std::string const &)> const &callback)
{
    if (paths.empty())
        return -1;

    string_vector vpaths;
    for (auto path : paths) {
        if (!path.empty()) {
            vpaths.push_back(path);
        }
    }

    return scan(vpaths, arguments, callback);
}

struct VisitorContext {
    pbxbuild::DependencyFile::vector *files;
    std::map <std::string, pbxbuild::DependencyFile::shared_ptr> filesMap;
};

static CXChildVisitResult
ClangCursorVisitor(CXCursor cursor, CXCursor, CXClientData data)
{
    auto context = reinterpret_cast <VisitorContext *> (data);

    if (cursor.kind == CXCursor_InclusionDirective) {
        CXFile file, includedFile;
        
        includedFile = clang_getIncludedFile(cursor);
        auto location = clang_getCursorLocation(cursor);
        clang_getFileLocation(location, &file, nullptr, nullptr, nullptr);

        CXString includedNameCXStr = clang_getFileName(includedFile);

        auto includeName = clang_getCString(includedNameCXStr);
        if (includeName == nullptr) {
            clang_disposeString(includedNameCXStr);

            includedNameCXStr = clang_getCursorDisplayName(cursor);
            includeName = clang_getCString(includedNameCXStr);
        }

        CXString fileNameCXStr = clang_getFileName(file);
        auto fileName = clang_getCString(fileNameCXStr);

        //if (fileName[0] != '/' && includeName[0] != '/') {
            auto I = context->filesMap.find(fileName);
            if (I == context->filesMap.end()) {
                auto dp = std::make_shared <pbxbuild::DependencyFile> (FSUtil::NormalizePath(fileName));
                context->files->push_back(dp);
                I = context->filesMap.insert(std::make_pair(FSUtil::NormalizePath(fileName), dp)).first;
            }
            I->second->add(FSUtil::NormalizePath(includeName));
        //}

        clang_disposeString(fileNameCXStr);
        clang_disposeString(includedNameCXStr);
    }

    return CXChildVisit_Continue;
}

int DependencyGenerator::
scan(string_vector const &paths, string_vector const &arguments)
{
    return scan(paths, arguments, [](std::string const &) {});
}

int DependencyGenerator::
scan(string_vector const &paths,
        std::function <void(std::string const &)> const &callback)
{
    return scan(paths, string_vector(), callback);
}

int DependencyGenerator::
scan(string_vector const &paths, string_vector const &arguments,
        std::function <void(std::string const &)> const &callback)
{
    if (paths.empty())
        return -1;

    size_t empty = 0;
    for (auto const &path : paths) {
        if (path.empty()) {
            empty++;
        }
    }

    if (empty == paths.size())
        return -1;

    std::vector <char const *> args;
    for (auto const &argument : arguments) {
        args.push_back(argument.c_str());
    }

    for (auto const &path : paths) {
        auto thePath = FSUtil::NormalizePath(path);

        callback(thePath);

        unsigned flags = CXTranslationUnit_SkipFunctionBodies |
                         CXTranslationUnit_Incomplete |
                         CXTranslationUnit_DetailedPreprocessingRecord;

        auto idx = clang_createIndex(0, 0);
        auto tu  = clang_parseTranslationUnit(idx, thePath.c_str(),
                &args[0], args.size(), nullptr, 0, flags);

        if (tu != nullptr) {
            VisitorContext context;
            context.files = &_files;

            auto cursor = clang_getTranslationUnitCursor(tu);
            clang_visitChildren(cursor, ClangCursorVisitor,
                    reinterpret_cast <CXClientData> (&context));
            clang_disposeTranslationUnit(tu);
        }

        clang_disposeIndex(idx);
    }

    return 0;
}
