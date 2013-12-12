#ifndef Magnum_Audio_Source_h
#define Magnum_Audio_Source_h
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
 * @brief Class Magnum::Audio::Source
 */

#include <initializer_list>
#include <vector>
#include <al.h>

#include "Math/Vector3.h"
#include "Magnum.h"
#include "Audio/Audio.h"
#include "Audio/magnumAudioVisibility.h"

namespace Magnum { namespace Audio {

/**
@brief %Source

Manages positional audio source.
@todo Expose convenient API for buffer queuing
*/
class MAGNUM_AUDIO_EXPORT Source {
    public:
        /**
         * @brief Constructor
         *
         * Creates OpenAL source object.
         * @see @fn_al{GenSources}
         */
        explicit Source() { alGenSources(1, &_id); }

        /**
         * @brief Destructor
         *
         * Deletes OpenAL source object.
         * @see @fn_al{DeleteSources}
         */
        ~Source() { if(_id) alDeleteSources(1, &_id); }

        /** @brief Copying is not allowed */
        Source(const Source&) = delete;

        /** @brief Move constructor */
        Source(Source&& other);

        /** @brief Copying is not allowed */
        Source& operator=(const Source&) = delete;

        /** @brief Move assignment */
        Source& operator=(Source&& other);

        /** @brief OpenAL source ID */
        ALuint id() const { return _id; }

        /** @{ @name Source positioning */

        /**
         * @brief Set position
         * @return Reference to self (for method chaining)
         *
         * Default is `{0.0f, 0.0f, 0.0f}`.
         * @see @ref setRelative(), @fn_al{Sourcefv} with @def_al{POSITION}
         */
        Source& setPosition(const Vector3& position) {
            alSourcefv(_id, AL_POSITION, position.data());
            return *this;
        }

        /** @overload
         * @see @fn_al{Sourceiv} with @def_al{POSITION}
         */
        Source& setPosition(const Vector3i& position) {
            alSourceiv(_id, AL_POSITION, position.data());
            return *this;
        }

        /**
         * @brief Set velocity
         * @return Reference to self (for method chaining)
         *
         * Default is `{0.0f, 0.0f, 0.0f}`.
         * @see @ref setRelative(), @fn_al{Sourcefv} with @def_al{VELOCITY}
         */
        Source& setVelocity(const Vector3& velocity) {
            alSourcefv(_id, AL_VELOCITY, velocity.data());
            return *this;
        }

        /** @overload
         * @see @fn_al{Sourceiv} with @def_al{VELOCITY}
         */
        Source& setVelocity(const Vector3i& velocity) {
            alSourceiv(_id, AL_VELOCITY, velocity.data());
            return *this;
        }

        /**
         * @brief Interpret source relatively to listener
         *
         * When enabled, source position, direction and velocity will be
         * interpreted relatively to listener. Default is `false`.
         * @see @ref setPosition(), @ref setDirection(), @ref setVelocity(),
         *      @fn_al{Sourcei} with @def_al{SOURCE_RELATIVE}
         */
        Source& setRelative(bool relative) {
            alSourcei(_id, AL_SOURCE_RELATIVE, relative);
            return *this;
        }

        /*@}*/

        /** @{ @name Source behavior */

        /**
         * @brief Set gain
         * @return Reference to self (for method chaining)
         *
         * Default is `1.0f`, which means that the sound is unattenuated.
         * If set to `0.0f`, the source is muted.
         * @see @ref setMinGain(), @ref setMaxGain(), @fn_al{Sourcef} with
         *      @def_al{GAIN}
         */
        Source& setGain(Float gain) {
            alSourcef(_id, AL_GAIN, gain);
            return *this;
        }

        /**
         * @brief Set min gain
         * @return Reference to self (for method chaining)
         *
         * If effective gain is lower than min gain, min gain is used. Note
         * that this is done before listener gain is applied. Default is
         * `0.0f`.
         * @see @ref setMinGain(), @ref setGain(), @fn_al{Sourcef} with
         *      @def_al{MIN_GAIN}
         */
        Source& setMinGain(Float gain) {
            alSourcef(_id, AL_MIN_GAIN, gain);
            return *this;
        }

        /**
         * @brief Set max gain
         * @return Reference to self (for method chaining)
         *
         * If effective gain is higher than max gain, max gain is used. Note
         * that this is done before listener gain is applied. Default is
         * `1.0f`. If set to `0.0f`, the source is muted.
         * @see @ref setMinGain(), @ref setGain(), @fn_al{Sourcef} with
         *      @def_al{MIN_GAIN}
         */
        Source& setMaxGain(Float gain) {
            alSourcef(_id, AL_MAX_GAIN, gain);
            return *this;
        }

