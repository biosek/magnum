#ifndef Magnum_AbstractShaderProgram_h
#define Magnum_AbstractShaderProgram_h
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
 * @brief Class Magnum::AbstractShaderProgram
 */

#include <string>
#include <Containers/EnumSet.h>

#include "Magnum.h"
#include "OpenGL.h"
#include "magnumVisibility.h"

namespace Magnum {

namespace Implementation {
    template<class> struct Attribute;
}

/**
@brief Base for shader program implementations

@section AbstractShaderProgram-subclassing Subclassing workflow

This class is designed to be used via subclassing. Subclasses define these
functions and properties:
 - <strong>%Attribute definitions</strong> with location and type for
   configuring meshes, for example:
@code
typedef Attribute<0, Vector3> Position;
typedef Attribute<1, Vector3> Normal;
typedef Attribute<2, Vector2> TextureCoordinates;
@endcode
 - **Output attribute locations**, if desired, for example:
@code
enum: UnsignedInt {
    ColorOutput = 0,
    NormalOutput = 1
};
@endcode
 - **Layers for texture uniforms** to which the textures will be bound before
   rendering, for example:
@code
enum: Int {
    DiffuseTextureLayer = 0,
    SpecularTextureLayer = 1
};
@endcode
 - **Uniform locations** for setting uniform data (see below) (private
   variables), for example:
@code
Int TransformationUniform = 0,
    ProjectionUniform = 1,
    DiffuseTextureUniform = 2,
    SpecularTextureUniform = 3;
@endcode
 - **Constructor**, which attaches particular shaders, links the program and
   gets uniform locations, for example:
@code
MyShader() {
    // Load shaders, compile them and attach them to the program
    Shader vert(Version::GL430, Shader::Type::Vertex);
    vert.attachFile("PhongShader.vert");
    CORRADE_INTERNAL_ASSERT_OUTPUT(vert.compile());
    attachShader(vert);

    Shader frag(Version::GL430, Shader::Type::Fragment);
    frag.attachFile("PhongShader.vert");
    CORRADE_INTERNAL_ASSERT_OUTPUT(frag.compile());
    attachShader(frag);

    // Link the program together
    CORRADE_INTERNAL_ASSERT_OUTPUT(link());
}
@endcode
 - **Uniform setting functions**, which will provide public interface for
   protected setUniform() functions. For usability purposes you can implement
   also method chaining. Example:
@code
MyShader& setTransformation(const Matrix4& matrix) {
    setUniform(TransformationUniform, matrix);
    return *this;
}
MyShader& setProjection(const Matrix4& matrix) {
    setUniform(ProjectionUniform, matrix);
    return *this;
}
@endcode

@subsection AbstractShaderProgram-attribute-location Binding attribute location

The preferred workflow is to specify attribute location for vertex shader
input attributes and fragment shader output attributes explicitly in the
shader code, e.g.:
@code
// GLSL 3.30, or
#extension GL_ARB_explicit_attrib_location: enable
layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 textureCoordinates;
@endcode

Similarly for ouput attributes, you can also specify blend equation color
index for them (see Framebuffer::BlendFunction for more information about
using color input index):
@code
layout(location = 0, index = 0) out vec4 color;
layout(location = 1, index = 1) out vec3 normal;
@endcode

If you don't have the required extension, declare the attributes without
`layout()` qualifier and use functions @ref bindAttributeLocation() and
@ref bindFragmentDataLocation() / @ref bindFragmentDataLocationIndexed() between
attaching the shaders and linking the program. Note that additional syntax
changes may be needed for GLSL 1.20 and GLSL ES 1.0.
@code
in vec4 position;
in vec3 normal;
in vec2 textureCoordinates;
@endcode
@code
out vec4 color;
out vec3 normal;
@endcode
@code
// Shaders attached...

bindAttributeLocation(Position::Location, "position");
bindAttributeLocation(Normal::Location, "normal");
bindAttributeLocation(TextureCoordinates::Location, "textureCoordinates");

bindFragmentDataLocationIndexed(ColorOutput, 0, "color");
bindFragmentDataLocationIndexed(NormalOutput, 1, "normal");

// Link...
@endcode

@see @ref Mesh::maxVertexAttributes(), @ref AbstractFramebuffer::maxDrawBuffers()
@requires_gl30 %Extension @extension{EXT,gpu_shader4} for using
    bindFragmentDataLocation().
@requires_gl33 %Extension @extension{ARB,blend_func_extended} for using
    bindFragmentDataLocationIndexed().
@requires_gl33 %Extension @extension{ARB,explicit_attrib_location} for
    explicit attribute location instead of using bindAttributeLocation(),
    bindFragmentDataLocation() or bindFragmentDataLocationIndexed().
@requires_gles30 Explicit location specification of input attributes is not
    supported in OpenGL ES 2.0, use bindAttributeLocation() instead.
@requires_gles30 Multiple fragment shader outputs are not available in OpenGL
    ES 2.0, similar functionality is available in extension
    @extension{NV,draw_buffers}.

@subsection AbstractShaderProgram-uniform-location Uniform locations

The preferred workflow is to specify uniform locations directly in the shader
code, e.g.:
@code
// GLSL 4.30, or
#extension GL_ARB_explicit_uniform_location: enable
layout(location = 0) uniform mat4 transformation;
layout(location = 1) uniform mat4 projection;
@endcode

If you don't have the required extension, declare the uniforms without the
`layout()` qualifier and get uniform location using @ref uniformLocation()
*after* linking stage. Note that additional syntax changes may be needed for
GLSL 1.20 and GLSL ES 1.0.
@code
uniform mat4 transformation;
uniform mat4 projection;
@endcode
@code
Int transformationUniform = uniformLocation("transformation");
Int projectionUniform = uniformLocation("projection");
@endcode

@see @ref maxUniformLocations()
@requires_gl43 %Extension @extension{ARB,explicit_uniform_location} for
    explicit uniform location instead of using uniformLocation().
@requires_gl Explicit uniform location is not supported in OpenGL ES. Use
    uniformLocation() instead.

@subsection AbstractShaderProgram-texture-layer Binding texture layer uniforms

The preferred workflow is to specify texture layers directly in the shader
code, e.g.:
@code
// GLSL 4.20, or
#extension GL_ARB_shading_language_420pack: enable
layout(binding = 0) uniform sampler2D diffuseTexture;
layout(binding = 1) uniform sampler2D specularTexture;
@endcode

If you don't have the required extension (or if you want to change the layer
later), declare the uniforms without the `layout()` qualifier and set the
texture layer uniform using @ref Magnum::AbstractShaderProgram::setUniform(Int, const T&) "setUniform(Int, Int)".
Note that additional syntax changes may be needed for GLSL 1.20 and GLSL ES
1.0.
@code
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
@endcode
@code
setUniform(DiffuseTextureUniform, DiffuseTextureLayer);
setUniform(SpecularTextureUniform, SpecularTextureLayer);
@endcode

@see @ref Shader::maxTextureImageUnits()
@requires_gl42 %Extension @extension{ARB,shading_language_420pack} for explicit
    texture layer binding instead of using
    @ref Magnum::AbstractShaderProgram::setUniform(Int, const T&) "setUniform(Int, Int)".
@requires_gl Explicit texture layer binding is not supported in OpenGL ES. Use
    @ref Magnum::AbstractShaderProgram::setUniform(Int, const T&) "setUniform(Int, Int)"
    instead.

@section AbstractShaderProgram-rendering-workflow Rendering workflow

Basic workflow with %AbstractShaderProgram subclasses is: instance shader
class, configure attribute binding in meshes (see @ref Mesh-configuration "Mesh documentation"
for more information) and map shader outputs to framebuffer attachments if
needed (see @ref Framebuffer-usage "Framebuffer documentation" for more
information). In each draw event set uniforms, mark the shader for use, bind
specific framebuffer (if needed) and bind required textures to their
respective layers using AbstractTexture::bind(Int). Then call Mesh::draw().
Example:
@code
shader.setTransformation(transformation)
    .setProjection(projection)
    .use();

diffuseTexture.bind(MyShader::DiffuseTextureLayer);
specularTexture.bind(MyShader::SpecularTextureLayer);

mesh.draw();
@endcode

@section AbstractShaderProgram-types Mapping between GLSL and Magnum types

See @ref types for more information, only types with GLSL equivalent can be used
(and their super- or subclasses with the same size and underlying type).

@requires_gl30 %Extension @extension{EXT,gpu_shader4} is required when using
    integer attributes (i.e. @ref Magnum::UnsignedInt "UnsignedInt",
    @ref Magnum::Int "Int", @ref Magnum::Vector2ui "Vector2ui",
    @ref Magnum::Vector2i "Vector2i", @ref Magnum::Vector3ui "Vector3ui",
    @ref Magnum::Vector3i "Vector3i", @ref Magnum::Vector4ui "Vector4ui" and
    @ref Magnum::Vector4i "Vector4i") or unsigned integer uniforms (i.e.
    @ref Magnum::UnsignedInt "UnsignedInt", @ref Magnum::Vector2ui "Vector2ui",
    @ref Magnum::Vector3ui "Vector3ui" and @ref Magnum::Vector4ui "Vector4ui").
@requires_gles30 Integer attributes and unsigned integer uniforms are not
    available in OpenGL ES 2.0.

@requires_gl40 %Extension @extension{ARB,gpu_shader_fp64} is required when
    using double uniforms (i.e. @ref Magnum::Double "Double",
    @ref Magnum::Vector2d "Vector2d", @ref Magnum::Vector3d "Vector3d",
    @ref Magnum::Vector4d "Vector4d", @ref Magnum::Matrix2d "Matrix2d",
    @ref Magnum::Matrix3d "Matrix3d", @ref Magnum::Matrix4d "Matrix4d",
    @ref Magnum::Matrix2x3d "Matrix2x3d", @ref Magnum::Matrix3x2d "Matrix3x2d",
    @ref Magnum::Matrix2x4d "Matrix2x4d", @ref Magnum::Matrix4x2d "Matrix4x2d",
    @ref Magnum::Matrix3x4d "Matrix3x4d" and @ref Magnum::Matrix4x3d "Matrix4x3d").
@requires_gl41 %Extension @extension{ARB,vertex_attrib_64bit} is required when
    using double attributes (i.e. @ref Magnum::Double "Double",
    @ref Magnum::Vector2d "Vector2d", @ref Magnum::Vector3d "Vector3d",
    @ref Magnum::Vector4d "Vector4d", @ref Magnum::Matrix2d "Matrix2d",
    @ref Magnum::Matrix3d "Matrix3d", @ref Magnum::Matrix4d "Matrix4d",
    @ref Magnum::Matrix2x3d "Matrix2x3d", @ref Magnum::Matrix3x2d "Matrix3x2d",
    @ref Magnum::Matrix2x4d "Matrix2x4d", @ref Magnum::Matrix4x2d "Matrix4x2d",
    @ref Magnum::Matrix3x4d "Matrix3x4d" and @ref Magnum::Matrix4x3d "Matrix4x3d").
@requires_gl Double attributes and uniforms are not available in OpenGL ES.

@requires_gles30 Non-square matrix attributes and uniforms (i.e.
    @ref Magnum::Matrix2x3 "Matrix2x3", @ref Magnum::Matrix3x2 "Matrix3x2",
    @ref Magnum::Matrix2x4 "Matrix2x4", @ref Magnum::Matrix4x2d "Matrix4x2",
    @ref Magnum::Matrix3x4 "Matrix3x4" and @ref Magnum::Matrix4x3 "Matrix4x3")
    are not available in OpenGL ES 2.0.

@section AbstractShaderProgram-performance-optimization Performance optimizations

The engine tracks currently used shader program to avoid unnecessary calls to
@fn_gl{UseProgram}. %Shader limits (such as @ref maxVertexAttributes())
are cached, so repeated queries don't result in repeated @fn_gl{Get} calls.

If extension @extension{ARB,separate_shader_objects} or
@extension{EXT,direct_state_access} is available, uniform setting functions
use DSA functions to avoid unnecessary calls to @fn_gl{UseProgram}. See
setUniform() documentation for more information.

To achieve least state changes, set all uniforms in one run -- method chaining
comes in handy.

@see @ref portability-shaders

@todo Compiling and linking more than one shader in parallel, then checking
    status, should be faster -- https://twitter.com/g_truc/status/352778836657700866
@todo `GL_NUM_{PROGRAM,SHADER}_BINARY_FORMATS` + `GL_{PROGRAM,SHADER}_BINARY_FORMATS` (vector), (@extension{ARB,ES2_compatibility})
 */
class MAGNUM_EXPORT AbstractShaderProgram {
    friend class Context;

