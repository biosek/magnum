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

#include "Atlas.h"

#include "Math/Functions.h"
#include "Math/Geometry/Rectangle.h"

namespace Magnum { namespace TextureTools {

Atlas::Atlas()
{
}

Atlas::Atlas(Vector2i size)
{
    init(size);
}

void Atlas::init(Vector2i size)
{
    _size=size;
    _usedRectangles.clear();
    _freeRectangles.clear();
    _freeRectangles.push_back(Rectanglei::fromSize({0,0}, size));
}

Rectanglei Atlas::insert(Vector2i size)
{
    Rectanglei newNode;
    int score1, score2;
    newNode = findPositionForNewNodeBestShortSideFit(size, score1, score2);

    if(newNode.height() == 0)
        return newNode;

    size_t numRectanglesToProcess = _freeRectangles.size();
    for(size_t i = 0; i < numRectanglesToProcess; ++i)
    {
        if(splitFreeNode(_freeRectangles[i], newNode))
        {
            _freeRectangles.erase((_freeRectangles.begin() + i));
            --i;
            --numRectanglesToProcess;
        }
    }

    pruneFreeList();

    _usedRectangles.push_back(newNode);
    return newNode;
}

void Atlas::insert(std::vector<Vector2i> &rects, std::vector<Rectanglei> &dst)
{
    dst.clear();

    while(rects.size() > 0)
    {
        int bestScore1 = std::numeric_limits<int>::max();
        int bestScore2 = std::numeric_limits<int>::max();
        int bestRectIndex = -1;
        Rectanglei bestNode;

        size_t rectsSize = rects.size();
        for(size_t i = 0; i < rectsSize; ++i)
        {
            int score1;
            int score2;
            Rectanglei newNode = scoreRect(rects[i], score1, score2);

            if(score1 < bestScore1 || (score1 == bestScore1 && score2 < bestScore2))
            {
                bestScore1 = score1;
                bestScore2 = score2;
                bestNode = newNode;
                bestRectIndex = i;
            }
        }

        if(bestRectIndex == -1)
            return;

        placeRect(bestNode);
        rects.erase(rects.begin() + bestRectIndex);
    }
}

Rectanglei Atlas::scoreRect(Vector2i size, int &score1, int &score2) const
{
    Rectanglei newNode;
    score1 = std::numeric_limits<int>::max();
    score2 = std::numeric_limits<int>::max();

    newNode = findPositionForNewNodeBestShortSideFit(size, score1, score2);

    if(newNode.height() == 0)
    {
        score1 = std::numeric_limits<int>::max();
        score2 = std::numeric_limits<int>::max();
    }

    return newNode;
}

void Atlas::placeRect(const Rectanglei &node)
{
    size_t numRectanglesToProcess = _freeRectangles.size();
    for(size_t i = 0; i < numRectanglesToProcess; ++i)
    {
        if(splitFreeNode(_freeRectangles[i], node))
        {
            _freeRectangles.erase(_freeRectangles.begin() + i);
            --i;
            --numRectanglesToProcess;
        }
    }

    pruneFreeList();

    _usedRectangles.push_back(node);
}

Rectanglei Atlas::findPositionForNewNodeBestShortSideFit(Vector2i size, int &bestShortSideFit, int &bestLongSideFit) const
{
    Rectanglei bestNode;
    bestShortSideFit = std::numeric_limits<int>::max();

    size_t freeRecSize = _freeRectangles.size();
    for(size_t i = 0; i < freeRecSize; ++i)
    {
        if(_freeRectangles[i].width() >= size.x() && _freeRectangles[i].height() >= size.y())
        {
            int leftoverHoriz = Math::abs(_freeRectangles[i].width() - size.x());
            int leftoverVert = Math::abs(_freeRectangles[i].height() - size.y());
            int shortSideFit = Math::min(leftoverHoriz, leftoverVert);
            int longSideFit = Math::max(leftoverHoriz, leftoverVert);

            if (shortSideFit < bestShortSideFit || (shortSideFit == bestShortSideFit && longSideFit < bestLongSideFit))
            {
                bestNode = Rectanglei::fromSize(_freeRectangles[i].bottomLeft(), size);
                bestShortSideFit = shortSideFit;
                bestLongSideFit = longSideFit;
            }
        }

        if(_freeRectangles[i].width() >= size.y() && _freeRectangles[i].height() >= size.x())
        {
            int flippedLeftoverHoriz = Math::abs(_freeRectangles[i].width() - size.y());
            int flippedLeftoverVert = Math::abs(_freeRectangles[i].height() - size.x());
            int flippedShortSideFit = Math::min(flippedLeftoverHoriz, flippedLeftoverVert);
            int flippedLongSideFit = Math::max(flippedLeftoverHoriz, flippedLeftoverVert);

            if (flippedShortSideFit < bestShortSideFit || (flippedShortSideFit == bestShortSideFit && flippedLongSideFit < bestLongSideFit))
            {
                bestNode = Rectanglei::fromSize(_freeRectangles[i].bottomLeft(), Vector2i(size.y(),size.x()));
                bestShortSideFit = flippedShortSideFit;
                bestLongSideFit = flippedLongSideFit;
            }
        }
    }

    return bestNode;
}

bool Atlas::splitFreeNode(Rectanglei freeNode, const Rectanglei &usedNode)
{
    // Test with SAT if the rectangles even intersect.
    if (usedNode.left() >= freeNode.right() || usedNode.right() <= freeNode.left() ||
            usedNode.bottom() >= freeNode.top() || usedNode.top() <= freeNode.bottom())
        return false;

    if (usedNode.left() < freeNode.right() && usedNode.right() > freeNode.left())
    {
        // New node at the top side of the used node.
        if (usedNode.bottom() > freeNode.bottom() && usedNode.bottom() < freeNode.top())
            _freeRectangles.push_back(Rectanglei::fromSize(freeNode.bottomLeft(), Vector2i(freeNode.width(), usedNode.bottom() - freeNode.bottom())));

        // New node at the bottom side of the used node.
        if (usedNode.bottom() + usedNode.height() < freeNode.bottom() + freeNode.height())
            _freeRectangles.push_back(Rectanglei::fromSize(Vector2i(freeNode.left(), usedNode.top()), Vector2i(freeNode.width(), freeNode.top() - usedNode.top())));
    }

    if (usedNode.bottom() < freeNode.top() && usedNode.top() > freeNode.bottom())
    {
        // New node at the left side of the used node.
        if (usedNode.left() > freeNode.left() && usedNode.left() < freeNode.right())
            _freeRectangles.push_back(Rectanglei::fromSize(freeNode.bottomLeft(), Vector2i(usedNode.left() - freeNode.left(), freeNode.height())));

        // New node at the right side of the used node.
        if (usedNode.left() + usedNode.width() < freeNode.left() + freeNode.width())
            _freeRectangles.push_back(Rectanglei::fromSize(Vector2i(usedNode.right(), freeNode.bottom()), Vector2i(freeNode.right() - usedNode.right(), freeNode.height())));
    }

    return true;
}

void Atlas::pruneFreeList()
{
    /// Go through each pair and remove any rectangle that is redundant.
    size_t freeRecSize = _freeRectangles.size();
    for(size_t i = 0; i < freeRecSize; ++i)
        for(size_t j = i+1; j < freeRecSize; ++j)
        {
            if (isRectContainedIn(_freeRectangles[i], _freeRectangles[j]))
            {
                _freeRectangles.erase(_freeRectangles.begin()+i);
                --i;
                break;
            }
            if (isRectContainedIn(_freeRectangles[j], _freeRectangles[i]))
            {
                _freeRectangles.erase(_freeRectangles.begin()+j);
                --j;
            }
        }
}

int Atlas::intervalIntersection(int x1, int y1, int &x2, int &y2) const
{
    if (y1 < x1 || y2 < x1)
        return 0;

    return Math::min(y1, y2) - Math::max(x1, x2);
}

bool Atlas::isRectContainedIn(const Rectanglei &a, const Rectanglei &b)
{
    return a.left() >= b.left() && a.bottom() >= b.bottom() && a.right() <= b.right() && a.top() <= b.top();
}

std::vector<Rectanglei> atlas(const Vector2i& atlasSize, const std::vector<Vector2i>& sizes, const Vector2i& padding) {
    if(sizes.empty()) return {};

    /* Size of largest texture */
    Vector2i maxSize;
    for(const Vector2i& size: sizes)
        maxSize = Math::max(maxSize, size);

    std::vector<Rectanglei> atlas;

    /* Columns and rows */
    const Vector2i paddedSize = maxSize+2*padding;
    const Vector2i gridSize = atlasSize/paddedSize;
    if(std::size_t(gridSize.product()) < sizes.size()) {
        Error() << "TextureTools::atlas(): requested atlas size" << atlasSize
                << "is too small to fit" << sizes.size() << paddedSize
                << "textures. Generated atlas will be empty.";
        return atlas;
    }

    /** @todo actual magic implementation, not this joke */

    atlas.reserve(sizes.size());
    for(std::size_t i = 0; i != sizes.size(); ++i)
        atlas.push_back(Rectanglei::fromSize(Vector2i(i%gridSize.x(), i/gridSize.x())*paddedSize+padding, sizes[i]));

    return atlas;
}

}}
