/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __plist_BinaryParser_h
#define __plist_BinaryParser_h

#include <plist/Object.h>
#include <plist/ABPReader.h>

namespace plist {

class BinaryParser {
private:
    ABPContext          _context;
    ABPStreamCallBacks  _streamCallBacks;
    ABPCreateCallBacks  _createCallBacks;
    error_function      _error;
    std::set <Object *> _seen;

public:
    BinaryParser();

public:
    Object *parse(std::string const &path, error_function const &error);
    Object *parse(std::FILE *fp, error_function const &error);

private:
    static off_t Seek(void *opaque, off_t offset, int whence);
    static ssize_t Read(void *opaque, void *buffer, size_t size);

private:
    static Object *Create(void *opaque, ABPRecordType type,
            void *arg1, void *arg2, void *arg3);
    static void Error(void *opaque, char const *message);
};

}

#endif  // !__plist_BinaryParser_h
