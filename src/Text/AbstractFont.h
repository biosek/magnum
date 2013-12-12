#ifndef Magnum_Text_AbstractFont_h
#define Magnum_Text_AbstractFont_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

/** @file
 * @brief Class @ref Magnum::Text::AbstractFont, @ref Magnum::Text::AbstractLayouter
 */

#include <memory>
#include <string>
#include <tuple>
#include <PluginManager/AbstractPlugin.h>

#include "Magnum.h"
#include "Texture.h"
#include "Text/Text.h"
#include "Text/magnumTextVisibility.h"

namespace Magnum { namespace Text {

/**
@brief Base for font plugins

Provides interface for opening fonts, filling glyph cache and layouting the
glyphs. See @ref plugins for more information and `*Font` classes in @ref Text
namespace for available font plugins.

@section AbstractFont-usage Usage

First step is to open the font using @ref openData(), @ref openSingleData() or
@ref openFile(). Next step is to prerender all the glyphs which will be used in
text rendering later, see @ref GlyphCache for more information. See
@ref Renderer for information about text rendering.

@section AbstractFont-subclassing Subclassing

Plugin implements @ref doFeatures(), @ref doClose(), @ref doLayout(), either
@ref doCreateGlyphCache() or @ref doFillGlyphCache() and one or more of
`doOpen*()` functions. See also @ref AbstractLayouter for more information.

You don't need to do most of the redundant sanity checks, these things are
checked by the implementation:

-   Functions @ref doOpenData(), @ref doOpenSingleData() and @ref doOpenFile()
    are called after the previous file was closed, function @ref doClose() is
    called only if there is any file opened.
-   Functions @ref doOpenData() and @ref doOpenSingleData() are called only if
    @ref Feature::OpenData is supported.
-   All `do*()` implementations working on opened file are called only if
    there is any file opened.
*/
class MAGNUM_TEXT_EXPORT AbstractFont: public PluginManager::AbstractPlugin {
    CORRADE_PLUGIN_INTERFACE("cz.mosra.magnum.Text.AbstractFont/0.2.3")

    public:
        /**
         * @brief Features supported by this importer
         *
         * @see @ref Features, @ref features()
         */
        enum class Feature: UnsignedByte {
            /** Opening fonts from raw data using @ref openData() */
            OpenData = 1 << 0,

            /**
             * The format is multi-file, thus @ref openSingleData() convenience
             * function cannot be used.
             */
            MultiFile = 1 << 1,

            /**
             * The font contains prepared glyph cache.
             *
             * @see @ref fillGlyphCache(), @ref createGlyphCache()
             */
            PreparedGlyphCache = 1 << 2
        };

        /** @brief Set of features supported by this importer */
        typedef Containers::EnumSet<Feature, UnsignedByte> Features;

        /** @brief Default constructor */
        explicit AbstractFont();

        /** @brief Plugin manager constructor */
        explicit AbstractFont(PluginManager::AbstractManager* manager, std::string plugin);

        /** @brief Features supported by this font */
        Features features() const { return doFeatures(); }

        /** @brief Whether any file is opened */
        bool isOpened() const { return doIsOpened(); }

        /**
         * @brief Open font from raw data
         * @param data          Pairs of filename and file data
         * @param size          Font size
         *
         * Closes previous file, if it was opened, and tries to open given
         * file. Available only if @ref Feature::OpenData is supported. Returns
         * `true` on success, `false` otherwise.
         */
        bool openData(const std::vector<std::pair<std::string, Containers::ArrayReference<const unsigned char>>>& data, Float size);

        /**
         * @brief Open font from single data
         * @param data          File data
         * @param size          Font size
         *
         * Closes previous file, if it was opened, and tries to open given
         * file. Available only if @ref Feature::OpenData is supported and the
         * plugin doesn't have @ref Feature::MultiFile. Returns `true` on
         * success, `false` otherwise.
         */
        bool openSingleData(Containers::ArrayReference<const unsigned char> data, Float size);

        /**
         * @brief Open font from file
         * @param filename      Font file
         * @param size          Font size
         *
         * Closes previous file, if it was opened, and tries to open given
         * file. If the plugin has @ref Feature::MultiFile, the function will
         * use additional files in given path, all sharing common basename
         * derived from @p filename. Returns `true` on success, `false`
         * otherwise.
         */
        bool openFile(const std::string& filename, Float size);

        /** @brief Close font */
        void close();

        /** @brief Font size */
        Float size() const { return _size; }

        /** @brief Line height */
        Float lineHeight() const { return _lineHeight; }

        /**
         * @brief Glyph ID for given character
         *
         * @note This function is not meant to be used in performance-critical
         *      code, only for font observations and conversions.
         */
        UnsignedInt glyphId(char32_t character);

        /**
         * @brief Glyph advance
         * @param glyph     Glyph ID
         *
         * @note This function is not meant to be used in performance-critical
         *      code, only for font observations and conversions.
         * @see @ref glyphId()
         */
        Vector2 glyphAdvance(UnsignedInt glyph);

        /**
         * @brief Fill glyph cache with given character set
         * @param cache         Glyph cache instance
         * @param characters    UTF-8 characters to render
         *
         * Fills the cache with given characters. Fonts having
         * @ref Feature::PreparedGlyphCache do not support partial glyph cache
         * filling, use @ref createGlyphCache() instead.
         */
        void fillGlyphCache(GlyphCache& cache, const std::string& characters);