        /**
         * @brief Set reference distance
         * @return Reference to self (for method chaining)
         *
         * Default is `1.0f`.
         * @see @ref setRolloffFactor(), @fn_al{Sourcef} with
         *      @def_al{REFERENCE_DISTANCE}
         */
        Source& setReferenceDistance(Float distance) {
            alSourcef(_id, AL_REFERENCE_DISTANCE, distance);
            return *this;
        }

        /** @overload
         * @see @fn_al{Sourcei} with @def_al{REFERENCE_DISTANCE}
         */
        Source& setReferenceDistance(Int distance) {
            alSourcei(_id, AL_REFERENCE_DISTANCE, distance);
            return *this;
        }

        /**
         * @brief Set rolloff factor
         * @return Reference to self (for method chaining)
         *
         * Default is `1.0f`.
         * @see @ref setReferenceDistance(), @fn_al{Sourcef} with
         *      @def_al{ROLLOFF_FACTOR}
         */
        Source& setRolloffFactor(Float factor) {
            alSourcef(_id, AL_ROLLOFF_FACTOR, factor);
            return *this;
        }

        /** @overload
         * @see @fn_al{Sourcei} with @def_al{ROLLOFF_FACTOR}
         */
        Source& setRolloffFactor(Int factor) {
            alSourcei(_id, AL_ROLLOFF_FACTOR, factor);
            return *this;
        }

        /**
         * @brief Set max distance
         * @return Reference to self (for method chaining)
         *
         * Default is max representable value.
         * @see @fn_al{Sourcef} with @def_al{MAX_DISTANCE}
         */
        Source& setMaxDistance(Float distance) {
            alSourcef(_id, AL_MAX_DISTANCE, distance);
            return *this;
        }

        /** @overload
         * @see @fn_al{Sourcei} with @def_al{MAX_DISTANCE}
         */
        Source& setMaxDistance(Int distance) {
            alSourcef(_id, AL_MAX_DISTANCE, distance);
            return *this;
        }

        /**
         * @brief Set direction
         * @return Reference to self (for method chaining)
         *
         * Default is `{0.0f, 0.0f, 0.0f}`, which means that the source is not
         * directional.
         * @see @ref setInnerConeAngle(), @ref setOuterConeAngle(),
         *      @ref setRelative(), @fn_al{Sourcefv} with @def_al{DIRECTION}
         */
        Source& setDirection(const Vector3& direction) {
            alSourcefv(_id, AL_DIRECTION, direction.data());
            return *this;
        }

        /** @overload
         * @see @fn_al{Sourceiv} with @def_al{DIRECTION}
         */
        Source& setDirection(const Vector3i& direction) {
            alSourceiv(_id, AL_DIRECTION, direction.data());
            return *this;
        }

        /**
         * @brief Set inner cone angle
         * @return Reference to self (for method chaining)
         *
         * Has effect only if the source is directional. Default is
         * `360.0_degf`.
         * @see @ref setOuterConeAngle(), @ref setDirection(), @fn_al{Sourcef}
         *      with @def_al{CONE_INNER_ANGLE}
         */
        Source& setInnerConeAngle(Deg angle) {
            alSourcef(_id, AL_CONE_INNER_ANGLE, angle.toUnderlyingType());
            return *this;
        }

        /**
         * @brief Set outer cone angle
         * @return Reference to self (for method chaining)
         *
         * Has effect only if the source is directional. Default is
         * `360.0_degf`.
         * @see @ref setInnerConeAngle(), @ref setDirection(),
         *      @ref setOuterConeGain() @fn_al{Sourcef} with
         *      @def_al{CONE_OUTER_ANGLE}
         */
        Source& setOuterConeAngle(Deg angle) {
            alSourcef(_id, AL_CONE_OUTER_ANGLE, angle.toUnderlyingType());
            return *this;
        }

        /**
         * @brief Set outer cone gain multiplier
         * @return Reference to self (for method chaining)
         *
         * The factor with which the gain is multiplied outside the outer cone.
         * Default is `0.0f`.
         * @see @ref setGain(), @ref setOuterConeAngle(), @fn_al{Sourcef} with
         *      @def_al{CONE_OUTER_GAIN}
         */
        Source& setOuterConeGain(Float multiplier) {
            alSourcef(_id, AL_CONE_OUTER_ANGLE, multiplier);
            return *this;
        }