    public:
        template<UnsignedInt, class> class Attribute;

        /**
         * @brief Max supported vertex attribute count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls.
         * @see @ref Mesh::maxVertexAttributes(),
         *      @ref AbstractShaderProgram::Attribute, @fn_gl{Get} with
         *      @def_gl{MAX_VERTEX_ATTRIBS}
         */
        static Int maxVertexAttributes();

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copydoc maxVertexAttributes()
         * @deprecated Use @ref Magnum::AbstractShaderProgram::maxVertexAttributes() "maxVertexAttributes()"
         *      instead.
         */
        static CORRADE_DEPRECATED("use maxVertexAttributes() instead") Int maxSupportedVertexAttributeCount() { return maxVertexAttributes(); }
        #endif

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Max supported atomic counter buffer size
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @extension{ARB,shader_atomic_counters} is
         * not available, returns `0`.
         * @requires_gl Atomic counters are not available in OpenGL ES.
         * @see @fn_gl{Get} with @def_gl{MAX_ATOMIC_COUNTER_BUFFER_SIZE}
         */
        static Int maxAtomicCounterBufferSize();

        /**
         * @brief Max supported compute shared memory size
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @extension{ARB,compute_shader} is not
         * available, returns `0`.
         * @requires_gl Compute shaders are not available in OpenGL ES.
         * @see @fn_gl{Get} with @def_gl{MAX_COMPUTE_SHARED_MEMORY_SIZE}
         */
        static Int maxComputeSharedMemorySize();

        /**
         * @brief Max supported compute work group invocation count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @extension{ARB,compute_shader} is not
         * available, returns `0`.
         * @requires_gl Compute shaders are not available in OpenGL ES.
         * @see @fn_gl{Get} with @def_gl{MAX_COMPUTE_WORK_GROUP_INVOCATIONS}
         */
        static Int maxComputeWorkGroupInvocations();

        /** @todo MAX_COMPUTE_WORK_GROUP_COUNT, MAX_COMPUTE_WORK_GROUP_SIZE */

        /**
         * @brief Max supported image unit count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @extension{ARB,shader_image_load_store}
         * is not available, returns `0`.
         * @requires_gl Image load/store is not available in OpenGL ES.
         * @see @fn_gl{Get} with @def_gl{MAX_IMAGE_UNITS}
         */
        static Int maxImageUnits();

        /**
         * @brief Max supported image sample count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @extension{ARB,shader_image_load_store}
         * is not available, returns `0`.
         * @requires_gl Image load/store is not available in OpenGL ES.
         * @see @fn_gl{Get} with @def_gl{MAX_IMAGE_SAMPLES}
         */
        static Int maxImageSamples();

        /**
         * @brief Max supported combined shader output resource count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither @extension{ARB,shader_image_load_store}
         * nor @extension{ARB,shader_storage_buffer_object} extension is
         * available, returns `0`.
         * @requires_gl Image load/store is not available in OpenGL ES.
         * @see @fn_gl{Get} with @def_gl{MAX_COMBINED_SHADER_OUTPUT_RESOURCES}
         */
        static Int maxCombinedShaderOutputResources();

        /**
         * @brief Max supported shader storage block size
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @extension{ARB,shader_storage_buffer_object}
         * is not available, returns `0`.
         * @requires_gl Shader storage is not available in OpenGL ES.
         * @see @fn_gl{Get} with @def_gl{MAX_SHADER_STORAGE_BLOCK_SIZE}
         */
        static Long maxShaderStorageBlockSize();
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Max supported uniform block size
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @extension{ARB,uniform_buffer_object}
         * is not available, returns `0`.
         * @requires_gles30 Uniform blocks are not available in OpenGL ES 2.0.
         * @see @fn_gl{Get} with @def_gl{MAX_UNIFORM_BLOCK_SIZE}
         */
        static Int maxUniformBlockSize();
        #endif

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Max supported explicit uniform location count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @extension{ARB,explicit_uniform_location}
         * is not available, returns `0`.
         * @requires_gl Explicit uniform location is not supported in OpenGL ES.
         * @see @fn_gl{Get} with @def_gl{MAX_UNIFORM_LOCATIONS}
         */
        static Int maxUniformLocations();
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Min supported program texel offset
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @extension{EXT,gpu_shader4} is not
         * available, returns `0`.
         * @requires_gles30 Texture lookup with offset is not available in
         *      OpenGL ES 2.0.
         * @see @fn_gl{Get} with @def_gl{MIN_PROGRAM_TEXEL_OFFSET}
         */
        static Int minTexelOffset();

        /**
         * @brief Max supported program texel offset
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @extension{EXT,gpu_shader4} is not
         * available, returns `0`.
         * @requires_gles30 Texture lookup with offset is not available in
         *      OpenGL ES 2.0.
         * @see @fn_gl{Get} with @def_gl{MAX_PROGRAM_TEXEL_OFFSET}
         */
        static Int maxTexelOffset();
        #endif

