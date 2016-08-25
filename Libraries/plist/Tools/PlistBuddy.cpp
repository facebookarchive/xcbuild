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

#include <histedit.h>
#include <iostream>
#include <sstream>
#include <string>
#include <queue>

class Options {
public:

private:
    bool                     _help;
    bool                     _xml;

private:
    std::string              _command;

private:
    std::string              _input;

public:
    Options();
    ~Options();

public:
    bool help() const
    { return _help; }
    bool xml() const
    { return _xml; }

public:
    std::string command() const
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
Options() :
    _help   (false),
    _xml    (false),
    _command("")
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
        return libutil::Options::MarkBool(&_help, arg, it);
    } else if (arg == "-x") {
        return libutil::Options::MarkBool(&_xml, arg, it);
    } else if (arg == "-c") {
        return libutil::Options::NextString(&_command, args, it);
    } else if (!arg.empty() && arg[0] != '-') {
        _input = arg;
        return std::make_pair(true, std::string());
    } else {
        return std::make_pair(false, "unknown argument " + arg);
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

static void parseCommandKeyPathString(const std::string &keyPathString, std::queue<std::string> &keyPath) {
    auto loc = keyPathString.find(':');
    if (loc == std::string::npos) {
        if (keyPathString.size() > 0) {
            keyPath.push(keyPathString);
        }
    } else {
        if (loc > 0) {
            keyPath.push(keyPathString.substr(0, loc));
        }
        parseCommandKeyPathString(keyPathString.substr(loc + 1), keyPath);
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
        std::cerr << "Invalid key path (indexing into null object)" << std::endl;
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
                std::cerr << "Invalid array index" << std::endl;
                return nullptr;
            }

            subObject = plist::CastTo<plist::Array>(object)->value(index);
            break;
        }
        default:
            // Reached a state of non-collection object with remaining key path. error
            std::cerr << "Invalid key path (indexing into non-collection object)" << std::endl;
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
Print(plist::Object *object, std::queue<std::string> &keyPath)
{
    plist::Object *target = getObjectAtKeyPath(object, keyPath, false);
    if (!keyPath.empty() || !target) {
        std::cerr << "Invalid key path (no object at key path)" << std::endl;
        return false;
    }

    /* Convert to ASCII. */
    plist::Format::ASCII out = plist::Format::ASCII::Create(false, plist::Format::Encoding::UTF8);
    auto serialize = plist::Format::ASCII::Serialize(target, out);
    if (serialize.first == nullptr) {
        fprintf(stderr, "error: %s\n", serialize.second.c_str());
        return false;
    }

    /* Print. */
    std::copy(serialize.first->begin(), serialize.first->end(), std::ostream_iterator<char>(std::cout));

    return true;
}

static bool
Set(plist::Object *object, std::queue<std::string> &keyPath, plist::ObjectType type, const std::string &valueString, bool overwrite = true)
{
    plist::Object *target = getObjectAtKeyPath(object, keyPath, true);
    if (!target || keyPath.size() != 1) {
        std::cerr << keyPath.front() << ' ' << keyPath.back() << std::endl;
        std::cerr << "Invalid key path (target object not found)" << std::endl;
        return false;
    }

    if (target->type() != plist::ObjectType::Dictionary || target->type() != plist::ObjectType::Array) {
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
            std::cerr << "Not supported" << std::endl;
            return false;
    }

    switch (target->type()) {
        case plist::ObjectType::Dictionary: {
            plist::Dictionary *dict = plist::CastTo<plist::Dictionary>(target);
            if (!overwrite && dict->value(targetKey)) {
                std::cerr << "Cannot overwrite key path" << std::endl;
                return false;
            }
            dict->set(targetKey, std::move(newObj));
            return true;
        }
        case plist::ObjectType::Array: {
            char *end = NULL;
            long long index = std::strtoll(targetKey.c_str(), &end, 0);
            if (end == targetKey.c_str() || index < 0) {
                std::cerr << "Invalid array index" << std::endl;
                return false;
            }
            plist::Array *array = plist::CastTo<plist::Array>(target);
            if (!overwrite && array->value(index)) {
                std::cerr << "Cannot overwrite key path" << std::endl;
                return false;
            }
            array->insert(index, std::move(newObj));
            return true;
        }
        default:
            std::cerr << "Invalid key path (setting value on non-collection object)" << std::endl;
            return false;
    }
}

static void CommandHelp()
{
#define INDENT "  "
    fprintf(stderr, "\nCommands help:\n");
    fprintf(stderr, INDENT "Help - Print this information\n");
    fprintf(stderr, INDENT "Exit - Exits this program\n");
    fprintf(stderr, INDENT "Print [<KeyPath>] - Print value at KeyPath. (default KeyPath = root)\n");
    fprintf(stderr, INDENT "Set <KeyPath> <Value> - Set value at KeyPath to Value\n");
    fprintf(stderr, INDENT "Add <KeyPath> <Type> <Value> - Set value at KeyPath to Value\n");
    fprintf(stderr, "\n<KeyPath>\n");
    fprintf(stderr, INDENT ":= \"\"                             => root object\n");
    fprintf(stderr, INDENT ":= <KeyPath>[:<Dictionary Key>]   => indexes into dictionary\n");
    fprintf(stderr, INDENT ":= <KeyPath>[:<Array Index>]      => indexes into Array\n");
    fprintf(stderr, "\n<Type> := (string|dictionary|array|bool|real|integer|date|data)\n\n");
#undef INDENT
}

static bool
ProcessCommand(libutil::Filesystem *filesystem, bool xml, std::string const &file, plist::Object *object, std::string const &input)
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
        if (tokens.size() < 2) {
            parseCommandKeyPathString("", keyPath);
        } else {
            parseCommandKeyPathString(tokens[1], keyPath);
        }
        Print(object, keyPath);
    } else if (command == "Exit") {
        return false;
    } else if (command == "Set") {
        if (tokens.size() < 2) {
            std::cerr << "Set command requires KeyValue" << std::endl;
            return true;
        } else {
            std::queue<std::string> keyPath;
            parseCommandKeyPathString(tokens[1], keyPath);
            Set(object, keyPath, plist::ObjectType::String, parseCommandValueString(tokens.begin() + 2, tokens.end()));
        }
    } else if (command == "Add") {
        if (tokens.size() < 3) {
            std::cerr << "Add command requires KeyValue and Type" << std::endl;
            return true;
        } else {
            std::queue<std::string> keyPath;
            parseCommandKeyPathString(tokens[1], keyPath);
            plist::ObjectType type = plist::ObjectType::String;
            if (tokens[2] == "string") {
                type = plist::ObjectType::String;
            } else if (tokens[2] == "dictionary") {
                type = plist::ObjectType::Dictionary;
            } else if (tokens[2] == "array") {
                type = plist::ObjectType::Array;
            } else if (tokens[2] == "bool") {
                type = plist::ObjectType::Boolean;
            } else if (tokens[2] == "real") {
                type = plist::ObjectType::Real;
            } else if (tokens[2] == "integer") {
                type = plist::ObjectType::Integer;
            } else if (tokens[2] == "date") {
                type = plist::ObjectType::Date;
            } else if (tokens[2] == "data") {
                type = plist::ObjectType::Data;
            } else {
                std::cerr << "Invalid type" << std::endl;
                return true;
            }
            Set(object, keyPath, type, parseCommandValueString(tokens.begin() + 3, tokens.end()), false);
        }
    } else if (command == "Help") {
        CommandHelp();
    } else {
        std::cerr << "Unrecognized command" << std::endl;
    }
    return true;
}

