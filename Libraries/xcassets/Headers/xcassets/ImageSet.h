/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef __xcassets_ImageSet_h
#define __xcassets_ImageSet_h

#include <xcassets/Asset.h>
#include <xcassets/Idiom.h>
#include <xcassets/Insets.h>
#include <xcassets/Resizing.h>
#include <plist/Dictionary.h>

#include <memory>
#include <string>
#include <vector>
#include <ext/optional>

namespace xcassets {

class ImageSet : public Asset {
public:
    class Image {
    private:
        ext::optional<std::string> _fileName;
        ext::optional<bool>        _unassigned;

    private:
        // TODO: graphics-feature-set
        ext::optional<Idiom>       _idiom;
        // TODO: memory
        // TODO: scale
        // TODO: subtype
        // TODO: screen-width
        // TODO: width-class
        // TODO: height-class

    private:
        ext::optional<Insets>      _alignmentInsets;
        ext::optional<Resizing>    _resizing;

    public:
        ext::optional<std::string> const &fileName() const
        { return _fileName; }
        bool unassigned() const
        { return _unassigned.value_or(false); }
        ext::optional<bool> const &unassignedOptional() const
        { return _unassigned; }

    public:
        // TODO: graphics-feature-set
        ext::optional<Idiom> const &idiom() const
        { return _idiom; }
        // TODO: memory
        // TODO: scale
        // TODO: subtype
        // TODO: screen-width
        // TODO: width-class
        // TODO: height-class

    public:
        ext::optional<Insets> const &alignmentInsets() const
        { return _alignmentInsets; }
        ext::optional<Resizing> const &resizing() const
        { return _resizing; }

    private:
        friend class ImageSet;
        bool parse(plist::Dictionary const *dict);
    };

private:
    ext::optional<std::vector<std::string>> _onDemandResourceTags;
    ext::optional<std::vector<Image>>       _images;

public:
    ext::optional<std::vector<std::string>> const &onDemandResourceTags() const
    { return _onDemandResourceTags; }
    ext::optional<std::vector<Image>> const &images() const
    { return _images; }

public:
    static AssetType Type()
    { return AssetType::ImageSet; }
    virtual AssetType type()
    { return AssetType::ImageSet; }

public:
    static ext::optional<std::string> Extension()
    { return std::string("imageset"); }

protected:
    virtual bool parse(plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check);
};

}

#endif // !__xcassets_ImageSet_h