        /**
         * @brief Constructor
         *
         * Creates one OpenGL shader program.
         * @see @fn_gl{CreateProgram}
         */
        explicit AbstractShaderProgram();

        /** @brief Copying is not allowed */
        AbstractShaderProgram(const AbstractShaderProgram&) = delete;

        /** @brief Move constructor */
        AbstractShaderProgram(AbstractShaderProgram&& other) noexcept;

        /**
         * @brief Destructor
         *
         * Deletes associated OpenGL shader program.
         * @see @fn_gl{DeleteProgram}
         */
        virtual ~AbstractShaderProgram() = 0;

        /** @brief Copying is not allowed */
        AbstractShaderProgram& operator=(const AbstractShaderProgram&) = delete;

        /** @brief Move assignment */
        AbstractShaderProgram& operator=(AbstractShaderProgram&& other) noexcept;

        /** @brief OpenGL program ID */
        GLuint id() const { return _id; }

        /**
         * @brief Validate program
         *
         * Returns validation status and optional validation message.
         * @see @fn_gl{ValidateProgram}, @fn_gl{GetProgram} with
         *      @def_gl{VALIDATE_STATUS}, @def_gl{INFO_LOG_LENGTH},
         *      @fn_gl{GetProgramInfoLog}
         */
        std::pair<bool, std::string> validate();

        /**
         * @brief Use shader for rendering
         *
         * @see @fn_gl{UseProgram}
         */
        void use();

    protected:
        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Allow retrieving program binary
         *
         * Initially disabled.
         * @see @fn_gl{ProgramParameter} with @def_gl{PROGRAM_BINARY_RETRIEVABLE_HINT}
         * @requires_gl41 %Extension @extension{ARB,get_program_binary}
         * @requires_gles30 Always allowed in OpenGL ES 2.0.
         */
        void setRetrievableBinary(bool enabled) {
            glProgramParameteri(_id, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, enabled ? GL_TRUE : GL_FALSE);
        }
        #endif

        /**
         * @brief Allow the program to be bound to individual pipeline stages
         *
         * Initially disabled.
         * @see @fn_gl{ProgramParameter} with @def_gl{PROGRAM_SEPARABLE}
         * @requires_gl41 %Extension @extension{ARB,separate_shader_objects}
         * @requires_es_extension %Extension @es_extension{EXT,separate_shader_objects}
         */
        void setSeparable(bool enabled) {
            /** @todo Remove when extension wrangler is available for ES */
            #ifndef MAGNUM_TARGET_GLES
            glProgramParameteri(_id, GL_PROGRAM_SEPARABLE, enabled ? GL_TRUE : GL_FALSE);
            #else
            static_cast<void>(enabled);
            #endif
        }

        /**
         * @brief Attach shader
         *
         * @fn_gl{AttachShader}
         */
        void attachShader(Shader& shader);

        /**
         * @brief Bind attribute to given location
         * @param location      Location
         * @param name          Attribute name
         *
         * Binds attribute to location which is used later for binding vertex
         * buffers.
         * @deprecated_gl Preferred usage is to specify attribute location
         *      explicitly in the shader instead of using this function. See
         *      @ref AbstractShaderProgram-attribute-location "class documentation"
         *      for more information.
         * @see @fn_gl{BindAttribLocation}
         */
        void bindAttributeLocation(UnsignedInt location, const std::string& name);

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Bind fragment data to given location and color input index
         * @param location      Location
         * @param name          Fragment output variable name
         * @param index         Blend equation color input index (`0` or `1`)
         *
         * Binds fragment data to location which is used later for framebuffer
         * operations. See also Framebuffer::BlendFunction for more
         * information about using color input index.
         * @deprecated_gl Preferred usage is to specify attribute location
         *      explicitly in the shader instead of using this function. See
         *      @ref AbstractShaderProgram-attribute-location "class documentation"
         *      for more information.
         * @see @fn_gl{BindFragDataLocationIndexed}
         * @requires_gl33 %Extension @extension{ARB,blend_func_extended}
         * @requires_gl Multiple blend function inputs are not available in
         *      OpenGL ES.
         */
        void bindFragmentDataLocationIndexed(UnsignedInt location, UnsignedInt index, const std::string& name);

        /**
         * @brief Bind fragment data to given location and first color input index
         * @param location      Location
         * @param name          Fragment output variable name
         *
         * The same as bindFragmentDataLocationIndexed(), but with `index` set
         * to `0`.
         * @see @fn_gl{BindFragDataLocation}
         * @requires_gl30 %Extension @extension{EXT,gpu_shader4}
         * @requires_gl Use explicit location specification in OpenGL ES 3.0
         *      instead.
         */
        void bindFragmentDataLocation(UnsignedInt location, const std::string& name);
        #endif

        /**
         * @brief Link the shader
         *
         * Returns `false` if linking failed, `true` otherwise. Compiler
         * message (if any) is printed to error output. All attached shaders
         * must be explicitly compiled with Shader::compile() before linking.
         * @see @fn_gl{LinkProgram}, @fn_gl{GetProgram} with
         *      @def_gl{LINK_STATUS} and @def_gl{INFO_LOG_LENGTH},
         *      @fn_gl{GetProgramInfoLog}
         */
        bool link();

        /**
         * @brief Get uniform location
         * @param name          Uniform name
         *
         * @deprecated_gl Preferred usage is to specify uniform location
         *      explicitly in the shader instead of using this function. See
         *      @ref AbstractShaderProgram-uniform-location "class documentation"
         *      for more information.
         * @see @fn_gl{GetUniformLocation}
         */
        Int uniformLocation(const std::string& name);

        /**
         * @brief Set uniform value
         * @param location      Uniform location
         * @param value         Value
         *
         * Convenience alternative for setting one value, see
         * setUniform(Int, UnsignedInt, const Float*) for more information.
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        template<class T> inline void setUniform(Int location, const T& value);
        #else
        void setUniform(Int location, Float value) {
            setUniform(location, 1, &value);
        }
        void setUniform(Int location, Int value) {
            setUniform(location, 1, &value);
        }
        #ifndef MAGNUM_TARGET_GLES2
        void setUniform(Int location, UnsignedInt value) {
            setUniform(location, 1, &value);
        }
        #endif
        #ifndef MAGNUM_TARGET_GLES
        void setUniform(Int location, Double value) {
            setUniform(location, 1, &value);
        }
        #endif
        template<std::size_t size, class T> void setUniform(Int location, const Math::Vector<size, T>& value) {
            setUniform(location, 1, &value);
        }
        template<std::size_t cols, std::size_t rows, class T> void setUniform(Int location, const Math::RectangularMatrix<cols, rows, T>& value) {
            setUniform(location, 1, &value);
        }
        #endif

        /**
         * @brief Set uniform values
         * @param location      Uniform location
         * @param count         Value count
         * @param values        Values
         *
         * If neither @extension{ARB,separate_shader_objects} nor
         * @extension{EXT,direct_state_access} is available, the shader is
         * marked for use before the operation.
         * @see setUniform(Int, const T&), @fn_gl{UseProgram}, @fn_gl{Uniform}
         *      or @fn_gl{ProgramUniform}/@fn_gl_extension{ProgramUniform,EXT,direct_state_access}.
         */
        void setUniform(Int location, UnsignedInt count, const Float* values) {
            (this->*uniform1fvImplementation)(location, count, values);
        }

        /** @copydoc setUniform(Int, UnsignedInt, const Float*) */
        void setUniform(Int location, UnsignedInt count, const Math::Vector<2, Float>* values) {
            (this->*uniform2fvImplementation)(location, count, values);
        }

        /** @copydoc setUniform(Int, UnsignedInt, const Float*) */
        void setUniform(Int location, UnsignedInt count, const Math::Vector<3, Float>* values) {
            (this->*uniform3fvImplementation)(location, count, values);
        }

        /** @copydoc setUniform(Int, UnsignedInt, const Float*) */
        void setUniform(Int location, UnsignedInt count, const Math::Vector<4, Float>* values) {
            (this->*uniform4fvImplementation)(location, count, values);
        }

        /** @copydoc setUniform(Int, UnsignedInt, const Float*) */
        void setUniform(Int location, UnsignedInt count, const Int* values) {
            (this->*uniform1ivImplementation)(location, count, values);
        }

        /** @copydoc setUniform(Int, UnsignedInt, const Float*) */
        void setUniform(Int location, UnsignedInt count, const Math::Vector<2, Int>* values) {
            (this->*uniform2ivImplementation)(location, count, values);
        }

