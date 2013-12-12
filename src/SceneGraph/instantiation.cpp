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

#include "SceneGraph/AbstractFeature.hpp"
#include "SceneGraph/Animable.hpp"
#include "SceneGraph/Camera2D.hpp"
#include "SceneGraph/Camera3D.hpp"
#include "SceneGraph/DualComplexTransformation.h"
#include "SceneGraph/DualQuaternionTransformation.h"
#include "SceneGraph/FeatureGroup.hpp"
#include "SceneGraph/MatrixTransformation2D.h"
#include "SceneGraph/MatrixTransformation3D.h"
#include "SceneGraph/Object.hpp"
#include "SceneGraph/RigidMatrixTransformation2D.h"
#include "SceneGraph/RigidMatrixTransformation3D.h"
#include "SceneGraph/TranslationTransformation.h"

namespace Magnum { namespace SceneGraph {

/* On Windows the instantiations are already marked with extern template */
#ifndef _WIN32
#define MAGNUM_SCENEGRAPH_EXPORT_HPP MAGNUM_SCENEGRAPH_EXPORT
#else
#define MAGNUM_SCENEGRAPH_EXPORT_HPP
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
template class MAGNUM_SCENEGRAPH_EXPORT_HPP AbstractObject<2, Float>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP AbstractObject<3, Float>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP AbstractTransformation<2, Float>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP AbstractTransformation<3, Float>;

template class MAGNUM_SCENEGRAPH_EXPORT_HPP AbstractFeature<2, Float>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP AbstractFeature<3, Float>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP AbstractFeatureGroup<2, Float>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP AbstractFeatureGroup<3, Float>;

template class MAGNUM_SCENEGRAPH_EXPORT_HPP Animable<2, Float>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP Animable<3, Float>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP AnimableGroup<2, Float>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP AnimableGroup<3, Float>;

template class MAGNUM_SCENEGRAPH_EXPORT_HPP AbstractCamera<2, Float>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP AbstractCamera<3, Float>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP BasicCamera2D<Float>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP BasicCamera3D<Float>;

template class MAGNUM_SCENEGRAPH_EXPORT_HPP Object<BasicDualComplexTransformation<Float>>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP Object<BasicDualQuaternionTransformation<Float>>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP Object<BasicMatrixTransformation2D<Float>>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP Object<BasicMatrixTransformation3D<Float>>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP Object<BasicRigidMatrixTransformation2D<Float>>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP Object<BasicRigidMatrixTransformation3D<Float>>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP Object<TranslationTransformation<2, Float>>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP Object<TranslationTransformation<3, Float>>;
#endif

}}
