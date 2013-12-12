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

#include "DistanceFieldVector.h"

#include <Utility/Resource.h>

#include "Context.h"
#include "Extensions.h"
#include "Shader.h"

namespace Magnum { namespace Shaders {

namespace {
    template<UnsignedInt> constexpr const char* vertexShaderName();
    template<> constexpr const char* vertexShaderName<2>() { return "AbstractVector2D.vert"; }
    template<> constexpr const char* vertexShaderName<3>() { return "AbstractVector3D.vert"; }
}

template<UnsignedInt dimensions> DistanceFieldVector<dimensions>::DistanceFieldVector(): transformationProjectionMatrixUniform(0), colorUniform(1), outlineColorUniform(2), outlineRangeUniform(3), smoothnessUniform(4) {
    Utility::Resource rs("MagnumShaders");

    /* Weird bug in GCC 4.5 - cannot use initializer list here, although the
       same thing works in PhongShader flawlessly */
    #ifndef MAGNUM_TARGET_GLES
    std::initializer_list<Version> vs{Version::GL320, Version::GL310, Version::GL300, Version::GL210};
    #else
    std::initializer_list<Version> vs{Version::GLES300, Version::GLES200};
    #endif
    Version version = Context::current()->supportedVersion(vs);

    Shader frag(version, Shader::Type::Vertex);
    frag.addSource(rs.get("compatibility.glsl"))
        .addSource(rs.get("generic.glsl"))
        .addSource(rs.get(vertexShaderName<dimensions>()));
    CORRADE_INTERNAL_ASSERT_OUTPUT(frag.compile());
    AbstractShaderProgram::attachShader(frag);

    Shader vert(version, Shader::Type::Fragment);
    vert.addSource(rs.get("compatibility.glsl"))
        .addSource(rs.get("DistanceFieldVector.frag"));
    CORRADE_INTERNAL_ASSERT_OUTPUT(vert.compile());
    AbstractShaderProgram::attachShader(vert);

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::explicit_attrib_location>(version))
    #else
    if(!Context::current()->isVersionSupported(Version::GLES300))
    #endif
    {
        AbstractShaderProgram::bindAttributeLocation(AbstractVector<dimensions>::Position::Location, "position");
        AbstractShaderProgram::bindAttributeLocation(AbstractVector<dimensions>::TextureCoordinates::Location, "textureCoordinates");
    }

    CORRADE_INTERNAL_ASSERT_OUTPUT(AbstractShaderProgram::link());

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::explicit_uniform_location>(version))
    #endif
    {
        transformationProjectionMatrixUniform = AbstractShaderProgram::uniformLocation("transformationProjectionMatrix");
        colorUniform = AbstractShaderProgram::uniformLocation("color");
        outlineColorUniform = AbstractShaderProgram::uniformLocation("outlineColor");
        outlineRangeUniform = AbstractShaderProgram::uniformLocation("outlineRange");
        smoothnessUniform = AbstractShaderProgram::uniformLocation("smoothness");
    }

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::shading_language_420pack>(version))
    #endif
    {
        AbstractShaderProgram::setUniform(AbstractShaderProgram::uniformLocation("vectorTexture"),
                                          AbstractVector<dimensions>::VectorTextureLayer);
    }

    /* Set defaults in OpenGL ES (for desktop they are set in shader code itself) */
    #ifdef MAGNUM_TARGET_GLES
    setOutlineRange(0.5f, 1.0f);
    setSmoothness(0.04f);
    #endif
}

template class DistanceFieldVector<2>;
template class DistanceFieldVector<3>;

}}