        /** @copydoc setUniform(Int, UnsignedInt, const Float*) */
        void setUniform(Int location, UnsignedInt count, const Math::Vector<3, Int>* values) {
            (this->*uniform3ivImplementation)(location, count, values);
        }

        /** @copydoc setUniform(Int, UnsignedInt, const Float*) */
        void setUniform(Int location, UnsignedInt count, const Math::Vector<4, Int>* values) {
            (this->*uniform4ivImplementation)(location, count, values);
        }

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @copydoc setUniform(Int, UnsignedInt, const Float*)
         * @requires_gl30 %Extension @extension{EXT,gpu_shader4}
         * @requires_gles30 Only signed integers are available in OpenGL ES 2.0.
         */
        void setUniform(Int location, UnsignedInt count, const UnsignedInt* values) {
            (this->*uniform1uivImplementation)(location, count, values);
        }

        /**
         * @copydoc setUniform(Int, UnsignedInt, const Float*)
         * @requires_gl30 %Extension @extension{EXT,gpu_shader4}
         * @requires_gles30 Only signed integers are available in OpenGL ES 2.0.
         */
        void setUniform(Int location, UnsignedInt count, const Math::Vector<2, UnsignedInt>* values) {
            (this->*uniform2uivImplementation)(location, count, values);
        }

        /**
         * @copydoc setUniform(Int, UnsignedInt, const Float*)
         * @requires_gl30 %Extension @extension{EXT,gpu_shader4}
         * @requires_gles30 Only signed integers are available in OpenGL ES 2.0.
         */
        void setUniform(Int location, UnsignedInt count, const Math::Vector<3, UnsignedInt>* values) {
            (this->*uniform3uivImplementation)(location, count, values);
        }

        /**
         * @copydoc setUniform(Int, UnsignedInt, const Float*)
         * @requires_gl30 %Extension @extension{EXT,gpu_shader4}
         * @requires_gles30 Only signed integers are available in OpenGL ES 2.0.
         */
        void setUniform(Int location, UnsignedInt count, const Math::Vector<4, UnsignedInt>* values) {
            (this->*uniform4uivImplementation)(location, count, values);
        }
        #endif

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @copydoc setUniform(Int, UnsignedInt, const Float*)
         * @requires_gl40 %Extension @extension{ARB,gpu_shader_fp64}
         * @requires_gl Only floats are available in OpenGL ES.
         */
        void setUniform(Int location, UnsignedInt count, const Double* values) {
            (this->*uniform1dvImplementation)(location, count, values);
        }

        /**
         * @copydoc setUniform(Int, UnsignedInt, const Float*)
         * @requires_gl40 %Extension @extension{ARB,gpu_shader_fp64}
         * @requires_gl Only floats are available in OpenGL ES.
         */
        void setUniform(Int location, UnsignedInt count, const Math::Vector<2, Double>* values) {
            (this->*uniform2dvImplementation)(location, count, values);
        }

        /**
         * @copydoc setUniform(Int, UnsignedInt, const Float*)
         * @requires_gl40 %Extension @extension{ARB,gpu_shader_fp64}
         * @requires_gl Only floats are available in OpenGL ES.
         */
        void setUniform(Int location, UnsignedInt count, const Math::Vector<3, Double>* values) {
            (this->*uniform3dvImplementation)(location, count, values);
        }

        /**
         * @copydoc setUniform(Int, UnsignedInt, const Float*)
         * @requires_gl40 %Extension @extension{ARB,gpu_shader_fp64}
         * @requires_gl Only floats are available in OpenGL ES.
         */
        void setUniform(Int location, UnsignedInt count, const Math::Vector<4, Double>* values) {
            (this->*uniform4dvImplementation)(location, count, values);
        }
        #endif

        /** @copydoc setUniform(Int, UnsignedInt, const Float*) */
        void setUniform(Int location, UnsignedInt count, const Math::RectangularMatrix<2, 2, Float>* values) {
            (this->*uniformMatrix2fvImplementation)(location, count, values);
        }

        /** @copydoc setUniform(Int, UnsignedInt, const Float*) */
        void setUniform(Int location, UnsignedInt count, const Math::RectangularMatrix<3, 3, Float>* values) {
            (this->*uniformMatrix3fvImplementation)(location, count, values);
        }

        /** @copydoc setUniform(Int, UnsignedInt, const Float*) */
        void setUniform(Int location, UnsignedInt count, const Math::RectangularMatrix<4, 4, Float>* values) {
            (this->*uniformMatrix4fvImplementation)(location, count, values);
        }

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @copydoc setUniform(Int, UnsignedInt, const Float*)
         * @requires_gles30 Only square matrices are available in OpenGL ES 2.0.
         */
        void setUniform(Int location, UnsignedInt count, const Math::RectangularMatrix<2, 3, Float>* values) {
            (this->*uniformMatrix2x3fvImplementation)(location, count, values);
        }

        /**
         * @copydoc setUniform(Int, UnsignedInt, const Float*)
         * @requires_gles30 Only square matrices are available in OpenGL ES 2.0.
         */
        void setUniform(Int location, UnsignedInt count, const Math::RectangularMatrix<3, 2, Float>* values) {
            (this->*uniformMatrix3x2fvImplementation)(location, count, values);
        }

        /**
         * @copydoc setUniform(Int, UnsignedInt, const Float*)
         * @requires_gles30 Only square matrices are available in OpenGL ES 2.0.
         */
        void setUniform(Int location, UnsignedInt count, const Math::RectangularMatrix<2, 4, Float>* values) {
            (this->*uniformMatrix2x4fvImplementation)(location, count, values);
        }

        /**
         * @copydoc setUniform(Int, UnsignedInt, const Float*)
         * @requires_gles30 Only square matrices are available in OpenGL ES 2.0.
         */
        void setUniform(Int location, UnsignedInt count, const Math::RectangularMatrix<4, 2, Float>* values) {
            (this->*uniformMatrix4x2fvImplementation)(location, count, values);
        }

        /**
         * @copydoc setUniform(Int, UnsignedInt, const Float*)
         * @requires_gles30 Only square matrices are available in OpenGL ES 2.0.
         */
        void setUniform(Int location, UnsignedInt count, const Math::RectangularMatrix<3, 4, Float>* values) {
            (this->*uniformMatrix3x4fvImplementation)(location, count, values);
        }

        /**
         * @copydoc setUniform(Int, UnsignedInt, const Float*)
         * @requires_gles30 Only square matrices are available in OpenGL ES 2.0.
         */
        void setUniform(Int location, UnsignedInt count, const Math::RectangularMatrix<4, 3, Float>* values) {
            (this->*uniformMatrix4x3fvImplementation)(location, count, values);
        }
        #endif

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @copydoc setUniform(Int, UnsignedInt, const Float*)
         * @requires_gl40 %Extension @extension{ARB,gpu_shader_fp64}
         * @requires_gl Only floats are available in OpenGL ES.
         */
        void setUniform(Int location, UnsignedInt count, const Math::RectangularMatrix<2, 2, Double>* values) {
            (this->*uniformMatrix2dvImplementation)(location, count, values);
        }

        /**
         * @copydoc setUniform(Int, UnsignedInt, const Float*)
         * @requires_gl40 %Extension @extension{ARB,gpu_shader_fp64}
         * @requires_gl Only floats are available in OpenGL ES.
         */
        void setUniform(Int location, UnsignedInt count, const Math::RectangularMatrix<3, 3, Double>* values) {
            (this->*uniformMatrix3dvImplementation)(location, count, values);
        }

        /**
         * @copydoc setUniform(Int, UnsignedInt, const Float*)
         * @requires_gl40 %Extension @extension{ARB,gpu_shader_fp64}
         * @requires_gl Only floats are available in OpenGL ES.
         */
        void setUniform(Int location, UnsignedInt count, const Math::RectangularMatrix<4, 4, Double>* values) {
            (this->*uniformMatrix4dvImplementation)(location, count, values);
        }

        /**
         * @copydoc setUniform(Int, UnsignedInt, const Float*)
         * @requires_gl40 %Extension @extension{ARB,gpu_shader_fp64}
         * @requires_gl Only floats are available in OpenGL ES.
         */
        void setUniform(Int location, UnsignedInt count, const Math::RectangularMatrix<2, 3, Double>* values) {
            (this->*uniformMatrix2x3dvImplementation)(location, count, values);
        }

        /**
         * @copydoc setUniform(Int, UnsignedInt, const Float*)
         * @requires_gl40 %Extension @extension{ARB,gpu_shader_fp64}
         * @requires_gl Only floats are available in OpenGL ES.
         */
        void setUniform(Int location, UnsignedInt count, const Math::RectangularMatrix<3, 2, Double>* values) {
            (this->*uniformMatrix3x2dvImplementation)(location, count, values);
        }