        /**
         * @brief Set pitch
         * @return Reference to self (for method chaining)
         *
         * Default is `1.0f`.
         * @see @fn_al{Sourcef} with @def_al{PITCH}
         */
        Source& setPitch(Float pitch) {
            alSourcef(_id, AL_PITCH, pitch);
            return *this;
        }

        /*@}*/

        /** @{ @name Buffer management */

        /**
         * @brief %Source type
         *
         * @see @ref type()
         */
        enum class Type: ALint {
            Undetermined = AL_UNDETERMINED, /**< Undetermined (default) */
            Static = AL_STATIC,             /**< Static source */
            Streaming = AL_STREAMING        /**< Streaming source */
        };

        /**
         * @brief Source type
         *
         * @see @ref setBuffer(), @fn_al{GetSourcei} with @def_al{SOURCE_TYPE}
         */
        Type type() const;

        /**
         * @brief Attach buffer
         * @param buffer        Buffer to attach or `nullptr`
         * @return Reference to self (for method chaining)
         *
         * If an buffer is attached, changes source type to @ref Type::Static,
         * if detached, changes source type to @ref Type::Undetermined. The
         * buffer must be already filled with data.
         * @see @ref type(), @fn_al{Sourcei} with @def_al{BUFFER}
         */
        Source& setBuffer(Buffer* buffer);

        /*@}*/

        /** @{ @name State management */

        /**
         * @brief %Source state
         *
         * @see @ref state(), @ref play(), @ref pause(), @ref stop(),
         *      @ref rewind()
         */
        enum class State: ALint {
            Initial = AL_INITIAL,   /**< Initial state (default) */
            Playing = AL_PLAYING,   /**< The source is playing */
            Paused = AL_PAUSED,     /**< The source is paused */
            Stopped = AL_STOPPED    /**< The source is stopped */
        };

        /**
         * @brief Play more sources at once
         *
         * The operation is guaranteed to be done for all sources at the same
         * time. `nullptr` is not allowed.
         * @see @ref play(), @ref pause(std::initializer_list<Source*>),
         *      @ref stop(std::initializer_list<Source*>),
         *      @ref rewind(std::initializer_list<Source*>),
         *      @fn_al{SourcePlayv}
         */
        static void play(std::initializer_list<Source*> sources);
        static void play(const std::vector<Source*>& sources); /**< @overload */

        /**
         * @brief Pause more sources at once
         *
         * The operation is guaranteed to be done for all sources at the same
         * time. `nullptr` is not allowed.
         * @see @ref pause(), @ref play(std::initializer_list<Source*>),
         *      @ref stop(std::initializer_list<Source*>),
         *      @ref rewind(std::initializer_list<Source*>),
         *      @fn_al{SourcePausev}
         */
        static void pause(std::initializer_list<Source*> sources);
        static void pause(const std::vector<Source*>& sources); /**< @overload */

        /**
         * @brief Stop more sources at once
         *
         * The operation is guaranteed to be done for all sources at the same
         * time. `nullptr` is not allowed.
         * @see @ref stop(), @ref play(std::initializer_list<Source*>),
         *      @ref pause(std::initializer_list<Source*>),
         *      @ref rewind(std::initializer_list<Source*>),
         *      @fn_al{SourceStopv}
         */
        static void stop(std::initializer_list<Source*> sources);
        static void stop(const std::vector<Source*>& sources); /**< @overload */

        /**
         * @brief Rewind more sources at once
         *
         * The operation is guaranteed to be done for all sources at the same
         * time. `nullptr` is not allowed.
         * @see @ref rewind(), @ref play(std::initializer_list<Source*>),
         *      @ref pause(std::initializer_list<Source*>),
         *      @ref stop(std::initializer_list<Source*>),
         *      @fn_al{SourceRewindv}
         */
        static void rewind(std::initializer_list<Source*> sources);
        static void rewind(const std::vector<Source*>& sources); /**< @overload */

        /**
         * @brief State
         *
         * @see @ref play(), @ref pause(), @ref stop(), @ref rewind(),
         *      @fn_al{GetSourcei} with @def_al{SOURCE_STATE}
         */
        State state() const;

        /**
         * @brief Play
         *
         * @see @ref play(std::initializer_list<Source*>), @ref state(),
         *      @ref pause(), @ref stop(), @ref rewind(), @fn_al{SourcePlay}
         */
        void play() { alSourcePlay(_id); }

