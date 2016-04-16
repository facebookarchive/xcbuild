/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <bom/bom.h>
#include <libutil/Options.h>

#include <memory>
#include <string>
#include <unordered_map>

#include <arpa/inet.h>

class Options {
private:
    bool _help;

private:
    bool _includeBlockDevices;
    bool _includeCharacterDevices;
    bool _includeDirectories;
    bool _includeFiles;
    bool _includeSymbolicLinks;

private:
    bool _printMTime;
    bool _onlyPath;
    bool _noModes;

private:
    std::string _arch;

private:
    bool _printChecksum;
    bool _printFileName;
    bool _printFileNameQuoted;
    bool _printGroupID;
    bool _printGroupName;
    bool _printPermissions;
    bool _printPermissionsText;
    bool _printFileSize;
    bool _printFileSizeFormatted;
    bool _printModificationTime;
    bool _printModificationTimeFormatted;
    bool _printUserID;
    bool _printUserName;
    bool _printUserGroupID;
    bool _printUserGroupName;

private:
    std::string _input;

public:
    Options();
    ~Options();

public:
    bool help() const
    { return _help; }

public:
    bool includeBlockDevices() const
    { return _includeBlockDevices; }
    bool includeCharacterDevices() const
    { return _includeCharacterDevices; }
    bool includeDirectories() const
    { return _includeDirectories; }
    bool includeFiles() const
    { return _includeFiles; }
    bool includeSymbolicLinks() const
    { return _includeSymbolicLinks; }

public:
    bool printMTime() const
    { return _printMTime; }
    bool onlyPath() const
    { return _onlyPath; }
    bool noModes() const
    { return _noModes; }

public:
    std::string const &arch() const
    { return _arch; }

public:
    bool printChecksum() const
    { return _printChecksum; }
    bool printFileName() const
    { return _printFileName; }
    bool printFileNameQuoted() const
    { return _printFileNameQuoted; }
    bool printGroupID() const
    { return _printGroupID; }
    bool printGroupName() const
    { return _printGroupName; }
    bool printPermissions() const
    { return _printPermissions; }
    bool printPermissionsText() const
    { return _printPermissionsText; }
    bool printFileSize() const
    { return _printFileSize; }
    bool printFileSizeFormatted() const
    { return _printFileSizeFormatted; }
    bool printModificationTime() const
    { return _printModificationTime; }
    bool printModificationTimeFormatted() const
    { return _printModificationTimeFormatted; }
    bool printUserID() const
    { return _printUserID; }
    bool printUserName() const
    { return _printUserName; }
    bool printUserGroupID() const
    { return _printUserGroupID; }
    bool printUserGroupName() const
    { return _printUserGroupName; }

public:
    std::string const &input() const
    { return _input; }

private:
    friend class libutil::Options;
    std::pair<bool, std::string>
    parseArgument(std::vector<std::string> const &args, std::vector<std::string>::const_iterator *it);
};

