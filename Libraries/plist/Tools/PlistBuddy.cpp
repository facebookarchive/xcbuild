/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <libutil/Options.h>
#include <libutil/DefaultFilesystem.h>
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

#include <linenoise.h>
#include <iostream>
#include <sstream>
#include <string>
#include <queue>

typedef struct {
    std::unique_ptr<plist::Object> object;
} RootObjectContainer;

class Options {
public:

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
        return libutil::Options::Current(&_help, arg, it);
    } else if (arg == "-x") {
        return libutil::Options::Current(&_xml, arg, it);
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
    fprintf(stderr, "\n<KeyPath>\n");
    fprintf(stderr, INDENT ":= \"\"                             => root object\n");
    fprintf(stderr, INDENT ":= <KeyPath>[:<Dictionary Key>]   => indexes into dictionary\n");
    fprintf(stderr, INDENT ":= <KeyPath>[:<Array Index>]      => indexes into Array\n");
    fprintf(stderr, "\n<Type> := (string|dictionary|array|bool|real|integer|date|data)\n\n");
#undef INDENT
}

static void
ParseCommandKeyPathString(std::string const &keyPathString, std::queue<std::string> *keyPath) {
    auto loc = keyPathString.find(':');
    if (loc == std::string::npos) {
        if (keyPathString.size() > 0) {
            keyPath->push(keyPathString);
        }
    } else {
        if (loc > 0) {
            keyPath->push(keyPathString.substr(0, loc));
        }
        ParseCommandKeyPathString(keyPathString.substr(loc + 1), keyPath);
    }
}

static std::string parseCommandValueString(std::vector<std::string>::const_iterator begin, std::vector<std::string>::const_iterator end) {
    if (begin == end) {
        return "";
    }
    std::stringstream sstream;
    for (auto it = begin; it != end; it++) {
        if (it != begin) {
            sstream << ' ';
        }
        sstream << it->c_str();
    }

    std::string valueString = sstream.str();
    if (valueString.front() == '\"' && valueString.back() == '\"') {
        // strip double quote if necessary
        valueString = valueString.substr(1, valueString.size() - 2);
    }
    return valueString;
}

static plist::Object *getObjectAtKeyPath(plist::Object *object, std::queue<std::string> &remainingKeys, bool leaveLastKey = true) {
    if (remainingKeys.empty() || (leaveLastKey && remainingKeys.size() == 1)) {
        return object;
    } else if (!object) {
        fprintf(stderr, "Invalid key path (indexing into null object)\n");
        return nullptr;
    }

    std::string currentKey = remainingKeys.front();

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
            // Reached a state of non-collection object with remaining key path. error
            fprintf(stderr, "Invalid key path (indexing into non-collection object)\n");
            return nullptr;
    }

    if (!subObject) {
        return object;
    } else {
        remainingKeys.pop();
        return getObjectAtKeyPath(subObject, remainingKeys, leaveLastKey);
    }
}

static bool
Print(plist::Object *object, std::queue<std::string> &keyPath, bool xml, libutil::Filesystem *fileSystem = nullptr, std::string path = "-")
{
    plist::Object *target = getObjectAtKeyPath(object, keyPath, false);
    if (!keyPath.empty() || !target) {
        fprintf(stderr, "Invalid key path (no object at key path)\n");
        return false;
    }

    plist::Format::ASCII ascii = plist::Format::ASCII::Create(false, plist::Format::Encoding::UTF8);
    plist::Format::Any out = plist::Format::Any::Create<plist::Format::ASCII>(ascii);
    /* Convert to xml if needed */
    if (xml) {
        plist::Format::XML xml = plist::Format::XML::Create(plist::Format::Encoding::UTF8);
        out  = plist::Format::Any::Create<plist::Format::XML>(xml);
    }

    auto serialize = plist::Format::Any::Serialize(target, out);
    if (serialize.first == nullptr) {
        fprintf(stderr, "error: %s\n", serialize.second.c_str());
        return false;
    }

    /* Print. */
    if (fileSystem && path != "-") {
        fileSystem->write(*serialize.first, path);
    } else {
        std::copy(serialize.first->begin(), serialize.first->end(), std::ostream_iterator<char>(std::cout));
    }

    return true;
}

