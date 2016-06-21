/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <plist/plist.h>
#include <iostream>

static std::pair<plist::Object *, std::string>
FindTargets(plist::Object *currentObject, plist::Adjustment const &adjustment)
{
    std::string path = adjustment.path();
    std::string::size_type start = 0;
    std::string::size_type end = 0;

    do {
        end = path.find('.', end);
        std::string key = (end != std::string::npos ? path.substr(start, end - start) : path.substr(start));

        if (end != std::string::npos) {
            /* Intermediate key path: continue iterating. */
            if (plist::Dictionary *dict = plist::CastTo<plist::Dictionary>(currentObject)) {
                currentObject = dict->value(key);
            } else if (plist::Array *array = plist::CastTo<plist::Array>(currentObject)) {
                uint64_t index = std::stoull(key.c_str(), NULL, 0);
                currentObject = array->value(index);
            }
        } else {
            return std::pair<plist::Object *, std::string>(currentObject, key);
        }

        if (currentObject == nullptr) {
            fprintf(stderr, "error: invalid key path\n");
            break;
        }

        start = end + 1;
    } while (end != std::string::npos);

    return std::pair<plist::Object *, std::string>(nullptr, nullptr);
}

std::pair<bool, std::vector<uint8_t>>
plist::Read(libutil::Filesystem const *filesystem, std::string const &path)
{
    std::vector<uint8_t> contents;

    if (path == "-") {
        /* - means read from stdin. */
        contents = std::vector<uint8_t>(std::istreambuf_iterator<char>(std::cin), std::istreambuf_iterator<char>());
    } else {
        /* Read from file. */
        if (!filesystem->read(&contents, path)) {
            return std::make_pair(false, std::vector<uint8_t>());
        }
    }

    return std::make_pair(true, std::move(contents));
}

bool
plist::Write(libutil::Filesystem *filesystem, std::vector<uint8_t> const &contents, std::string const &path)
{
    if (path == "-") {
        /* - means write to stdout. */
        std::copy(contents.begin(), contents.end(), std::ostream_iterator<char>(std::cout));
    } else {
        /* Read from file. */
        if (!filesystem->write(contents, path)) {
            return false;
        }
    }

    return true;
}

plist::Object *
plist::PerformAdjustment(plist::Object *rootObject, plist::Adjustment const &adjustment)
{
    auto targetPair = FindTargets(rootObject, adjustment);
    plist::Object *object = targetPair.first;
    std::string const &key = targetPair.second;
    plist::Object *returnObject = rootObject;

    if (plist::Dictionary *dict = plist::CastTo<plist::Dictionary>(object)) {
        switch (adjustment.type()) {
            case plist::Adjustment::Type::Insert:
                /* Only insert if doesn't already exist. */
                if (dict->value(key) == nullptr) {
                    dict->set(key, adjustment.value()->copy());
                }
                break;
            case plist::Adjustment::Type::Replace:
                /* Insert or replace as needed. */
                dict->set(key, adjustment.value()->copy());
                break;
            case plist::Adjustment::Type::Remove:
                dict->remove(key);
                break;
            case plist::Adjustment::Type::Extract:
                returnObject = dict->value(key);
                break;
        }
    } else if (plist::Array *array = plist::CastTo<plist::Array>(object)) {
        uint64_t index = std::stoull(key.c_str(), NULL, 0);

        switch (adjustment.type()) {
            case plist::Adjustment::Type::Insert: {
                /* Insert within the array, otherwise append. */
                if (index < array->count()) {
                    array->insert(index, adjustment.value()->copy());
                } else {
                    array->append(adjustment.value()->copy());
                }
                break;
            }
            case plist::Adjustment::Type::Replace: {
                /* Replace within the array, otherwise append. */
                if (index < array->count()) {
                    array->set(index, adjustment.value()->copy());
                } else {
                    array->append(adjustment.value()->copy());
                }
                break;
            }
            case plist::Adjustment::Type::Remove: {
                if (index < array->count()) {
                    array->remove(index);
                }
                break;
            }
            case plist::Adjustment::Type::Extract: {
                returnObject = array->value(index);
                break;
            }
        }
    }
    return returnObject;
}

plist::Object *
plist::Extract(plist::Object *readObject, std::string const &keyPath)
{
    plist::Adjustment adjustment = plist::Adjustment(plist::Adjustment::Type::Extract, keyPath, nullptr);
    return plist::PerformAdjustment(readObject, adjustment);
}

plist::Object *
plist::Replace(plist::Object *readObject, std::string const &keyPath, std::unique_ptr<plist::Object const> value)
{
    plist::Adjustment adjustment = plist::Adjustment(plist::Adjustment::Type::Replace, keyPath, std::move(value));
    return plist::PerformAdjustment(readObject, adjustment); 
}

plist::Object *
plist::Insert(plist::Object *readObject, std::string const &keyPath, std::unique_ptr<plist::Object const> value)
{
    plist::Adjustment adjustment = plist::Adjustment(plist::Adjustment::Type::Insert, keyPath, std::move(value));
    return plist::PerformAdjustment(readObject, adjustment); 
}

plist::Object *
plist::Remove(plist::Object *readObject, std::string const &keyPath)
{
    plist::Adjustment adjustment = plist::Adjustment(plist::Adjustment::Type::Remove, keyPath, nullptr);
    return plist::PerformAdjustment(readObject, adjustment);
}
