/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef __xcassets_Asset_Asset_h
#define __xcassets_Asset_Asset_h

#include <xcassets/Asset/AssetType.h>
#include <plist/Dictionary.h>

#include <memory>
#include <set>
#include <string>
#include <unordered_set>
#include <ext/optional>

namespace xcassets {
namespace Asset {

class Asset {
private:
    std::string                _path;
    std::string                _name;
    ext::optional<std::string> _group;

private:
    ext::optional<std::string> _author;
    ext::optional<int>         _version;

protected:
    Asset();
    virtual ~Asset();

public:
    /*
     * The dynamic type of the asset.
     */
    virtual AssetType type() = 0;

public:
    /*
     * The path to the asset.
     */
    std::string const &path()
    { return _path; }

    /*
     * The name of the asset.
     */
    std::string const &name()
    { return _name; }

    /*
     * The group containing the asset.
     */
    ext::optional<std::string> const &group() const
    { return _group; }

    /*
     * The identifier of the asset within its group.
     */
    std::string identifier() const
    { return (_group ? *_group + "/" : "") + _name; }

public:
    ext::optional<std::string> const &author()
    { return _author; }
    ext::optional<int> const &version()
    { return _version; }

public:
    /*
     * Load an asset from a directory.
     */
    static std::shared_ptr<Asset> Load(std::string const &path, ext::optional<std::string> const &group);

protected:
    /*
     * Override to parse the contents, which can be null.
     */
    virtual bool parse(plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check);

    /*
     * Iterate children of this asset and load them. If specified, must match types.
     */
    bool loadChildren(std::vector<std::shared_ptr<Asset>> *children);

    /*
     * Load children of a specific type.
     */
    template<typename T>
    bool loadChildren(std::vector<std::shared_ptr<T>> *children)
    {
        std::vector<std::shared_ptr<Asset>> assets;
        if (!loadChildren(&assets)) {
            return false;
        }

        bool error = false;
        for (std::shared_ptr<Asset> const &asset : assets) {
            if (asset->type() == T::Type()) {
                auto child = std::static_pointer_cast<T>(asset);
                children->push_back(child);
            } else {
                error = true;
            }
        }
        return !error;
    }
};

}
}

#endif // !__xcassets_Asset_Asset_h

