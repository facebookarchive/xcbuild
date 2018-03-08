/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <libutil/Relative.h>
#include <libutil/Absolute.h>
#include <libutil/Unix.h>
#include <libutil/Windows.h>

#include <algorithm>
#include <cassert>

#if _WIN32
#include <cstring>
#else
#include <strings.h>
#endif

namespace Path = libutil::Path;

template<class Traits>
Path::BaseRelative<Traits>::
BaseRelative(std::string const &raw) :
    _raw(raw)
{
}

static std::vector<std::string>
NormalizePathComponents(
    std::string const &path,
    std::string::const_iterator start,
    std::string::const_iterator end,
    bool absolute,
    bool collapse,
    char separator,
    bool(*isSeparator)(char c))
{
    std::vector<std::string> output;
    std::string component;
    bool finished = start == end;

    for (auto it = start; !finished; ++it) {
        finished = it == end;

        if (finished || isSeparator(*it)) {
            bool isNewComponent = false, isPreviousComponent = false;
            if (!component.empty()) {
                if (!collapse) {
                    isNewComponent = true;
                } else if (component == "..") {
                    isPreviousComponent = true;
                } else if (component != ".") {
                    isNewComponent = true;
                }
            }

            /* Either there's a new valid component to add to the list... */
            if (isNewComponent) {
                output.push_back(component);
            } else if (isPreviousComponent) {
                /* ...or a component needs to be removed from the output.
                   In cases where a component shouldn't be removed (a parent
                   representative or an empty output) just add a token representing
                   a directory up. */
                if (!absolute && (output.empty() || output.back() == "..")) {
                    /* Initial parent. Copy to output if relative. */
                    output.push_back("..");
                } else if (!output.empty()) {
                    output.pop_back();
                }
            }

            for (; it != end; ++it) {
                auto candidate = std::next(it);
                if (candidate == end || !isSeparator(*candidate)) {
                    break;
                }
            }

            finished = it == end;
            component.clear();
        } else {
            component += *it;
        }
    }

    return output;
}

template<class Traits>
std::vector<std::string> Path::BaseRelative<Traits>::
normalizedComponents() const
{
    std::string prefix;
    std::vector<std::string> output;

    size_t start;
    bool absolute = Traits::IsAbsolute(_raw, &start);
    for (std::string::size_type n = 0; n < start; ++n) {
        prefix += (Traits::IsSeparator(_raw[n]) ? Traits::Separator : _raw[n]);
    }
    if (!prefix.empty()) {
        output.push_back(prefix);
    }

    std::vector<std::string> relativeOutput = NormalizePathComponents(
        _raw,
        _raw.begin() + start,
        _raw.end(),
        absolute,
        true,
        Traits::Separator,
        &Traits::IsSeparator
    );

    output.insert(output.end(), relativeOutput.begin(), relativeOutput.end());
    return output;
}

template<class Traits>
std::string Path::BaseRelative<Traits>::
normalized() const
{
    std::vector<std::string> components = this->normalizedComponents();
    std::string output;

    size_t start;
    Traits::IsAbsolute(_raw, &start);
    bool hasPrefix = start > 0;
    auto end = components.end();
    auto it = components.begin();

    /* Skip past the initial absolute prefix if needed; it doesn't need to be
       split from the rest of the path with a separator. */
    if (it != end && hasPrefix) {
        output += *it;
        ++it;
    }

    for (; it != end; ++it) {
        output += *it;
        if (std::next(it) != end) {
          output += Traits::Separator;
        }
    }

    return output;
}

template<class Traits>
ext::optional<Path::BaseAbsolute<Traits>> Path::BaseRelative<Traits>::
absolute() const
{
    if (Traits::IsAbsolute(_raw)) {
        return Path::BaseAbsolute<Traits>(*this);
    } else {
        return ext::nullopt;
    }
}

template<class Traits>
Path::BaseAbsolute<Traits> Path::BaseRelative<Traits>::
resolved(Path::BaseAbsolute<Traits> const &against) const
{
    if (ext::optional<Path::BaseAbsolute<Traits>> absolute = this->absolute()) {
        return *absolute;
    } else {
        std::string base;
        std::string relative;
        if (!Traits::Resolve(_raw, against.raw(), &base, &relative)) {
            abort();
        }

        if (auto absolute = Path::BaseAbsolute<Traits>::Create(base)) {
            return absolute->child(relative);
        } else {
            /* Should always be absolute... */
            abort();
        }
    }
}

template<class Traits>
Path::BaseRelative<Traits> Path::BaseRelative<Traits>::
parent() const
{
    size_t start;
    (void)Traits::IsAbsolute(_raw, &start);

    auto begin = (_raw.size() > start && Path::Windows::IsSeparator(_raw.back()) ? std::next(_raw.rbegin()) : _raw.rbegin());
    auto end = std::prev(_raw.rend(), start);
    auto it = std::find_if(begin, end, &Path::Windows::IsSeparator);

    if (it == end) {
        return Path::BaseRelative<Traits>(std::string(_raw.begin(), end.base()));
    } else {
        return Path::BaseRelative<Traits>(std::string(_raw.begin(), std::prev(it.base())));
    }
}

template<class Traits>
Path::BaseRelative<Traits> Path::BaseRelative<Traits>::
child(std::string const &name) const
{
    if (_raw.empty()) {
        return Path::BaseRelative<Traits>(name);
    } else if (name.empty()) {
        return Path::BaseRelative<Traits>(_raw);
    }

    if (Traits::IsSeparator(_raw.back())) {
        /* Already has a trailing separator, don't duplicate. */
        return Path::BaseRelative<Traits>(_raw + name);
    } else {
        return Path::BaseRelative<Traits>(_raw + Traits::Separator + name);
    }
}

template<class Traits>
std::string Path::BaseRelative<Traits>::
base(bool extension) const
{
    size_t start;
    (void)Traits::IsAbsolute(_raw, &start);

    std::string base;
    auto sit = std::prev(_raw.rend(), start);
    auto it = std::find_if(_raw.rbegin(), sit, &Traits::IsSeparator);

    if (it == sit) {
        /* Remove root even if no separators. */
        base = _raw.substr(start);
    } else {
        /* Remove up to the last separator. */
        base = std::string(it.base(), _raw.end());
    }

    if (!extension) {
        size_t pos = base.rfind('.');
        if (pos != std::string::npos) {
            base = base.substr(0, pos);
        }
    }

    return base;
}

template<class Traits>
std::string Path::BaseRelative<Traits>::
extension() const
{
    std::string base = this->base();
    size_t pos = base.rfind('.');
    if (pos == std::string::npos) {
        return std::string();
    }

    return base.substr(pos + 1);
}

template<class Traits>
bool Path::BaseRelative<Traits>::
extension(std::string const &extension, bool insensitive) const
{
    std::string pathExtension = this->extension();

    if (insensitive) {
#if _WIN32
        return ::_stricmp(pathExtension.c_str(), extension.c_str()) == 0;
#else
        return ::strcasecmp(pathExtension.c_str(), extension.c_str()) == 0;
#endif
    } else {
        return pathExtension == extension;
    }
}

namespace libutil { namespace Path { template class BaseRelative<Unix>; } }
namespace libutil { namespace Path { template class BaseRelative<Windows>; } }