        /**
         * @copydoc setUniform(Int, UnsignedInt, const Float*)
         * @requires_gl40 %Extension @extension{ARB,gpu_shader_fp64}
         * @requires_gl Only floats are available in OpenGL ES.
         */
        void setUniform(Int location, UnsignedInt count, const Math::RectangularMatrix<2, 4, Double>* values) {
            (this->*uniformMatrix2x4dvImplementation)(location, count, values);
        }

        /**
         * @copydoc setUniform(Int, UnsignedInt, const Float*)
         * @requires_gl40 %Extension @extension{ARB,gpu_shader_fp64}
         * @requires_gl Only floats are available in OpenGL ES.
         */
        void setUniform(Int location, UnsignedInt count, const Math::RectangularMatrix<4, 2, Double>* values) {
            (this->*uniformMatrix4x2dvImplementation)(location, count, values);
        }

        /**
         * @copydoc setUniform(Int, UnsignedInt, const Float*)
         * @requires_gl40 %Extension @extension{ARB,gpu_shader_fp64}
         * @requires_gl Only floats are available in OpenGL ES.
         */
        void setUniform(Int location, UnsignedInt count, const Math::RectangularMatrix<3, 4, Double>* values) {
            (this->*uniformMatrix3x4dvImplementation)(location, count, values);
        }

        /**
         * @copydoc setUniform(Int, UnsignedInt, const Float*)
         * @requires_gl40 %Extension @extension{ARB,gpu_shader_fp64}
         * @requires_gl Only floats are available in OpenGL ES.
         */
        void setUniform(Int location, UnsignedInt count, const Math::RectangularMatrix<4, 3, Double>* values) {
            (this->*uniformMatrix4x3dvImplementation)(location, count, values);
        }
        #endif

    private:
        static void MAGNUM_LOCAL initializeContextBasedFunctionality(Context& context);

        typedef void(AbstractShaderProgram::*Uniform1fvImplementation)(GLint, GLsizei, const GLfloat*);
        typedef void(AbstractShaderProgram::*Uniform2fvImplementation)(GLint, GLsizei, const Math::Vector<2, GLfloat>*);
        typedef void(AbstractShaderProgram::*Uniform3fvImplementation)(GLint, GLsizei, const Math::Vector<3, GLfloat>*);
        typedef void(AbstractShaderProgram::*Uniform4fvImplementation)(GLint, GLsizei, const Math::Vector<4, GLfloat>*);
        typedef void(AbstractShaderProgram::*Uniform1ivImplementation)(GLint, GLsizei, const GLint*);
        typedef void(AbstractShaderProgram::*Uniform2ivImplementation)(GLint, GLsizei, const Math::Vector<2, GLint>*);
        typedef void(AbstractShaderProgram::*Uniform3ivImplementation)(GLint, GLsizei, const Math::Vector<3, GLint>*);
        typedef void(AbstractShaderProgram::*Uniform4ivImplementation)(GLint, GLsizei, const Math::Vector<4, GLint>*);
        #ifndef MAGNUM_TARGET_GLES2
        typedef void(AbstractShaderProgram::*Uniform1uivImplementation)(GLint, GLsizei, const GLuint*);
        typedef void(AbstractShaderProgram::*Uniform2uivImplementation)(GLint, GLsizei, const Math::Vector<2, GLuint>*);
        typedef void(AbstractShaderProgram::*Uniform3uivImplementation)(GLint, GLsizei, const Math::Vector<3, GLuint>*);
        typedef void(AbstractShaderProgram::*Uniform4uivImplementation)(GLint, GLsizei, const Math::Vector<4, GLuint>*);
        #endif
        #ifndef MAGNUM_TARGET_GLES
        typedef void(AbstractShaderProgram::*Uniform1dvImplementation)(GLint, GLsizei, const GLdouble*);
        typedef void(AbstractShaderProgram::*Uniform2dvImplementation)(GLint, GLsizei, const Math::Vector<2, GLdouble>*);
        typedef void(AbstractShaderProgram::*Uniform3dvImplementation)(GLint, GLsizei, const Math::Vector<3, GLdouble>*);
        typedef void(AbstractShaderProgram::*Uniform4dvImplementation)(GLint, GLsizei, const Math::Vector<4, GLdouble>*);
        #endif
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const GLfloat* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::Vector<2, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::Vector<3, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::Vector<4, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const GLint* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::Vector<2, GLint>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::Vector<3, GLint>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::Vector<4, GLint>* values);
        #ifndef MAGNUM_TARGET_GLES2
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const GLuint* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::Vector<2, GLuint>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::Vector<3, GLuint>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::Vector<4, GLuint>* values);
        #endif
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const GLdouble* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::Vector<2, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::Vector<3, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::Vector<4, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, GLsizei count, const GLfloat* values);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, GLsizei count, const Math::Vector<2, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, GLsizei count, const Math::Vector<3, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, GLsizei count, const Math::Vector<4, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, GLsizei count, const GLint* values);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, GLsizei count, const Math::Vector<2, GLint>* values);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, GLsizei count, const Math::Vector<3, GLint>* values);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, GLsizei count, const Math::Vector<4, GLint>* values);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, GLsizei count, const GLuint* values);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, GLsizei count, const Math::Vector<2, GLuint>* values);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, GLsizei count, const Math::Vector<3, GLuint>* values);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, GLsizei count, const Math::Vector<4, GLuint>* values);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, GLsizei count, const GLdouble* values);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, GLsizei count, const Math::Vector<2, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, GLsizei count, const Math::Vector<3, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, GLsizei count, const Math::Vector<4, GLdouble>* values);
        #endif
        static Uniform1fvImplementation uniform1fvImplementation;
        static Uniform2fvImplementation uniform2fvImplementation;
        static Uniform3fvImplementation uniform3fvImplementation;
        static Uniform4fvImplementation uniform4fvImplementation;
        static Uniform1ivImplementation uniform1ivImplementation;
        static Uniform2ivImplementation uniform2ivImplementation;
        static Uniform3ivImplementation uniform3ivImplementation;
        static Uniform4ivImplementation uniform4ivImplementation;
        #ifndef MAGNUM_TARGET_GLES2
        static Uniform1uivImplementation uniform1uivImplementation;
        static Uniform2uivImplementation uniform2uivImplementation;
        static Uniform3uivImplementation uniform3uivImplementation;
        static Uniform4uivImplementation uniform4uivImplementation;
        #endif
        #ifndef MAGNUM_TARGET_GLES
        static Uniform1dvImplementation uniform1dvImplementation;
        static Uniform2dvImplementation uniform2dvImplementation;
        static Uniform3dvImplementation uniform3dvImplementation;
        static Uniform4dvImplementation uniform4dvImplementation;
        #endif

        typedef void(AbstractShaderProgram::*UniformMatrix2fvImplementation)(GLint, GLsizei, const Math::RectangularMatrix<2, 2, GLfloat>*);
        typedef void(AbstractShaderProgram::*UniformMatrix3fvImplementation)(GLint, GLsizei, const Math::RectangularMatrix<3, 3, GLfloat>*);
        typedef void(AbstractShaderProgram::*UniformMatrix4fvImplementation)(GLint, GLsizei, const Math::RectangularMatrix<4, 4, GLfloat>*);
        #ifndef MAGNUM_TARGET_GLES2
        typedef void(AbstractShaderProgram::*UniformMatrix2x3fvImplementation)(GLint, GLsizei, const Math::RectangularMatrix<2, 3, GLfloat>*);
        typedef void(AbstractShaderProgram::*UniformMatrix3x2fvImplementation)(GLint, GLsizei, const Math::RectangularMatrix<3, 2, GLfloat>*);
        typedef void(AbstractShaderProgram::*UniformMatrix2x4fvImplementation)(GLint, GLsizei, const Math::RectangularMatrix<2, 4, GLfloat>*);
        typedef void(AbstractShaderProgram::*UniformMatrix4x2fvImplementation)(GLint, GLsizei, const Math::RectangularMatrix<4, 2, GLfloat>*);
        typedef void(AbstractShaderProgram::*UniformMatrix3x4fvImplementation)(GLint, GLsizei, const Math::RectangularMatrix<3, 4, GLfloat>*);
        typedef void(AbstractShaderProgram::*UniformMatrix4x3fvImplementation)(GLint, GLsizei, const Math::RectangularMatrix<4, 3, GLfloat>*);
        #endif
        #ifndef MAGNUM_TARGET_GLES
        typedef void(AbstractShaderProgram::*UniformMatrix2dvImplementation)(GLint, GLsizei, const Math::RectangularMatrix<2, 2, GLdouble>*);
        typedef void(AbstractShaderProgram::*UniformMatrix3dvImplementation)(GLint, GLsizei, const Math::RectangularMatrix<3, 3, GLdouble>*);
        typedef void(AbstractShaderProgram::*UniformMatrix4dvImplementation)(GLint, GLsizei, const Math::RectangularMatrix<4, 4, GLdouble>*);
        typedef void(AbstractShaderProgram::*UniformMatrix2x3dvImplementation)(GLint, GLsizei, const Math::RectangularMatrix<2, 3, GLdouble>*);
        typedef void(AbstractShaderProgram::*UniformMatrix3x2dvImplementation)(GLint, GLsizei, const Math::RectangularMatrix<3, 2, GLdouble>*);
        typedef void(AbstractShaderProgram::*UniformMatrix2x4dvImplementation)(GLint, GLsizei, const Math::RectangularMatrix<2, 4, GLdouble>*);
        typedef void(AbstractShaderProgram::*UniformMatrix4x2dvImplementation)(GLint, GLsizei, const Math::RectangularMatrix<4, 2, GLdouble>*);
        typedef void(AbstractShaderProgram::*UniformMatrix3x4dvImplementation)(GLint, GLsizei, const Math::RectangularMatrix<3, 4, GLdouble>*);
        typedef void(AbstractShaderProgram::*UniformMatrix4x3dvImplementation)(GLint, GLsizei, const Math::RectangularMatrix<4, 3, GLdouble>*);
        #endif
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<2, 2, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<3, 3, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<4, 4, GLfloat>* values);
        #ifndef MAGNUM_TARGET_GLES2
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<2, 3, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<3, 2, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<2, 4, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<4, 2, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<3, 4, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<4, 3, GLfloat>* values);
        #endif
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<2, 2, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<3, 3, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<4, 4, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<2, 3, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<3, 2, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<2, 4, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<4, 2, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<3, 4, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<4, 3, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, GLsizei count, const Math::RectangularMatrix<2, 2, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, GLsizei count, const Math::RectangularMatrix<3, 3, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, GLsizei count, const Math::RectangularMatrix<4, 4, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, GLsizei count, const Math::RectangularMatrix<2, 3, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, GLsizei count, const Math::RectangularMatrix<3, 2, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, GLsizei count, const Math::RectangularMatrix<2, 4, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, GLsizei count, const Math::RectangularMatrix<4, 2, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, GLsizei count, const Math::RectangularMatrix<3, 4, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, GLsizei count, const Math::RectangularMatrix<4, 3, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, GLsizei count, const Math::RectangularMatrix<2, 2, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, GLsizei count, const Math::RectangularMatrix<3, 3, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, GLsizei count, const Math::RectangularMatrix<4, 4, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, GLsizei count, const Math::RectangularMatrix<2, 3, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, GLsizei count, const Math::RectangularMatrix<3, 2, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, GLsizei count, const Math::RectangularMatrix<2, 4, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, GLsizei count, const Math::RectangularMatrix<4, 2, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, GLsizei count, const Math::RectangularMatrix<3, 4, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, GLsizei count, const Math::RectangularMatrix<4, 3, GLdouble>* values);
        #endif
        static UniformMatrix2fvImplementation uniformMatrix2fvImplementation;
        static UniformMatrix3fvImplementation uniformMatrix3fvImplementation;
        static UniformMatrix4fvImplementation uniformMatrix4fvImplementation;
        #ifndef MAGNUM_TARGET_GLES2
        static UniformMatrix2x3fvImplementation uniformMatrix2x3fvImplementation;
        static UniformMatrix3x2fvImplementation uniformMatrix3x2fvImplementation;
        static UniformMatrix2x4fvImplementation uniformMatrix2x4fvImplementation;
        static UniformMatrix4x2fvImplementation uniformMatrix4x2fvImplementation;
        static UniformMatrix3x4fvImplementation uniformMatrix3x4fvImplementation;
        static UniformMatrix4x3fvImplementation uniformMatrix4x3fvImplementation;
        #endif
        #ifndef MAGNUM_TARGET_GLES
        static UniformMatrix2dvImplementation uniformMatrix2dvImplementation;
        static UniformMatrix3dvImplementation uniformMatrix3dvImplementation;
        static UniformMatrix4dvImplementation uniformMatrix4dvImplementation;
        static UniformMatrix2x3dvImplementation uniformMatrix2x3dvImplementation;
        static UniformMatrix3x2dvImplementation uniformMatrix3x2dvImplementation;
        static UniformMatrix2x4dvImplementation uniformMatrix2x4dvImplementation;
        static UniformMatrix4x2dvImplementation uniformMatrix4x2dvImplementation;
        static UniformMatrix3x4dvImplementation uniformMatrix3x4dvImplementation;
        static UniformMatrix4x3dvImplementation uniformMatrix4x3dvImplementation;
        #endif

        GLuint _id;
};

