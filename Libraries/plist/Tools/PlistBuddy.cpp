/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <plist/Array.h>
#include <plist/Boolean.h>
#include <plist/Data.h>
#include <plist/Date.h>
#include <plist/Dictionary.h>
#include <plist/Integer.h>
#include <plist/Real.h>
#include <plist/String.h>
#include <plist/Object.h>
#include <plist/Format/Any.h>
#include <plist/Format/ASCII.h>
#include <plist/Format/Binary.h>
#include <plist/Format/Encoding.h>
#include <plist/Format/XML.h>
#include <libutil/Options.h>
#include <libutil/Filesystem.h>
#include <libutil/DefaultFilesystem.h>

#include <iostream>
#include <iterator>
#include <queue>
#include <sstream>
#include <string>

#if defined(HAVE_LINENOISE)
#include <linenoise.h>
#endif

using libutil::Filesystem;
using libutil::DefaultFilesystem;

typedef struct {
    std::unique_ptr<plist::Object> object;
} RootObjectContainer;

class Options {
private:
    ext::optional<bool>         _help;
    ext::optional<bool>         _xml;

private:
    ext::optional<std::string>  _command;

private:
    std::string                 _input;

public:
    Options();
    ~Options();

public:
    bool help() const
    { return _help.value_or(false); }
    bool xml() const
    { return _xml.value_or(false); }

public:
    ext::optional<std::string> command() const
    { return _command; }

public:
    std::string input() const
    { return _input; }

private:
    friend class libutil::Options;
    std::pair<bool, std::string>
    parseArgument(std::vector<std::string> const &args, std::vector<std::string>::const_iterator *it);

};

Options::
Options()
{
}

Options::
~Options()
{
}

std::pair<bool, std::string> Options::
parseArgument(std::vector<std::string> const &args, std::vector<std::string>::const_iterator *it)
{
    std::string const &arg = **it;

    if (arg == "-h") {
        return libutil::Options::Current(&_help, arg);
    } else if (arg == "-x") {
        return libutil::Options::Current(&_xml, arg);
    } else if (arg == "-c") {
        return libutil::Options::Next<std::string>(&_command, args, it);
    } else if (!arg.empty() && arg[0] != '-') {
        _input = arg;
        return std::make_pair(true, std::string());
    } else {
        return std::make_pair(false, "unknown argument " + arg);
    }
}

static plist::ObjectType
ParseType(std::string const &typeString) {
    if (typeString == "string") {
        return plist::ObjectType::String;
    } else if (typeString == "dictionary") {
        return plist::ObjectType::Dictionary;
    } else if (typeString == "array") {
        return plist::ObjectType::Array;
    } else if (typeString == "bool") {
        return plist::ObjectType::Boolean;
    } else if (typeString == "real") {
        return plist::ObjectType::Real;
    } else if (typeString == "integer") {
        return plist::ObjectType::Integer;
    } else if (typeString == "date") {
        return plist::ObjectType::Date;
    } else if (typeString == "data") {
        return plist::ObjectType::Data;
    } else {
        return plist::ObjectType::None;
    }
}

static int
Help(std::string const &error = std::string())
{
    if (!error.empty()) {
        fprintf(stderr, "error: %s\n", error.c_str());
        fprintf(stderr, "\n");
    }

    fprintf(stderr, "usage: PlistBuddy [options] <file.plist>\n");

#define INDENT "  "
    fprintf(stderr, "\noptions:\n");
    fprintf(stderr, INDENT "-c \"<command>\" command to execute, otherwise run in interactive mode\n");
    fprintf(stderr, INDENT "-x output will be in xml plist format\n");
    fprintf(stderr, INDENT "-h print help including commands\n");
#undef INDENT

    return (error.empty() ? 0 : -1);
}