        /**
         * @brief Pause
         *
         * @see @ref pause(std::initializer_list<Source*>), @ref state(),
         *      @ref play(), @ref stop(), @ref rewind(), @fn_al{SourcePause}
         */
        void pause() { alSourcePause(_id); }

        /**
         * @brief Stop
         *
         * @see @ref stop(std::initializer_list<Source*>), @ref state(),
         *      @ref play(), @ref pause(), @ref rewind(), @fn_al{SourceStop}
         */
        void stop() { alSourceStop(_id); }

        /**
         * @brief Rewind
         *
         * @see @ref rewind(std::initializer_list<Source*>), @ref state(),
         *      @ref play(), @ref pause(), @ref stop(), @fn_al{SourceRewind}
         */
        void rewind() { alSourceRewind(_id); }

        /**
         * @brief Whether the source is looping
         *
         * @see @fn_al{GetSourcei} with @def_al{LOOPING}
         */
        bool isLooping() const;

        /**
         * @brief Set source looping
         * @return Reference to self (for method chaining)
         *
         * Default is `false`.
         * @see @fn_al{Sourcei} with @def_al{LOOPING}
         */
        Source& setLooping(bool loop) {
            alSourcei(_id, AL_LOOPING, loop);
            return *this;
        }

        /**
         * @brief Offset in seconds
         *
         * @see @ref offsetInBytes(), @ref offsetInSamples(),
         *      @fn_al{GetSourcef} with @def_al{SEC_OFFSET}
         */
        Float offsetInSeconds() const;

        /**
         * @brief Set offset in seconds
         * @return Reference to self (for method chaining)
         *
         * @see @ref setOffsetInBytes(), @ref setOffsetInSamples(),
         *      @fn_al{Sourcef} with @def_al{SEC_OFFSET}
         */
        Source& setOffsetInSeconds(Float offset) {
            alSourcef(_id, AL_SEC_OFFSET, offset);
            return *this;
        }

        /**
         * @brief Offset in bytes
         *
         * @see @ref offsetInSeconds(), @ref offsetInSamples(),
         *      @fn_al{GetSourcei} with @def_al{BYTE_OFFSET}
         */
        Int offsetInBytes() const;

        /**
         * @brief Set offset in bytes
         * @return Reference to self (for method chaining)
         *
         * @see @ref setOffsetInSeconds(), @ref setOffsetInSamples(),
         *      @fn_al{Sourcei} with @def_al{SEC_OFFSET}
         */
        Source& setOffsetInBytes(Int offset) {
            alSourcei(_id, AL_BYTE_OFFSET, offset);
            return *this;
        }

        /**
         * @brief Offset in samples
         *
         * @see @ref offsetInSeconds(), @ref offsetInBytes(),
         *      @fn_al{GetSourcei} with @def_al{SAMPLE_OFFSET}
         */
        Int offsetInSamples() const;

        /**
         * @brief Set offset in samples
         * @return Reference to self (for method chaining)
         *
         * @see @ref setOffsetInSeconds(), @ref setOffsetInBytes(),
         *      @fn_al{Sourcei} with @def_al{SEC_OFFSET}
         */
        Source& setOffsetInSamples(Int offset) {
            alSourcei(_id, AL_SAMPLE_OFFSET, offset);
            return *this;
        }

        /*@}*/

    private:
        ALuint _id;
};

/** @debugoperator{Magnum::Audio::Source} */
Debug MAGNUM_AUDIO_EXPORT operator<<(Debug debug, Source::State value);

inline Source::Source(Source&& other): _id(other._id) {
    other._id = 0;
}

inline Source& Source::operator=(Source&& other) {
    std::swap(_id, other._id);
    return *this;
}

inline auto Source::state() const -> State {
    ALint state;
    alGetSourcei(_id, AL_SOURCE_STATE, &state);
    return State(state);
}

inline bool Source::isLooping() const {
    ALint looping;
    alGetSourcei(_id, AL_LOOPING, &looping);
    return looping;
}

inline Float Source::offsetInSeconds() const {
    Float offset;
    alGetSourcef(_id, AL_SEC_OFFSET, &offset);
    return offset;
}

inline Int Source::offsetInBytes() const {
    Int offset;
    alGetSourcei(_id, AL_BYTE_OFFSET, &offset);
    return offset;
}

inline Int Source::offsetInSamples() const {
    Int offset;
    alGetSourcei(_id, AL_SAMPLE_OFFSET, &offset);
    return offset;
}

}}

#endif