/**
@brief Base struct for attribute location and type

Template parameter @p location is vertex attribute location, number between `0`
and @ref maxVertexAttributes(). To ensure compatibility, you should always have
vertex attribute with location `0`.

Template parameter @p T is the type which is used for shader attribute, e.g.
@ref Vector4i for `ivec4`. DataType is type of passed data when adding vertex
buffers to mesh. By default it is the same as type used in shader (e.g.
@ref DataType::Int for @ref Vector4i). It's also possible to pass integer data
to floating-point shader inputs. In this case you may want to normalize the
values (e.g. color components from 0-255 to 0.0f - 1.0f) -- see @ref DataOption::Normalize.

Only some types are allowed as attribute types, see @ref AbstractShaderProgram-types
or TypeTraits::AttributeType for more information.

See @ref AbstractShaderProgram-subclassing for example usage in shaders and
@ref Mesh-configuration for example usage when adding vertex buffers to mesh.
*/
template<UnsignedInt location, class T> class AbstractShaderProgram::Attribute {
    public:
        enum: UnsignedInt {
            Location = location /**< Location to which the attribute is bound */
        };

        /**
         * @brief Type
         *
         * Type used in shader code.
         * @see DataType
         */
        typedef typename Implementation::Attribute<T>::Type Type;

        /**
         * @brief Component count
         *
         * Count of components passed to the shader. If passing smaller count
         * of components than corresponding type has, unspecified components
         * are set to default values (second and third to `0`, fourth to `1`).
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        enum class Components: GLint {
            /**
             * Only first component is specified. Second, third and fourth
             * component are set to `0`, `0`, `1`, respectively. Only for
             * scalar and vector types, not matrices.
             */
            One = 1,

            /**
             * First two components are specified. Third and fourth component
             * are set to `0`, `1`, respectively. Only for two, three and
             * four-component vector types and 2x2, 3x2 and 4x2 matrix types.
             */
            Two = 2,

            /**
             * First three components are specified. Fourth component is set to
             * `1`. Only for three and four-component vector types, 2x3, 3x3
             * and 4x3 matrix types.
             */
            Three = 3,

            /**
             * All four components are specified. Only for four-component
             * vector types and 2x4, 3x4 and 4x4 matrix types.
             */
            Four = 4,

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Four components with BGRA ordering. Only for four-component
             * float vector type.
             * @requires_gl32 %Extension @extension{ARB,vertex_array_bgra}
             * @requires_gl Only RGBA component ordering is supported in OpenGL
             *      ES.
             */
            BGRA = 1 << 1
            #endif
        };
        #else
        typedef typename Implementation::Attribute<T>::Components Components;
        #endif

        /**
         * @brief Data type
         *
         * Type of data passed to shader.
         * @see Type, DataOptions, Attribute()
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        enum class DataType: GLenum {
            UnsignedByte = GL_UNSIGNED_BYTE,    /**< Unsigned byte */
            Byte = GL_BYTE,                     /**< Byte */
            UnsignedShort = GL_UNSIGNED_SHORT,  /**< Unsigned short */
            Short = GL_SHORT,                   /**< Short */
            UnsignedInt = GL_UNSIGNED_INT,      /**< Unsigned int */
            Int = GL_INT,                       /**< Int */

            /**
             * Half float. Only for float attribute types.
             * @requires_gl30 %Extension @extension{NV,half_float}
             * @requires_gles30 %Extension @es_extension{OES,vertex_half_float}
             */
            HalfFloat = GL_HALF_FLOAT,

            /** Float. Only for float attribute types. */
            Float = GL_FLOAT,

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Double. Only for float and double attribute types.
             * @requires_gl Only floats are available in OpenGL ES.
             */
            Double = GL_DOUBLE,
            #endif

            /* GL_FIXED not supported */

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Unsigned 2.10.10.10 packed integer. Only for four-component
             * float vector attribute type.
             * @todo How about (incompatible) @es_extension{OES,vertex_type_10_10_10_2}?
             * @requires_gl33 %Extension @extension{ARB,vertex_type_2_10_10_10_rev}
             * @requires_gles30 (no extension providing this functionality)
             */
            UnsignedInt2101010Rev = GL_UNSIGNED_INT_2_10_10_10_REV,

            /**
             * Signed 2.10.10.10 packed integer. Only for four-component float
             * vector attribute type.
             * @requires_gl33 %Extension @extension{ARB,vertex_type_2_10_10_10_rev}
             * @requires_gles30 (no extension providing this functionality)
             */
            Int2101010Rev = GL_INT_2_10_10_10_REV
            #endif
        };
        #else
        typedef typename Implementation::Attribute<T>::DataType DataType;
        #endif

        /**
         * @brief Data option
         * @see DataOptions, Attribute()
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        enum class DataOption: UnsignedByte {
            /**
             * Normalize integer components. Only for float attribute types.
             * Default is to not normalize.
             */
            Normalize = 1 << 0
        };
        #else
        typedef typename Implementation::Attribute<T>::DataOption DataOption;
        #endif

        /**
         * @brief Data options
         * @see Attribute()
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        typedef typename Containers::EnumSet<DataOption, UnsignedByte> DataOptions;
        #else
        typedef typename Implementation::Attribute<T>::DataOptions DataOptions;
        #endif

        /**
         * @brief Constructor
         * @param components    Component count
         * @param dataType      Type of passed data. Default is the same as
         *      type used in shader (e.g. DataType::Integer for Vector4i).
         * @param dataOptions   Data options. Default is no options.
         */
        constexpr Attribute(Components components, DataType dataType = Implementation::Attribute<T>::DefaultDataType, DataOptions dataOptions = DataOptions()): _components(components), _dataType(dataType), _dataOptions(dataOptions) {}

        /**
         * @brief Constructor
         * @param dataType      Type of passed data. Default is the same as
         *      type used in shader (e.g. @ref DataType::Int for @ref Vector4i).
         * @param dataOptions   Data options. Default is no options.
         *
         * Component count is set to the same value as in type used in shader
         * (e.g. @ref Components::Three for @ref Vector3).
         */
        constexpr Attribute(DataType dataType = Implementation::Attribute<T>::DefaultDataType, DataOptions dataOptions = DataOptions()): _components(Implementation::Attribute<T>::DefaultComponents), _dataType(dataType), _dataOptions(dataOptions) {}

        /** @brief Component count of passed data */
        constexpr Components components() const { return _components; }

        /** @brief Type of passed data */
        constexpr DataType dataType() const { return _dataType; }

        /** @brief Size of passed data */
        std::size_t dataSize() const {
            return Implementation::Attribute<T>::size(GLint(_components)*Implementation::Attribute<T>::vectorCount(), _dataType);
        }

        /** @brief Data options */
        constexpr DataOptions dataOptions() const { return _dataOptions; }

    private:
        Components _components;
        DataType _dataType;
        DataOptions _dataOptions;
};