static void
CommandHelp()
{
#define INDENT "  "
    fprintf(stderr, "\nCommands help:\n");
    fprintf(stderr, INDENT "Help - Print this information\n");
    fprintf(stderr, INDENT "Exit - Exits this program\n");
    fprintf(stderr, INDENT "Print [<KeyPath>] - Print value at KeyPath. (default KeyPath = root)\n");
    fprintf(stderr, INDENT "Save - Save the changed plist file\n");
    fprintf(stderr, INDENT "Set <KeyPath> <Value> - Set value at KeyPath to Value\n");
    fprintf(stderr, INDENT "Add <KeyPath> <Type> <Value> - Set value at KeyPath to Value\n");
    fprintf(stderr, INDENT "Clear <Type> - Clears all data, and sets root to of the given type\n");
    fprintf(stderr, INDENT "Delete <KeyPath> - Removes entry at KeyPath\n");
    fprintf(stderr, INDENT "Merge <file> [<KeyPath>] - Merges data from <file> to KeyPath if exists, otherwise to root\n");
    fprintf(stderr, "\n<KeyPath>\n");
    fprintf(stderr, INDENT ":= \"\"                             => root object\n");
    fprintf(stderr, INDENT ":= <KeyPath>[:<Dictionary Key>]   => indexes into dictionary\n");
    fprintf(stderr, INDENT ":= <KeyPath>[:<Array Index>]      => indexes into Array\n");
    fprintf(stderr, "\n<Type> := (string|dictionary|array|bool|real|integer|date|data)\n\n");
#undef INDENT
}

static std::unique_ptr<plist::Object>
ReadPropertyList(Filesystem const *filesystem, std::string const &path)
{
    if (path.empty()) {
        fprintf(stderr, "error: no input specified\n");
        return NULL;
    }

    if (!filesystem->exists(path)) {
        fprintf(stderr, "error: file does not exist: %s\n", path.c_str());
        return NULL;
    }

    std::vector<uint8_t> contents;
    if (!filesystem->read(&contents, path)) {
        fprintf(stderr, "error: unable to read %s\n", path.c_str());
        return NULL;
    }

    auto deserialize = plist::Format::Any::Deserialize(contents);
    if (!deserialize.first) {
        fprintf(stderr, "error: %s\n", deserialize.second.c_str());
        return NULL;
    }

    return std::move(deserialize.first);
}

static void
ParseCommandKeyPathString(std::string const &keyPathString, std::queue<std::string> *keyPath)
{
    std::string::size_type prev = 0;
    do {
        std::string::size_type pos = keyPathString.find(':', prev);
        std::string::size_type end = (pos == std::string::npos ? pos : pos - prev);

        std::string key = keyPathString.substr(prev, end);
        if (!key.empty()) {
            keyPath->push(key);
        }

        prev = (pos == std::string::npos ? pos : pos + 1);
    } while (prev != std::string::npos);
}

static std::string
ParseCommandValueString(std::vector<std::string>::const_iterator begin, std::vector<std::string>::const_iterator end)
{
    if (begin == end) {
        return "";
    }

    std::string value;
    for (auto it = begin; it != end; it++) {
        if (it != begin) {
            value += ' ';
        }
        value += it->c_str();
    }

    /* Strip double quotes if necessary. */
    if (value.front() == '\"' && value.back() == '\"') {
        value = value.substr(1, value.size() - 2);
    }

    return value;
}

static plist::Object *
GetObjectAtKeyPath(plist::Object *object, std::queue<std::string> *remainingKeys, bool leaveLastKey = true)
{
    if (remainingKeys->empty() || (leaveLastKey && remainingKeys->size() == 1)) {
        return object;
    } else if (!object) {
        fprintf(stderr, "Invalid key path (indexing into null object)\n");
        return nullptr;
    }

    std::string currentKey = remainingKeys->front();

    plist::Object *subObject = nullptr;
    switch (object->type()) {
        case plist::ObjectType::Dictionary:
            subObject = plist::CastTo<plist::Dictionary>(object)->value(currentKey);
            break;
        case plist::ObjectType::Array: {
            char *end = NULL;
            long long index = std::strtoll(currentKey.c_str(), &end, 0);
            if (end == currentKey.c_str() || index < 0) {
                fprintf(stderr, "Invalid array index\n");
                return nullptr;
            }

            subObject = plist::CastTo<plist::Array>(object)->value(index);
            break;
        }
        default:
            /* Reached a non-collection object with remaining key path, error. */
            fprintf(stderr, "Invalid key path (indexing into non-collection object)\n");
            return nullptr;
    }

    if (!subObject) {
        return object;
    } else {
        remainingKeys->pop();
        return GetObjectAtKeyPath(subObject, remainingKeys, leaveLastKey);
    }
}