static bool
Set(plist::Object *object, std::queue<std::string> &keyPath, plist::ObjectType type, const std::string &valueString, bool overwrite = true)
{
    plist::Object *target = getObjectAtKeyPath(object, keyPath, true);
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

static bool Clear(RootObjectContainer &root, plist::ObjectType clearType) {
    switch (clearType) {
        case plist::ObjectType::String:
            root.object = plist::String::New();
            break;
        case plist::ObjectType::Dictionary:
            root.object = plist::Dictionary::New();
            break;
        case plist::ObjectType::Array:
            root.object = plist::Array::New();
            break;
        case plist::ObjectType::Boolean:
            root.object = plist::Boolean::New(false);
            break;
        case plist::ObjectType::Real:
            root.object = plist::Real::New();
            break;
        case plist::ObjectType::Integer:
            root.object = plist::Integer::New();
            break;
        case plist::ObjectType::Date:
            root.object = plist::Date::New();
            break;
        case plist::ObjectType::Data:
            root.object = plist::Data::New();
            break;
        default:
            fprintf(stderr, "Unsupported type\n");
            return false;
    }
    return true;
}

static bool Delete(plist::Object *object, std::queue<std::string> &keyPath) {
    plist::Object *target = getObjectAtKeyPath(object, keyPath, true);
    if (!target || keyPath.size() != 1) {
        fprintf(stderr, "Invalid key path (target object not found)\n");
        return false;
    }

    std::string targetKey = keyPath.front();

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
ProcessCommand(libutil::Filesystem *filesystem, std::string path, bool xml, std::string const &file, RootObjectContainer &root, std::string const &input)
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
        Print(root.object.get(), keyPath, xml);
    } else if (command == "Save") {
        std::queue<std::string> keyPath;
        Print(root.object.get(), keyPath, xml, filesystem, path);
    } else if (command == "Exit") {
        return false;
    } else if (command == "Set") {
        if (tokens.size() < 2) {
            fprintf(stderr, "Set command requires KeyPath\n");
            return true;
        } else {
            std::queue<std::string> keyPath;
            ParseCommandKeyPathString(tokens[1], &keyPath);
            Set(root.object.get(), keyPath, plist::ObjectType::String, parseCommandValueString(tokens.begin() + 2, tokens.end()));
        }
    } else if (command == "Add") {
        if (tokens.size() < 3) {
            fprintf(stderr, "Add command requires KeyPath and Type\n");
            return true;
        } else {
            std::queue<std::string> keyPath;
            ParseCommandKeyPathString(tokens[1], &keyPath);
            plist::ObjectType type = ParseType(tokens[2]);
            Set(root.object.get(), keyPath, type, parseCommandValueString(tokens.begin() + 3, tokens.end()), false);
        }
    } else if (command == "Clear") {
        plist::ObjectType clearType;
        if (tokens.size() < 2) {
            clearType = plist::ObjectType::Dictionary;
        } else {
            clearType = ParseType(tokens[1]);
        }
        Clear(root, clearType);
    } else if (command == "Delete") {
        if (tokens.size() < 2) {
            fprintf(stderr, "Add command requires KeyPath\n");
            return true;
        }
        std::queue<std::string> keyPath;
        ParseCommandKeyPathString(tokens[1], &keyPath);
        Delete(root.object.get(), keyPath);
    } else if (command == "Help") {
        CommandHelp();
    } else {
        fprintf(stderr, "Unrecognized command\n");
    }
    return true;
}

int
main(int argc, char **argv)
{
    std::vector<std::string> args = std::vector<std::string>(argv + 1, argv + argc);
    auto filesystem = libutil::DefaultFilesystem();

    Options options;
    std::pair<bool, std::string> result = libutil::Options::Parse<Options>(&options, args);
    if (!result.first) {
        return Help(result.second);
    }

    if (options.help() || options.input().empty()) {
        Help();
        return 1;
    }

    if (!filesystem.exists(options.input())) {
        fprintf(stderr, "File does not exist, will be created: %s\n", options.input().c_str());
    }

    std::vector<uint8_t> contents;
    if (!filesystem.read(&contents, options.input())) {
        return false;
    }

    auto format = plist::Format::Any::Identify(contents);
    if (format == nullptr) {
        fprintf(stderr, "error: input %s not a plist\n", options.input().c_str());
        return 1;
    }

    auto deserialize = plist::Format::Any::Deserialize(contents, *format);
    if (!deserialize.first) {
        fprintf(stderr, "error: %s\n", deserialize.second.c_str());
        return 1;
    }

    RootObjectContainer root;
    root.object = std::move(deserialize.first);

    if (options.command()) {
        ProcessCommand(&filesystem, options.input(), options.xml(), options.input(), root, *options.command());
        // saves
        std::queue<std::string> keyPath;
        Print(root.object.get(), keyPath, options.xml(), &filesystem, options.input());
    } else {
        const char *line;
        bool keepReading = true;

        while (keepReading) {
            /* linenoise calls will go here */
            line = linenoise("Command: ");
            if (line[0] != '\0') {
                linenoiseHistoryAdd(line);
                std::string strline = std::string(line);
                keepReading = ProcessCommand(&filesystem, options.input(), options.xml(), options.input(), root, strline);
            } else {
                keepReading = false;
            }
            linenoiseFree((void *)line);
        }
    }

    return 0;
}