#ifdef DOXYGEN_GENERATING_OUTPUT
/** @debugoperator{Magnum::AbstractShaderProgram::Attribute} */
template<class T> Debug operator<<(Debug debug, AbstractShaderProgram::Attribute<T>::Components);

/** @debugoperator{Magnum::AbstractShaderProgram::Attribute} */
template<class T> Debug operator<<(Debug debug, AbstractShaderProgram::Attribute<T>::DataType);
#endif

namespace Implementation {

/* Base for sized attributes */
template<std::size_t cols, std::size_t rows> struct SizedAttribute;

/* Vector attribute sizes */
template<std::size_t cols> struct SizedVectorAttribute {
    constexpr static std::size_t vectorCount() { return cols; }
};
template<> struct SizedAttribute<1, 1>: SizedVectorAttribute<1> {
    enum class Components: GLint { One = 1 };
    #if !defined(CORRADE_GCC45_COMPATIBILITY) && !defined(CORRADE_MSVC2013_COMPATIBILITY)
    constexpr static
    #else
    static const
    #endif
    Components DefaultComponents = Components::One;
};
template<> struct SizedAttribute<1, 2>: SizedVectorAttribute<1> {
    enum class Components: GLint { One = 1, Two = 2 };
    #if !defined(CORRADE_GCC45_COMPATIBILITY) && !defined(CORRADE_MSVC2013_COMPATIBILITY)
    constexpr static
    #else
    static const
    #endif
    Components DefaultComponents = Components::Two;
};
template<> struct SizedAttribute<1, 3>: SizedVectorAttribute<1> {
    enum class Components: GLint { One = 1, Two = 2, Three = 3 };
    #if !defined(CORRADE_GCC45_COMPATIBILITY) && !defined(CORRADE_MSVC2013_COMPATIBILITY)
    constexpr static
    #else
    static const
    #endif
    Components DefaultComponents = Components::Three;
};
template<> struct SizedAttribute<1, 4>: SizedVectorAttribute<1> {
    enum class Components: GLint { One = 1, Two = 2, Three = 3, Four = 4 };
    #if !defined(CORRADE_GCC45_COMPATIBILITY) && !defined(CORRADE_MSVC2013_COMPATIBILITY)
    constexpr static
    #else
    static const
    #endif
    Components DefaultComponents = Components::Four;
};
Debug MAGNUM_EXPORT operator<<(Debug debug, SizedAttribute<1, 1>::Components value);
Debug MAGNUM_EXPORT operator<<(Debug debug, SizedAttribute<1, 2>::Components value);
Debug MAGNUM_EXPORT operator<<(Debug debug, SizedAttribute<1, 3>::Components value);
Debug MAGNUM_EXPORT operator<<(Debug debug, SizedAttribute<1, 4>::Components value);

/* Matrix attribute sizes */
template<std::size_t rows> struct SizedMatrixAttribute;
template<> struct SizedMatrixAttribute<2> {
    enum class Components: GLint { Two   = 2 };
    #if !defined(CORRADE_GCC45_COMPATIBILITY) && !defined(CORRADE_MSVC2013_COMPATIBILITY)
    constexpr static
    #else
    static const
    #endif
    Components DefaultComponents = Components::Two;
};
template<> struct SizedMatrixAttribute<3> {
    enum class Components: GLint { Three = 3 };
    #if !defined(CORRADE_GCC45_COMPATIBILITY) && !defined(CORRADE_MSVC2013_COMPATIBILITY)
    constexpr static
    #else
    static const
    #endif
    Components DefaultComponents = Components::Three;
};
template<> struct SizedMatrixAttribute<4> {
    enum class Components: GLint { Four  = 4 };
    #if !defined(CORRADE_GCC45_COMPATIBILITY) && !defined(CORRADE_MSVC2013_COMPATIBILITY)
    constexpr static
    #else
    static const
    #endif
    Components DefaultComponents = Components::Four;
};
Debug MAGNUM_EXPORT operator<<(Debug debug, SizedMatrixAttribute<2>::Components value);
Debug MAGNUM_EXPORT operator<<(Debug debug, SizedMatrixAttribute<3>::Components value);
Debug MAGNUM_EXPORT operator<<(Debug debug, SizedMatrixAttribute<4>::Components value);
template<> struct SizedAttribute<2, 2>: SizedVectorAttribute<2>, SizedMatrixAttribute<2> {};
template<> struct SizedAttribute<3, 3>: SizedVectorAttribute<3>, SizedMatrixAttribute<3> {};
template<> struct SizedAttribute<4, 4>: SizedVectorAttribute<4>, SizedMatrixAttribute<4> {};
#ifndef MAGNUM_TARGET_GLES2
template<> struct SizedAttribute<2, 3>: SizedVectorAttribute<2>, SizedMatrixAttribute<3> {};
template<> struct SizedAttribute<3, 2>: SizedVectorAttribute<3>, SizedMatrixAttribute<2> {};
template<> struct SizedAttribute<2, 4>: SizedVectorAttribute<2>, SizedMatrixAttribute<4> {};
template<> struct SizedAttribute<4, 2>: SizedVectorAttribute<4>, SizedMatrixAttribute<2> {};
template<> struct SizedAttribute<3, 4>: SizedVectorAttribute<3>, SizedMatrixAttribute<4> {};
template<> struct SizedAttribute<4, 3>: SizedVectorAttribute<4>, SizedMatrixAttribute<3> {};
#endif

/* Base for attributes */
template<class> struct Attribute;

/* Base for float attributes */
struct FloatAttribute {
    typedef Float Type;

    enum class DataType: GLenum {
        UnsignedByte = GL_UNSIGNED_BYTE,
        Byte = GL_BYTE,
        UnsignedShort = GL_UNSIGNED_SHORT,
        Short = GL_SHORT,
        UnsignedInt = GL_UNSIGNED_INT,
        Int = GL_INT,
        #ifndef MAGNUM_TARGET_GLES2
        HalfFloat = GL_HALF_FLOAT,
        #else
        HalfFloat = GL_HALF_FLOAT_OES,
        #endif
        Float = GL_FLOAT