static bool
Print(plist::Object *object, std::queue<std::string> &keyPath, bool xml, Filesystem *filesystem = nullptr, std::string path = "-")
{
    plist::Object *target = GetObjectAtKeyPath(object, &keyPath, false);
    if (!keyPath.empty() || target == nullptr) {
        fprintf(stderr, "Invalid key path (no object at key path)\n");
        return false;
    }

    /*
     * Determine output format.
     */
    ext::optional<plist::Format::Any> out;
    if (xml) {
        plist::Format::XML xml = plist::Format::XML::Create(plist::Format::Encoding::UTF8);
        out = plist::Format::Any::Create<plist::Format::XML>(xml);
    } else {
        plist::Format::ASCII ascii = plist::Format::ASCII::Create(false, plist::Format::Encoding::UTF8);
        out = plist::Format::Any::Create<plist::Format::ASCII>(ascii);
    }

    auto serialize = plist::Format::Any::Serialize(target, *out);
    if (serialize.first == nullptr) {
        fprintf(stderr, "error: %s\n", serialize.second.c_str());
        return false;
    }

    /* Print. */
    if (filesystem != nullptr && path != "-") {
        filesystem->write(*serialize.first, path);
    } else {
        std::copy(serialize.first->begin(), serialize.first->end(), std::ostream_iterator<char>(std::cout));
    }

    return true;
}

static bool
Set(plist::Object *object, std::queue<std::string> &keyPath, plist::ObjectType type, const std::string &valueString, bool overwrite = true)
{
    plist::Object *target = GetObjectAtKeyPath(object, &keyPath, true);
    if (!target || keyPath.size() != 1) {
        fprintf(stderr, "Invalid key path (target object not found)\n");
        return false;
    }

    std::string targetKey = keyPath.front();

    std::unique_ptr<plist::Object> newObj;
    switch (type) {
        case plist::ObjectType::String:
            newObj = plist::String::New(valueString);
            break;
        case plist::ObjectType::Dictionary:
            newObj = plist::Dictionary::New();
            break;
        case plist::ObjectType::Array:
            newObj = plist::Array::New();
            break;
        case plist::ObjectType::Boolean:
            newObj = plist::Boolean::Coerce(new plist::String(valueString));
            break;
        case plist::ObjectType::Real:
            newObj = plist::Real::Coerce(new plist::String(valueString));
            break;
        case plist::ObjectType::Integer:
            newObj = plist::Integer::Coerce(new plist::String(valueString));
            break;
        case plist::ObjectType::Date:
            newObj = plist::Date::New(valueString);
            break;
        case plist::ObjectType::Data:
            newObj = plist::Data::New(valueString);
            break;
        default:
            fprintf(stderr, "Not supported\n");
            return false;
    }

    switch (target->type()) {
        case plist::ObjectType::Dictionary: {
            plist::Dictionary *dict = plist::CastTo<plist::Dictionary>(target);
            if (!overwrite && dict->value(targetKey)) {
                fprintf(stderr, "Cannot overwrite key path\n");
                return false;
            }
            dict->set(targetKey, std::move(newObj));
            return true;
        }
        case plist::ObjectType::Array: {
            char *end = NULL;
            long long index = std::strtoll(targetKey.c_str(), &end, 0);
            if (end == targetKey.c_str() || index < 0) {
                fprintf(stderr, "Invalid array index\n");
                return false;
            }
            plist::Array *array = plist::CastTo<plist::Array>(target);
            if (!overwrite && array->value(index)) {
                fprintf(stderr, "Cannot overwrite key path\n");
                return false;
            }
            array->insert(index, std::move(newObj));
            return true;
        }
        default:
            fprintf(stderr, "Invalid key path (setting value on non-collection object)\n");
            return false;
    }
}

static bool
Clear(RootObjectContainer *root, plist::ObjectType clearType) {
    switch (clearType) {
        case plist::ObjectType::String:
            root->object = plist::String::New();
            break;
        case plist::ObjectType::Dictionary:
            root->object = plist::Dictionary::New();
            break;
        case plist::ObjectType::Array:
            root->object = plist::Array::New();
            break;
        case plist::ObjectType::Boolean:
            root->object = plist::Boolean::New(false);
            break;
        case plist::ObjectType::Real:
            root->object = plist::Real::New();
            break;
        case plist::ObjectType::Integer:
            root->object = plist::Integer::New();
            break;
        case plist::ObjectType::Date:
            root->object = plist::Date::New();
            break;
        case plist::ObjectType::Data:
            root->object = plist::Data::New();
            break;
        default:
            fprintf(stderr, "Unsupported type\n");
            return false;
    }
    return true;
}

