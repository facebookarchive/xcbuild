/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef __xcassets_Asset_Asset_h
#define __xcassets_Asset_Asset_h

#include <xcassets/Asset/AssetType.h>
#include <xcassets/FullyQualifiedName.h>
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
    FullyQualifiedName         _name;
    std::string                _path;

private:
    ext::optional<std::string> _author;
    ext::optional<int>         _version;

protected:
    Asset(FullyQualifiedName const &name, std::string const &path);
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
    FullyQualifiedName const &name()
    { return _name; }

public:
    ext::optional<std::string> const &author()
    { return _author; }
    ext::optional<int> const &version()
    { return _version; }

public:
    /*
     * Load an asset from a directory.
     */
    static std::shared_ptr<Asset> Load(std::string const &path, std::vector<std::string> const &groups);

protected:
    /*
     * Override to parse the contents, which can be null.
     */
    virtual bool parse(plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check);

    /*
     * Iterate children of this asset and load them. If specified, must match types.
     */
    bool loadChildren(std::vector<std::shared_ptr<Asset>> *children, bool providesNamespace = false);

    /*
     * Load children of a specific type.
     */
    template<typename T>
    bool loadChildren(std::vector<std::shared_ptr<T>> *children, bool providesNamespace = false)
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