Options::
Options() :
    _help(false),
    _includeBlockDevices(false),
    _includeCharacterDevices(false),
    _includeDirectories(false),
    _includeFiles(false),
    _includeSymbolicLinks(false),
    _printMTime(false),
    _onlyPath(false),
    _noModes(false),
    _printChecksum(false),
    _printFileName(false),
    _printFileNameQuoted(false),
    _printGroupID(false),
    _printGroupName(false),
    _printPermissions(false),
    _printPermissionsText(false),
    _printFileSize(false),
    _printFileSizeFormatted(false),
    _printModificationTime(false),
    _printModificationTimeFormatted(false),
    _printUserID(false),
    _printUserName(false),
    _printUserGroupID(false),
    _printUserGroupName(false)
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

    if (arg == "-h" || arg == "--help") {
        return libutil::Options::MarkBool(&_help, arg, it);
    } else if (arg == "-b") {
        return libutil::Options::MarkBool(&_includeBlockDevices, arg, it);
    } else if (arg == "-c") {
        return libutil::Options::MarkBool(&_includeCharacterDevices, arg, it);
    } else if (arg == "-d") {
        return libutil::Options::MarkBool(&_includeDirectories, arg, it);
    } else if (arg == "-f") {
        return libutil::Options::MarkBool(&_includeFiles, arg, it);
    } else if (arg == "-l") {
        return libutil::Options::MarkBool(&_includeSymbolicLinks, arg, it);
    } else if (arg == "-m") {
        return libutil::Options::MarkBool(&_printMTime, arg, it);
    } else if (arg == "-s") {
        return libutil::Options::MarkBool(&_onlyPath, arg, it);
    } else if (arg == "-x") {
        return libutil::Options::MarkBool(&_noModes, arg, it);
    } else if (arg == "--arch") {
        return libutil::Options::NextString(&_arch, args, it);
    } else if (arg == "-p") {
        std::string print;

        auto result = libutil::Options::NextString(&print, args, it);
        if (!result.first) {
            return result;
        }

        for (char c : print) {
            switch (c) {
                case 'c': _printChecksum = true; break;
                case 'f': _printFileName = true; break;
                case 'F': _printFileNameQuoted = true; break;
                case 'g': _printGroupID = true; break;
                case 'G': _printGroupName = true; break;
                case 'm': _printPermissions = true; break;
                case 'M': _printPermissionsText = true; break;
                case 's': _printFileSize = true; break;
                case 'S': _printFileSizeFormatted = true; break;
                case 't': _printModificationTime = true; break;
                case 'T': _printModificationTimeFormatted = true; break;
                case 'u': _printUserID = true; break;
                case 'U': _printUserName = true; break;
                case '/': _printUserGroupID = true; break;
                case '?': _printUserGroupName = true; break;
                default:
                    return std::make_pair(false, "unknown print option " + std::string(1, c));
            }
        }

        return std::make_pair(true, std::string());
    } else if (!arg.empty() && arg[0] != '-') {
        if (_input.empty()) {
            _input = arg;
            return std::make_pair(true, std::string());
        } else {
            return std::make_pair(false, "extra input " + arg);
        }
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

    fprintf(stderr, "Usage: lsbom [options] [inputs]\n\n");
    fprintf(stderr, "List the contents of a BOM archive.\n\n");

#define INDENT "  "
#define SEPARATOR "\t  "
    fprintf(stderr, INDENT "-h, --help (this message)\n");
    fprintf(stderr, "\n");

    fprintf(stderr, "Options:\n");
    fprintf(stderr, INDENT "-m" SEPARATOR "print modification times\n");
    fprintf(stderr, INDENT "-s" SEPARATOR "print only paths\n");
    fprintf(stderr, INDENT "-x" SEPARATOR "print no modes\n");
    fprintf(stderr, INDENT "--arch [arch]\n");
    fprintf(stderr, INDENT "-p [flags]\n");
    fprintf(stderr, "\n");

    fprintf(stderr, "Print flags:\n");
    fprintf(stderr, INDENT "c" SEPARATOR "print checksum\n");
    fprintf(stderr, INDENT "f" SEPARATOR "print file name\n");
    fprintf(stderr, INDENT "F" SEPARATOR "print file name (quoted)\n");
    fprintf(stderr, INDENT "g" SEPARATOR "print group id\n");
    fprintf(stderr, INDENT "G" SEPARATOR "print group name\n");
    fprintf(stderr, INDENT "m" SEPARATOR "print permissions\n");
    fprintf(stderr, INDENT "M" SEPARATOR "print permissions (text)\n");
    fprintf(stderr, INDENT "s" SEPARATOR "print file size\n");
    fprintf(stderr, INDENT "S" SEPARATOR "print file size (formatted)\n");
    fprintf(stderr, INDENT "t" SEPARATOR "print modification time\n");
    fprintf(stderr, INDENT "T" SEPARATOR "print modification time (formatted)\n");
    fprintf(stderr, INDENT "u" SEPARATOR "print user id\n");
    fprintf(stderr, INDENT "U" SEPARATOR "print user name\n");
    fprintf(stderr, INDENT "/" SEPARATOR "print user/group id\n");
    fprintf(stderr, INDENT "?" SEPARATOR "print user/group name\n");
    fprintf(stderr, "\n");

    fprintf(stderr, "Include:\n");
    fprintf(stderr, INDENT "-b" SEPARATOR "include block devices\n");
    fprintf(stderr, INDENT "-c" SEPARATOR "include character devices\n");
    fprintf(stderr, INDENT "-d" SEPARATOR "include directories\n");
    fprintf(stderr, INDENT "-f" SEPARATOR "include files\n");
    fprintf(stderr, INDENT "-l" SEPARATOR "include symbolic links\n");
    fprintf(stderr, "\n");
#undef INDENT
#undef SEPARATOR

    return (error.empty() ? 0 : -1);
}

extern "C" {
    enum bom_path_type {
      bom_path_type_file = 1, // BOMPathInfo2 is exe=88 regular=35 bytes
      bom_path_type_directory = 2, // BOMPathInfo2 is 31 bytes
      bom_path_type_link = 3, // BOMPathInfo2 is 44? bytes
      bom_path_type_device = 4  // BOMPathInfo2 is 35 bytes
    };

    struct bom_path_info_2 {
      uint8_t type; // See types above
      uint8_t unknown0; // = 1?
      uint16_t architecture; // Not sure exactly what this means...
      uint16_t mode;
      uint32_t user;
      uint32_t group;
      uint32_t modtime;
      uint32_t size;
      uint8_t unknown1; // = 1?
      union {
        uint32_t checksum;
        uint32_t devType;
      };
      uint32_t linkNameLength;
      char linkName[0];

      // FIXME: executable files have a buch of other crap here:
    } __attribute__((packed));

    struct bom_path_info_1 {
      uint32_t id;
      uint32_t index; // Pointer to BOMPathInfo2
    } __attribute__((packed));

    struct bom_file {
      uint32_t parent; // Parent BOMPathInfo1->id
      char name[0];
    } __attribute__((packed));
}

