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

#include "AbstractFont.h"

#include <Containers/Array.h>
#include <Utility/Directory.h>
#include <Utility/Unicode.h>

#include "Text/GlyphCache.h"

namespace Magnum { namespace Text {

AbstractFont::AbstractFont(): _size(0.0f) {}

AbstractFont::AbstractFont(PluginManager::AbstractManager* manager, std::string plugin): AbstractPlugin(manager, std::move(plugin)), _size(0.0f), _lineHeight(0.0f) {}

bool AbstractFont::openData(const std::vector<std::pair<std::string, Containers::ArrayReference<const unsigned char>>>& data, const Float size) {
    CORRADE_ASSERT(features() & Feature::OpenData,
        "Text::AbstractFont::openData(): feature not supported", false);
    CORRADE_ASSERT(!data.empty(),
        "Text::AbstractFont::openData(): no data passed", false);

    close();
    std::tie(_size, _lineHeight) = doOpenData(data, size);
    CORRADE_INTERNAL_ASSERT(isOpened() || (_size == 0.0f && _lineHeight == 0.0f));
    return isOpened();
}

std::pair<Float, Float> AbstractFont::doOpenData(const std::vector<std::pair<std::string, Containers::ArrayReference<const unsigned char>>>& data, const Float size) {
    CORRADE_ASSERT(!(features() & Feature::MultiFile),
        "Text::AbstractFont::openData(): feature advertised but not implemented", {});
    CORRADE_ASSERT(data.size() == 1,
        "Text::AbstractFont::openData(): expected just one file for single-file format", {});

    close();
    return doOpenSingleData(data[0].second, size);
}

bool AbstractFont::openSingleData(const Containers::ArrayReference<const unsigned char> data, const Float size) {
    CORRADE_ASSERT(features() & Feature::OpenData,
        "Text::AbstractFont::openSingleData(): feature not supported", false);
    CORRADE_ASSERT(!(features() & Feature::MultiFile),
        "Text::AbstractFont::openSingleData(): the format is not single-file", false);

    close();
    std::tie(_size, _lineHeight) = doOpenSingleData(data, size);
    CORRADE_INTERNAL_ASSERT(isOpened() || (_size == 0.0f && _lineHeight == 0.0f));
    return isOpened();
}

std::pair<Float, Float> AbstractFont::doOpenSingleData(Containers::ArrayReference<const unsigned char>, Float) {
    CORRADE_ASSERT(false, "Text::AbstractFont::openSingleData(): feature advertised but not implemented", {});
}

bool AbstractFont::openFile(const std::string& filename, const Float size) {
    close();
    std::tie(_size, _lineHeight) = doOpenFile(filename, size);
    CORRADE_INTERNAL_ASSERT(isOpened() || (_size == 0.0f && _lineHeight == 0.0f));
    return isOpened();
}

std::pair<Float, Float> AbstractFont::doOpenFile(const std::string& filename, const Float size) {
    CORRADE_ASSERT(features() & Feature::OpenData && !(features() & Feature::MultiFile),
        "Text::AbstractFont::openFile(): not implemented", {});

    /* Open file */
    if(!Utility::Directory::fileExists(filename)) {
        Error() << "Trade::AbstractFont::openFile(): cannot open file" << filename;
        return {};
    }

    return doOpenSingleData(Utility::Directory::read(filename), size);
}

void AbstractFont::close() {
    if(isOpened()) {
        doClose();
        _size = 0.0f;
        _lineHeight = 0.0f;
        CORRADE_INTERNAL_ASSERT(!isOpened());
    }
}

UnsignedInt AbstractFont::glyphId(const char32_t character) {
    CORRADE_ASSERT(isOpened(), "Text::AbstractFont::glyphId(): no font opened", 0);

    return doGlyphId(character);
}

Vector2 AbstractFont::glyphAdvance(const UnsignedInt glyph) {
    CORRADE_ASSERT(isOpened(), "Text::AbstractFont::glyphAdvance(): no font opened", {});

    return doGlyphAdvance(glyph);
}

void AbstractFont::fillGlyphCache(GlyphCache& cache, const std::string& characters) {
    CORRADE_ASSERT(isOpened(),
        "Text::AbstractFont::createGlyphCache(): no font opened", );
    CORRADE_ASSERT(!(features() & Feature::PreparedGlyphCache),
        "Text::AbstractFont::fillGlyphCache(): feature not supported", );

    doFillGlyphCache(cache, Utility::Unicode::utf32(characters));
}

#ifndef __MINGW32__
void AbstractFont::doFillGlyphCache(GlyphCache&, const std::u32string&)
#else
void AbstractFont::doFillGlyphCache(GlyphCache&, const std::vector<char32_t>&)
#endif
{
    CORRADE_ASSERT(false, "Text::AbstractFont::fillGlyphCache(): feature advertised but not implemented", );
}

std::unique_ptr<GlyphCache> AbstractFont::createGlyphCache() {
    CORRADE_ASSERT(isOpened(),
        "Text::AbstractFont::createGlyphCache(): no font opened", {});
    CORRADE_ASSERT(features() & Feature::PreparedGlyphCache,
        "Text::AbstractFont::createGlyphCache(): feature not supported", {});

    return doCreateGlyphCache();
}

std::unique_ptr<GlyphCache> AbstractFont::doCreateGlyphCache() {
    CORRADE_ASSERT(false, "Text::AbstractFont::createGlyphCache(): feature advertised but not implemented", {});
}

std::unique_ptr<AbstractLayouter> AbstractFont::layout(const GlyphCache& cache, const Float size, const std::string& text) {
    CORRADE_ASSERT(isOpened(), "Text::AbstractFont::layout(): no font opened", {});

    return doLayout(cache, size, text);
}

AbstractLayouter::AbstractLayouter(UnsignedInt glyphCount): _glyphCount(glyphCount) {}

AbstractLayouter::~AbstractLayouter() {}

std::pair<Range2D, Range2D> AbstractLayouter::renderGlyph(const UnsignedInt i, Vector2& cursorPosition, Range2D& rectangle) {
    CORRADE_ASSERT(i < glyphCount(), "Text::AbstractLayouter::renderGlyph(): glyph index out of bounds", {});

    /* Render the glyph */
    Range2D quadPosition, textureCoordinates;
    Vector2 advance;
    std::tie(quadPosition, textureCoordinates, advance) = doRenderGlyph(i);

    /* Move the quad to cursor */
    quadPosition.bottomLeft() += cursorPosition;
    quadPosition.topRight() += cursorPosition;

    /* Extend rectangle with current quad bounds. If zero size, replace it. */
    if(!rectangle.size().isZero()) {
        rectangle.bottomLeft() = Math::min(rectangle.bottomLeft(), quadPosition.bottomLeft());
        rectangle.topRight() = Math::max(rectangle.topRight(), quadPosition.topRight());
    } else rectangle = quadPosition;

    /* Advance cursor position to next character */
    cursorPosition += advance;

    /* Return moved quad and unchanged texture coordinates */
    return {quadPosition, textureCoordinates};
}

}}