static const char *prompt(EditLine *e) {
  return "Command: ";
}

int
main(int argc, char **argv)
{
    std::vector<std::string> args = std::vector<std::string>(argv + 1, argv + argc);
    auto filesystem = std::unique_ptr<libutil::Filesystem>(new libutil::DefaultFilesystem());

    Options options;
    std::pair<bool, std::string> result = libutil::Options::Parse<Options>(&options, args);
    if (!result.first) {
        return Help(result.second);
    }

    if (options.help() || options.input().empty()) {
        Help();
        return 1;
    }

    if (!filesystem->exists(options.input())) {
        fprintf(stderr, "File does not exist, will be created: %s\n", options.input().c_str());
    }

    std::vector<uint8_t> contents;
    if (!filesystem->read(&contents, options.input())) {
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

    std::unique_ptr<plist::Object> object = std::move(deserialize.first);

    if (!options.command().empty()) {
        ProcessCommand(filesystem.get(), options.xml(), options.input(), object.get(), options.command());
    } else {
        EditLine *el;
        History *cmdhistory;
        HistEvent ev;
        int count;
        const char *line;
        bool keepReading = true;

        el = el_init(argv[0], stdin, stdout, stderr);
        el_set(el, EL_PROMPT, &prompt);
        el_set(el, EL_EDITOR, "emacs");

        cmdhistory = history_init();
        history(cmdhistory, &ev, H_SETSIZE, 800);

        /* This sets up the call back functions for history functionality */
        el_set(el, EL_HIST, history, cmdhistory);

        while (keepReading) {
            /* libedit calls will go here */
            line = el_gets(el, &count);
            if (count > 0) {
              history(cmdhistory, &ev, H_ENTER, line);

              /* strip trailing newline */
              std::string strline = std::string(line, 0, count - 1);
              keepReading = ProcessCommand(filesystem.get(), options.xml(), options.input(), object.get(), strline);
            } else {
                keepReading = false;
            }
        }

        history_end(cmdhistory);
        el_end(el);
    }

    return 0;
}