int
main(int argc, char **argv)
{
    std::vector<std::string> args = std::vector<std::string>(argv + 1, argv + argc);

    /*
     * Parse out the options, or print help & exit.
     */
    Options options;
    std::pair<bool, std::string> result = libutil::Options::Parse<Options>(&options, args);
    if (!result.first) {
        return Help(result.second);
    }

    /*
     * Basic arguments.
     */
    if (options.help()) {
        return Help();
    }

    /*
     * Validate options.
     */
    if (options.input().empty()) {
        return Help("input is required");
    }
    if (!options.arch().empty()) {
        fprintf(stderr, "warning: arch argument not yet implemented\n");
    }

    /* If no include options are specified, include all. */
    bool includeAll = (
        !options.includeBlockDevices() &&
        !options.includeCharacterDevices() &&
        !options.includeDirectories() &&
        !options.includeFiles() &&
        !options.includeSymbolicLinks());

    /*
     * Load the BOM file.
     */
    struct bom_context_memory memory = bom_context_memory_file(options.input().c_str(), false);
    auto bom = std::unique_ptr<struct bom_context, decltype(&bom_free)>(bom_alloc_load(memory), bom_free);
    if (bom == nullptr) {
        fprintf(stderr, "error: failed to load BOM\n");
        return 1;
    }

    /*
     * Create the BOM tree for the paths.
     */
    auto tree = std::unique_ptr<struct bom_tree_context, decltype(&bom_tree_free)>(bom_tree_alloc_load(bom.get(), "Paths"), bom_tree_free);
    if (tree == NULL) {
        fprintf(stderr, "error: failed to load paths tree\n");
        return 1;
    }

    /*
     * Iterate contents of the tree and print it out.
     */

    /* Store file pointers to their parent information. */
    struct file_info {
        uint32_t parent;
        std::string name;
    };
    std::unordered_map<uint32_t, struct file_info> files;

    /* Store data needed inside the iteration. */
    struct iteration_context {
        Options const *options;
        bool includeAll;
        struct bom_context *bom;
        std::unordered_map<uint32_t, struct file_info> *files;
    } context = {
        &options,
        includeAll,
        bom.get(),
        &files,
    };

    bom_tree_iterate(tree.get(), [](struct bom_tree_context *tree, void *key, size_t key_len, void *value, size_t value_len, void *ctx) {
        struct iteration_context *context = reinterpret_cast<struct iteration_context *>(ctx);
        Options const *options = context->options;

        /*
         * Collect information about this entry.
         */
        struct bom_file *file_key = (struct bom_file *)key;
        struct bom_path_info_1 *path_info_1_value = (struct bom_path_info_1 *)value;

        /*
         * Store file information for computing full path.
         */
        struct file_info info = { file_key->parent, std::string(file_key->name) };
        uint32_t path_info_1_value_id = path_info_1_value->id;
        context->files->insert({ path_info_1_value_id, info });

        /*
         * Extract the secondary information for the file. This information is structured differently
         * per file type, but the general structure is close enough to avoid branching. Note the ntohl.
         */
        size_t value_2_len;
        struct bom_path_info_2 *path_info_2_value = (struct bom_path_info_2 *)bom_index_get(context->bom, ntohl(path_info_1_value->index), &value_2_len);
        if (path_info_2_value == NULL) {
            fprintf(stderr, "error: failed to get secondary path info\n");
            return;
        }

        /*
         * Filter by included file type.
         */
        if (!context->includeAll) {
            if (path_info_2_value->type == bom_path_type_file) {
                if (!options->includeFiles()) {
                    return;
                }
            } else if (path_info_2_value->type == bom_path_type_directory) {
                if (!options->includeDirectories()) {
                    return;
                }
            } else if (path_info_2_value->type == bom_path_type_link) {
                if (!options->includeSymbolicLinks()) {
                    return;
                }
            } else if (path_info_2_value->type == bom_path_type_device) {
                if (ntohs(path_info_2_value->mode) & 0x4000) {
                    if (!options->includeBlockDevices()) {
                        return;
                    }
                } else {
                    if (!options->includeCharacterDevices()) {
                        return;
                    }
                }
            } else {
                fprintf(stderr, "error: unknown file type %x\n", path_info_2_value->type);
                return;
            }
        }

        /*
         * Load full file path.
         */
        std::string path = std::string(file_key->name);
        uint32_t next_index = file_key->parent;
        for (auto it = context->files->find(next_index); it != context->files->end(); it = context->files->find(next_index)) {
            path = it->second.name + "/" + path;
            next_index = it->second.parent;
        }

        /*
         * Print out requested details.
         */
        if (options->onlyPath()) {
            printf("%s\n", path.c_str());
        } else {
            // TODO: Respect options about what to print.
            printf("%s\n", path.c_str());
        }
    }, reinterpret_cast<void *>(&context));

    return 0;
}