static bool
Delete(plist::Object *object, std::queue<std::string> *keyPath) {
    plist::Object *target = GetObjectAtKeyPath(object, keyPath, true);
    if (!target || keyPath->size() != 1) {
        fprintf(stderr, "Invalid key path (target object not found)\n");
        return false;
    }

    std::string targetKey = keyPath->front();

    switch (target->type()) {
        case plist::ObjectType::Dictionary: {
            plist::Dictionary *dict = plist::CastTo<plist::Dictionary>(target);
            dict->remove(targetKey);
            return true;
        }
        case plist::ObjectType::Array: {
            char *end = NULL;
            long long index = std::strtoll(targetKey.c_str(), &end, 0);
            if (end == targetKey.c_str() || index < 0) {
                fprintf(stderr, "Invalid array index\n");
                return false;
            }

            plist::Array *array = plist::CastTo<plist::Array>(target);
            array->remove(index);
            return true;
        }
        default:
            fprintf(stderr, "Invalid key path (removing value on non-collection object)\n");
            return false;
    }
}

static bool
Merge(plist::Object *object, std::string &mergeSource, std::queue<std::string> *keyPath, Filesystem const *filesystem)
{
    std::unique_ptr<plist::Object> mergeObject = ReadPropertyList(filesystem, mergeSource);
    if (mergeObject == nullptr) {
        fprintf(stderr, "Unable to read merge source file\n");
        return false;
    }

    plist::Object *mergeTarget = GetObjectAtKeyPath(object, keyPath, false);
    if (!keyPath->empty() || mergeTarget == nullptr) {
        fprintf(stderr, "Invalid key path (no object at key path)\n");
        return false;
    }

    if (auto mergeTargetDictionary = plist::CastTo<plist::Dictionary>(mergeTarget)) {
        if (auto mergeObjectDictionary = plist::CastTo<plist::Dictionary>(mergeObject.get())) {
            mergeTargetDictionary->merge(mergeObjectDictionary);
            return true;
        } else if (auto mergeObjectArray = plist::CastTo<plist::Array>(mergeObject.get())) {
            (void)mergeObjectArray;
            fprintf(stderr, "Cannot merge array into dictionary\n");
            return false;
        } else {
            /* Add object to empty key. */
            mergeTargetDictionary->set("", std::move(mergeObject));
            return true;
        }
    } else if (auto mergeTargetArray = plist::CastTo<plist::Array>(mergeTarget)) {
        if (auto mergeObjectDictionary = plist::CastTo<plist::Dictionary>(mergeObject.get())) {
            (void)mergeObjectDictionary;
            fprintf(stderr, "Cannot merge dictionary into array\n");
            return false;
        } else if (auto mergeObjectArray = plist::CastTo<plist::Array>(mergeObject.get())) {
            mergeTargetArray->merge(mergeObjectArray);
            return true;
        } else {
            mergeTargetArray->append(std::move(mergeObject));
            return true;
        }
    } else {
        fprintf(stderr, "Object at KeyPath is not a container\n");
        return false;
    }
}

