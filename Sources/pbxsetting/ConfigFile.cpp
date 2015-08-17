// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxsetting/ConfigFile.h>
#include <pbxsetting/Level.h>
#include <pbxsetting/Setting.h>

#include <cstdio>

using pbxsetting::ConfigFile;
using pbxsetting::Level;
using pbxsetting::Setting;
using libutil::string_set;
using libutil::string_map;
using libutil::string_vector;
using libutil::trim;
using libutil::FSUtil;

ConfigFile::ConfigFile()
{
    _current.fp = nullptr;
}

std::pair<bool, Level> ConfigFile::
open(std::string const &path, Environment const &environment, XC::Config::error_function const &error)
{
    bool parsed = false;
    std::vector<Setting> settings;

    _stop = false;
    _error = error;

    if (parse(path, environment) && !_stop) {
        parsed = true;

        std::string line;
        while (std::getline(_processed, line)) {
            trim(line);

            if (line.size() > 0) {
                if (line.back() == ';') {
                    line = line.substr(0, line.size() - 1);
                }

                Setting setting = Setting::Parse(line);
                settings.push_back(setting);
            }
        }
    }

    _included.clear();
    _processed.clear();
    _error = nullptr;
    _stop = false;

    return std::make_pair(parsed, Level(settings));
}

bool ConfigFile::
parse(std::string const &path, Environment const &environment)
{
    std::string realPath = FSUtil::ResolvePath(path);
    if (realPath.empty()) {
        //
        // Add as missing.
        //
        return false;
    }

    //
    // Already processed?
    //
    if (_included.find(realPath) != _included.end()) {
        return true;
    }

    _included.insert(realPath);

    push();
    _current.path = realPath;
    _current.fp   = std::fopen(_current.path.c_str(), "rt");
    if (_current.fp == nullptr) {
        pop();
        return false;
    }

    process(environment);

    pop();

    return true;
}

void ConfigFile::
push()
{
    if (_current.fp != nullptr) {
        _files.push_back(_current);
    }
    _current.fp = nullptr;
    _current.path.clear();
}

void ConfigFile::
pop()
{
    if (_current.fp != nullptr) {
        std::fclose(_current.fp);
        _current.fp = nullptr;
        _current.path.clear();
    }

    if (_files.empty())
        return;

    _current = _files.back();
    _files.pop_back();
}

void ConfigFile::
process(Environment const &environment)
{
    enum {
        kNormal,
        kCComment,
        kCPPComment
    };

    //
    // Create a stream with no comments, but always emit \n
    // so that line count matches.
    //
    std::stringstream cls;

    int lost = EOF;
    int c, state = kNormal;

    for (;;) {
        if (lost != EOF) {
            c = lost, lost = EOF;
        } else {
            c = std::fgetc(_current.fp);
        }
        if (c == EOF)
            break;

        switch (state) {
            case kNormal:
                if (c == '/') {
                    char nc = std::fgetc(_current.fp);
                    if (nc == '/') {
                        state = kCPPComment;
                        break;
                    } if (nc == '*') {
                        state = kCComment;
                        break;
                    } else {
                        lost = nc;
                    }
                }
                // Otherwise push the character to the stream
                cls << static_cast <char> (c);
                break;

            case kCComment:
                if (c == '*') {
                    char nc = std::fgetc(_current.fp);
                    if (nc == '/') {
                        state = kNormal;
                    } else {
                        lost = nc;
                    }
                } else if (c == '\n') {
                    cls << static_cast <char> (c);
                }
                break;

            case kCPPComment:
                if (c == '\n' || c == '\r') {
                    state = kNormal;
                    cls << static_cast <char> (c);
                }
                break;

            default:
                cls << static_cast <char> (c);
                break;
        }
    }

    //
    // Now we have a comment-less stream, process #include directives.
    //
    std::string line;
    size_t      lineno = 0;

    while (std::getline(cls, line)) {
        lineno++;

        if (line.empty())
            continue;

        trim(line);
        if (line[0] == '#') {
            if (line.substr(1, 7) == "include") {
                line = line.substr(8);
                trim(line);
                if (line.front() != '"' || line.back() != '"') {
                    error(lineno, "Malformed #include directive");
                    if (_stop)
                        break;
                } else {
                    std::string filename = line.substr(1, line.length() - 2);

                    std::string root_prefix = "/";
                    std::string developer_prefix = "<DEVELOPER_DIR>";

                    if (filename.compare(0, root_prefix.size(), root_prefix) == 0) {
                    } else if (filename.compare(0, developer_prefix.size(), developer_prefix) == 0) {
                        std::string developer_dir = environment.resolve("DEVELOPER_DIR");
                        filename = developer_dir + filename.substr(developer_prefix.size());
                    } else {
                        filename = FSUtil::GetDirectoryName(_current.path) + "/" + filename;
                    }

                    if (!parse(filename, environment)) {
                        error(lineno, "Unable to find included file \"%s\"", filename.c_str());
                    }
                    if (_stop)
                        break;
                }
            } else {
                error(lineno, "Unknown directive");
                if (_stop)
                    break;
            }
        } else {
            _processed << line << std::endl;
        }
    }
}

void ConfigFile::
error(unsigned line, std::string const &format, ...)
{
    static char const sErrorMessage[] = "error";

    va_list  ap;
    char    *buf;

    va_start(ap, format);
    if (::vasprintf(&buf, format.c_str(), ap) < 0) {
        buf = const_cast <char *> (sErrorMessage);
    }
    va_end(ap);

    _stop = !_error(_current.path, line, buf);

    if (buf != sErrorMessage) {
        std::free(buf);
    }
}
