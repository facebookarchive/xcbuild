/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef _LIBCAR_RENDITION_H
#define _LIBCAR_RENDITION_H

#include <car/AttributeList.h>
#include <ext/optional>

#include <string>
#include <functional>

namespace car {

class Reader;

/*
 * Represents a specific variant of a facet.
 */
class Rendition {
public:
    class Data {
    public:
        enum class Format {
            /*
             * BGRA pixels in order.
             */
            PremultipliedBGRA8,
            /*
             * Gray and alpha in order.
             */
            PremultipliedGA8,
            /*
             * Raw data.
             */
            Data,
        };

        /*
         * The size of a pixel in the format.
         */
        static size_t FormatSize(Format format);

    private:
        std::vector<uint8_t> _data;
        Format               _format;

    public:
        Data(std::vector<uint8_t> const &data, Format format);

    public:
        /*
         * The raw data.
         */
        std::vector<uint8_t> const &data() const
        { return _data; }
        std::vector<uint8_t> &data()
        { return _data; }

        /*
         * The pixel format of the data.
         */
        Format format() const
        { return _format; }
        Format &format()
        { return _format; }
    };

private:
    AttributeList  _attributes;

private:
    std::function<ext::optional<Data>(Rendition const *)> _data;

private:
    std::string _fileName;
    int         _width;
    int         _height;
    float       _scale;

private:
    Rendition(AttributeList const &attributes, std::function<ext::optional<Data>(Rendition const *)> const &data);

public:
    /*
     * The attributes that describe the rendition.
     */
    AttributeList const &attributes() const
    { return _attributes; }

public:
    /*
     * The file name of the rendition
     */
    std::string const &fileName() const
    { return _fileName; }
    std::string &fileName()
    { return _fileName; }

    /*
     * The width of the rendition, in pixels.
     */
    int width() const
    { return _width; }
    int &width()
    { return _width; }

    /*
     * The height of the rendition, in pixels.
     */
    int height() const
    { return _height; }
    int &height()
    { return _height; }

    /*
     * The scale of the rendition.
     */
    float scale() const
    { return _scale; }
    float &scale()
    { return _scale; }

public:
    /*
     * The rendition pixel data. May incur expensive decoding.
     */
    ext::optional<Data> decode() const;

public:
    /*
     * Dump a description of the rendition. For debugging.
     */
    void dump() const;

public:
    /*
     * Load an existing rendition matching the provided attributes.
     */
    static Rendition const Load(
        AttributeList const &attributes,
        struct car_rendition_value *value);

    /*
     * Create a new rendition with the given properties.
     */
    static Rendition Create(
        AttributeList const &attributes,
        std::function<ext::optional<Data>(Rendition const *)> const &data);
};

}

#endif /* _LIBCAR_RENDITION_H */