static bool
ProcessCommand(Filesystem *filesystem, std::string const &path, bool xml, RootObjectContainer &root, std::string const &input, bool *mutated, bool *keepReading)
{
    std::vector<std::string> tokens;
    std::stringstream sstream(input);
    std::copy(std::istream_iterator<std::string>(sstream), std::istream_iterator<std::string>(), std::back_inserter(tokens));

    if (tokens.size() < 1) {
        return true;
    }

    std::string command = tokens[0];
    if (command == "Print") {
        std::queue<std::string> keyPath;
        if (tokens.size() > 1) {
            ParseCommandKeyPathString(tokens[1], &keyPath);
        }
        return Print(root.object.get(), keyPath, xml);
    } else if (command == "Save") {
        std::queue<std::string> keyPath;
        return Print(root.object.get(), keyPath, xml, filesystem, path);
    } else if (command == "Exit") {
        return true;
    } else if (command == "Set") {
        if (tokens.size() < 2) {
            fprintf(stderr, "Set command requires KeyPath\n");
            return false;
        } else {
            std::queue<std::string> keyPath;
            ParseCommandKeyPathString(tokens[1], &keyPath);
            if (!Set(root.object.get(), keyPath, plist::ObjectType::String, ParseCommandValueString(tokens.begin() + 2, tokens.end()))) {
                /* don't set mutated */
                return false;
            }
            *mutated = true;
        }
    } else if (command == "Add") {
        if (tokens.size() < 3) {
            fprintf(stderr, "Add command requires KeyPath and Type\n");
            return false;
        } else {
            std::queue<std::string> keyPath;
            ParseCommandKeyPathString(tokens[1], &keyPath);
            plist::ObjectType type = ParseType(tokens[2]);
            if (!Set(root.object.get(), keyPath, type, ParseCommandValueString(tokens.begin() + 3, tokens.end()), false)) {
                /* don't set mutated */
                return false;
            }
            *mutated = true;
        }
    } else if (command == "Clear") {
        plist::ObjectType clearType;
        if (tokens.size() < 2) {
            clearType = plist::ObjectType::Dictionary;
        } else {
            clearType = ParseType(tokens[1]);
        }
        if (!Clear(&root, clearType)) {
            /* don't set mutated */
            return false;
        }
        *mutated = true;
    } else if (command == "Delete") {
        if (tokens.size() < 2) {
            fprintf(stderr, "Add command requires KeyPath\n");
            /* don't set mutated */
            return false;
        }
        std::queue<std::string> keyPath;
        ParseCommandKeyPathString(tokens[1], &keyPath);
        if (!Delete(root.object.get(), &keyPath)) {
            /* don't set mutated */
            return false;
        }
        *mutated = true;
    } else if (command == "Merge") {
        std::queue<std::string> keyPath;
        if (tokens.size() > 2) {
            ParseCommandKeyPathString(tokens[2], &keyPath);
        }

        if (!Merge(root.object.get(), tokens[1], &keyPath, filesystem)) {
            return false;
        }
        *mutated = true;
    } else if (command == "Help") {
        CommandHelp();
    } else {
        fprintf(stderr, "Unrecognized command\n");
        return false;
    }
    return true;
}

int
main(int argc, char **argv)
{
    std::vector<std::string> args = std::vector<std::string>(argv + 1, argv + argc);
    DefaultFilesystem filesystem = DefaultFilesystem();

    Options options;
    std::pair<bool, std::string> result = libutil::Options::Parse<Options>(&options, args);
    if (!result.first) {
        return Help(result.second);
    }

    if (options.help()) {
        Help();
        CommandHelp();
        return 0;
    }

    if (!options.input().empty() && !filesystem.exists(options.input())) {
        fprintf(stderr, "File does not exist, will create %s\n", options.input().c_str());
        if (!filesystem.createFile(options.input())) {
            fprintf(stderr, "Failed to create file at path %s\n", options.input().c_str());
            return 1;
        }
    }

    RootObjectContainer root;
    root.object = ReadPropertyList(&filesystem, options.input());
    if (!root.object) {
        return 1;
    }

    bool success = true;
    if (options.command()) {
        bool keepReading = true; // unused here
        bool mutated = false;
        success &= ProcessCommand(&filesystem, options.input(), options.xml(), root, *options.command(), &mutated, &keepReading);
        /* If there was a failure, mutated would be false */
        if (mutated) {
            /* Save result. */
            std::queue<std::string> keyPath;
            Print(root.object.get(), keyPath, options.xml(), &filesystem, options.input());
        }
    } else {
        char *line;
        bool keepReading = true;
        while (keepReading) {
#if defined(HAVE_LINENOISE)
            line = linenoise("Command: ");
            if (line[0] != '\0') {
                linenoiseHistoryAdd(line);
#else
            fprintf(stdout, "Command: ");

            size_t size = 1024;
            line = static_cast<char *>(malloc(size));
            if (fgets(line, size, stdin) == line) {
#endif

                bool mutated = false;
                success &= ProcessCommand(&filesystem, options.input(), options.xml(), root, std::string(line), &mutated, &keepReading);
            } else {
                keepReading = false;
            }
#if defined(HAVE_LINENOISE)
            linenoiseFree(static_cast<void *>(line));
#else
            free(static_cast<void *>(line));
#endif
        }
    }

    return (success ? 0 : 1);
}
