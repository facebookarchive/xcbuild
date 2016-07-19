/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <libutil/Options.h>
#include <libutil/DefaultFilesystem.h>

#include <histedit.h>
#include <string>

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

static bool
ProcessCommand(libutil::Filesystem *filesystem, bool xml, std::string const &file, std::string const &command)
{
    fprintf(stderr, "Unrecognized command");
    return true;
}

static const char * prompt(EditLine *e) {
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

    if (!options.command().empty()) {
        ProcessCommand(filesystem.get(), options.xml(), options.input(), options.command());
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
              ProcessCommand(filesystem.get(), options.xml(), options.input(),  strline);
            } else {
                keepReading = false;
            }
        }

        history_end(cmdhistory);
        el_end(el);
    }

    return 0;
}