        #ifndef MAGNUM_TARGET_GLES
        ,
        Double = GL_DOUBLE
        #endif
    };
    #if !defined(CORRADE_GCC45_COMPATIBILITY) && !defined(CORRADE_MSVC2013_COMPATIBILITY)
    constexpr static
    #else
    static const
    #endif
    DataType DefaultDataType = DataType::Float;

    enum class DataOption: UnsignedByte {
        Normalized = 1 << 0
    };
    typedef Containers::EnumSet<DataOption, UnsignedByte> DataOptions;

    static std::size_t MAGNUM_EXPORT size(GLint components, DataType dataType);
};

CORRADE_ENUMSET_OPERATORS(FloatAttribute::DataOptions)

Debug MAGNUM_EXPORT operator<<(Debug debug, FloatAttribute::DataType value);

#ifndef MAGNUM_TARGET_GLES2
/* Base for int attributes */
struct IntAttribute {
    typedef Int Type;

    enum class DataType: GLenum {
        UnsignedByte = GL_UNSIGNED_BYTE,
        Byte = GL_BYTE,
        UnsignedShort = GL_UNSIGNED_SHORT,
        Short = GL_SHORT,
        UnsignedInt = GL_UNSIGNED_INT,
        Int = GL_INT
    };
    #if !defined(CORRADE_GCC45_COMPATIBILITY) && !defined(CORRADE_MSVC2013_COMPATIBILITY)
    constexpr static
    #else
    static const
    #endif
    DataType DefaultDataType = DataType::Int;

    enum class DataOption: UnsignedByte {};
    typedef Containers::EnumSet<DataOption, UnsignedByte> DataOptions;

    static std::size_t MAGNUM_EXPORT size(GLint components, DataType dataType);
};

Debug MAGNUM_EXPORT operator<<(Debug debug, IntAttribute::DataType value);

/* Base for unsigned int attributes */
struct UnsignedIntAttribute {
    typedef UnsignedInt Type;

    typedef IntAttribute::DataType DataType;
    #if !defined(CORRADE_GCC45_COMPATIBILITY) && !defined(CORRADE_MSVC2013_COMPATIBILITY)
    constexpr static
    #else
    static const
    #endif
    DataType DefaultDataType = DataType::UnsignedInt;

    typedef IntAttribute::DataOption DataOption;
    typedef Containers::EnumSet<DataOption, UnsignedByte> DataOptions;

    static std::size_t size(GLint components, DataType dataType) {
        return IntAttribute::size(components, dataType);
    }
};
#endif

#ifndef MAGNUM_TARGET_GLES
/* Base for double attributes */
struct DoubleAttribute {
    typedef Double Type;

    enum class DataType: GLenum {
        Double = GL_DOUBLE
    };
    #if !defined(CORRADE_GCC45_COMPATIBILITY) && !defined(CORRADE_MSVC2013_COMPATIBILITY)
    constexpr static
    #else
    static const
    #endif
    DataType DefaultDataType = DataType::Double;

    enum class DataOption: UnsignedByte {};
    typedef Containers::EnumSet<DataOption, UnsignedByte> DataOptions;

    static std::size_t MAGNUM_EXPORT size(GLint components, DataType dataType);
};

Debug MAGNUM_EXPORT operator<<(Debug debug, DoubleAttribute::DataType value);
#endif

/* Floating-point four-component vector is absolutely special case */
template<> struct Attribute<Math::Vector<4, Float>> {
    typedef Float Type;

    enum class Components: GLint {
        One = 1,
        Two = 2,
        Three = 3,
        Four = 4
        #ifndef MAGNUM_TARGET_GLES
        ,
        BGRA = GL_BGRA
        #endif
    };
    #if !defined(CORRADE_GCC45_COMPATIBILITY) && !defined(CORRADE_MSVC2013_COMPATIBILITY)
    constexpr static
    #else
    static const
    #endif
    Components DefaultComponents = Components::Four;

    enum class DataType: GLenum {
        UnsignedByte = GL_UNSIGNED_BYTE,
        Byte = GL_BYTE,
        UnsignedShort = GL_UNSIGNED_SHORT,
        Short = GL_SHORT,
        UnsignedInt = GL_UNSIGNED_INT,
        Int = GL_INT,
        #ifndef MAGNUM_TARGET_GLES2
        HalfFloat = GL_HALF_FLOAT,
        #else
        HalfFloat = GL_HALF_FLOAT_OES,
        #endif
        Float = GL_FLOAT
        #ifndef MAGNUM_TARGET_GLES
        ,
        Double = GL_DOUBLE
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        ,
        UnsignedInt2101010Rev = GL_UNSIGNED_INT_2_10_10_10_REV,
        Int2101010Rev = GL_INT_2_10_10_10_REV
        #endif
    };
    #if !defined(CORRADE_GCC45_COMPATIBILITY) && !defined(CORRADE_MSVC2013_COMPATIBILITY)
    constexpr static
    #else
    static const
    #endif
    DataType DefaultDataType = DataType::Float;

    enum class DataOption: UnsignedByte {
        Normalized = 1 << 0
    };
    typedef Containers::EnumSet<DataOption, UnsignedByte> DataOptions;

    constexpr static std::size_t vectorCount() { return 1; }

    static std::size_t MAGNUM_EXPORT size(GLint components, DataType dataType);
};

typedef Math::Vector<4, Float> _Vector4;
CORRADE_ENUMSET_OPERATORS(Attribute<_Vector4>::DataOptions)

Debug MAGNUM_EXPORT operator<<(Debug debug, Attribute<Math::Vector<4, Float>>::Components value);
Debug MAGNUM_EXPORT operator<<(Debug debug, Attribute<Math::Vector<4, Float>>::DataType value);

/* Common float, int, unsigned int and double scalar attributes */
template<> struct Attribute<Float>: FloatAttribute, SizedAttribute<1, 1> {};
#ifndef MAGNUM_TARGET_GLES2
template<> struct Attribute<Int>: IntAttribute, SizedAttribute<1, 1> {};
template<> struct Attribute<UnsignedInt>: UnsignedIntAttribute, SizedAttribute<1, 1> {};
#ifndef MAGNUM_TARGET_GLES
template<> struct Attribute<Double>: DoubleAttribute, SizedAttribute<1, 1> {};
#endif
#endif

/* Common float, int, unsigned int and double vector attributes */
template<std::size_t size_> struct Attribute<Math::Vector<size_, Float>>: FloatAttribute, SizedAttribute<1, size_> {};
#ifndef MAGNUM_TARGET_GLES2
template<std::size_t size_> struct Attribute<Math::Vector<size_, Int>>: IntAttribute, SizedAttribute<1, size_> {};
template<std::size_t size_> struct Attribute<Math::Vector<size_, UnsignedInt>>: UnsignedIntAttribute, SizedAttribute<1, size_> {};
#ifndef MAGNUM_TARGET_GLES
template<std::size_t size_> struct Attribute<Math::Vector<size_, Double>>: DoubleAttribute, SizedAttribute<1, size_> {};
#endif
#endif
template<class T> struct Attribute<Math::Vector2<T>>: Attribute<Math::Vector<2, T>> {};
template<class T> struct Attribute<Math::Vector3<T>>: Attribute<Math::Vector<3, T>> {};
template<class T> struct Attribute<Math::Vector4<T>>: Attribute<Math::Vector<4, T>> {};
template<class T> struct Attribute<BasicColor3<T>>: Attribute<Math::Vector3<T>> {};
template<class T> struct Attribute<BasicColor4<T>>: Attribute<Math::Vector4<T>> {};

/* Common float and double rectangular matrix attributes */
template<std::size_t cols, std::size_t rows> struct Attribute<Math::RectangularMatrix<cols, rows, Float>>: FloatAttribute, SizedAttribute<cols, rows> {};
#ifndef MAGNUM_TARGET_GLES
template<std::size_t cols, std::size_t rows> struct Attribute<Math::RectangularMatrix<cols, rows, Double>>: DoubleAttribute, SizedAttribute<cols, rows> {};
#endif

/* Common float and double square matrix attributes */
template<std::size_t size_> struct Attribute<Math::Matrix<size_, Float>>: Attribute<Math::RectangularMatrix<size_, size_, Float>> {};
#ifndef MAGNUM_TARGET_GLES
template<std::size_t size_> struct Attribute<Math::Matrix<size_, Double>>: Attribute<Math::RectangularMatrix<size_, size_, Double>> {};
#endif
template<class T> struct Attribute<Math::Matrix3<T>>: Attribute<Math::Matrix<3, T>> {};
template<class T> struct Attribute<Math::Matrix4<T>>: Attribute<Math::Matrix<4, T>> {};

}

}

#endif