        /**
         * @brief Create glyph cache
         *
         * Configures and fills glyph cache with the contents of whole font.
         * Available only if @ref Feature::PreparedGlyphCache is supported.
         * Other fonts support only partial glyph cache filling, see
         * @ref fillGlyphCache().
         */
        std::unique_ptr<GlyphCache> createGlyphCache();

        /**
         * @brief Layout the text using font's own layouter
         * @param cache     Glyph cache
         * @param size      Font size
         * @param text      %Text to layout
         *
         * Note that the layouters support rendering of single-line text only.
         * See @ref Renderer class for more advanced text layouting.
         * @see @ref fillGlyphCache(), @ref createGlyphCache()
         */
        std::unique_ptr<AbstractLayouter> layout(const GlyphCache& cache, Float size, const std::string& text);

    #ifdef DOXYGEN_GENERATING_OUTPUT
    protected:
    #else
    private:
    #endif
        /** @brief Implementation for @ref features() */
        virtual Features doFeatures() const = 0;

        /** @brief Implementation for @ref isOpened() */
        virtual bool doIsOpened() const = 0;

        /**
         * @brief Implementation for @ref openData()
         *
         * Return size and line height of opened font on successful opening,
         * zeros otherwise. If the plugin doesn't have @ref Feature::MultiFile,
         * default implementation calls @ref doOpenSingleData().
         */
        virtual std::pair<Float, Float> doOpenData(const std::vector<std::pair<std::string, Containers::ArrayReference<const unsigned char>>>& data, Float size);

        /**
         * @brief Implementation for @ref openSingleData()
         *
         * Return size and line height of opened font on successful opening,
         * zeros otherwise.
         */
        virtual std::pair<Float, Float> doOpenSingleData(Containers::ArrayReference<const unsigned char> data, Float size);

        /**
         * @brief Implementation for @ref openFile()
         *
         * Return size and line height of opened font on successful opening,
         * zeros otherwise. If @ref Feature::OpenData is supported and the
         * plugin doesn't have @ref Feature::MultiFile, default implementation
         * opens the file and calls @ref doOpenSingleData() with its contents.
         */
        virtual std::pair<Float, Float> doOpenFile(const std::string& filename, Float size);

        /** @brief Implementation for @ref close() */
        virtual void doClose() = 0;

        /** @brief Implementation for @ref glyphId() */
        virtual UnsignedInt doGlyphId(char32_t character) = 0;

        /** @brief Implementation for @ref glyphAdvance() */
        virtual Vector2 doGlyphAdvance(UnsignedInt glyph) = 0;

        /**
         * @brief Implementation for @ref fillGlyphCache()
         *
         * The string is converted from UTF-8 to UTF-32, unique characters are
         * *not* removed.
         * @note On Windows uses `std::vector<char32_t>` instead of
         *      `std::u32string`. See @ref Corrade::Utility::Unicode::utf32()
         *      for more information.
         */
        #ifndef __MINGW32__
        virtual void doFillGlyphCache(GlyphCache& cache, const std::u32string& characters);
        #else
        virtual void doFillGlyphCache(GlyphCache& cache, const std::vector<char32_t>& characters);
        #endif

        /** @brief Implementation for @ref createGlyphCache() */
        virtual std::unique_ptr<GlyphCache> doCreateGlyphCache();

        /** @brief Implementation for @ref layout() */
        virtual std::unique_ptr<AbstractLayouter> doLayout(const GlyphCache& cache, Float size, const std::string& text) = 0;

    #ifdef DOXYGEN_GENERATING_OUTPUT
    private:
    #endif
        Float _size, _lineHeight;
};

CORRADE_ENUMSET_OPERATORS(AbstractFont::Features)

/**
@brief Base for text layouters

Returned by @ref AbstractFont::layout().

@section TextAbstractLayouter-subclassing Subclassing

Plugin creates private subclass (no need to expose it to end users) and
implements @ref doRenderGlyph(). Bounds checking on @p i is done automatically
in the wrapping @ref renderGlyph() function.
*/
class MAGNUM_TEXT_EXPORT AbstractLayouter {
    AbstractLayouter(const AbstractLayouter&) = delete;
    AbstractLayouter(AbstractLayouter&&) = delete;
    AbstractLayouter& operator=(const AbstractLayouter&) = delete;
    AbstractLayouter& operator=(const AbstractLayouter&&) = delete;

    public:
        ~AbstractLayouter();

        /** @brief Count of glyphs in laid out text */
        UnsignedInt glyphCount() const { return _glyphCount; }

        /**
         * @brief Render glyph
         * @param i                 Glyph index
         * @param cursorPosition    Cursor position
         * @param rectangle         Bounding rectangle
         *
         * The function returns pair of quad position and texture coordinates,
         * advances @p cursorPosition to next character and updates @p rectangle
         * with extended bounds.
         */
        std::pair<Range2D, Range2D> renderGlyph(UnsignedInt i, Vector2& cursorPosition, Range2D& rectangle);

    protected:
        /**
         * @brief Constructor
         * @param glyphCount    Count of glyphs in laid out text
         */
        explicit AbstractLayouter(UnsignedInt glyphCount);

    #ifdef DOXYGEN_GENERATING_OUTPUT
    protected:
    #else
    private:
    #endif
        /**
         * @brief Implementation for @ref renderGlyph()
         * @param i                 Glyph index
         *
         * Return quad position (relative to current cursor position), texture
         * coordinates and advance to next glyph.
         */
        virtual std::tuple<Range2D, Range2D, Vector2> doRenderGlyph(UnsignedInt i) = 0;

    #ifdef DOXYGEN_GENERATING_OUTPUT
    private:
    #endif
        UnsignedInt _glyphCount;
};

}}

#endif
