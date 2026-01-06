#include "sound.hpp"

#include <cstdio>

constexpr std::size_t SAMPLE_SIZE = 512;
constexpr std::size_t NUM_TRACKS = 200;

sound::sound()
{
    printf("Initing sound\n");

    sound::mTracks.resize(NUM_TRACKS);

    SDL_AudioSpec desired {};
    desired.channels = 2;
    desired.freq = 44100;
    desired.format = AUDIO_S16SYS;
    desired.samples = SAMPLE_SIZE;
    desired.callback = sound::bufferCallback;
    desired.userdata = this;

    if (SDL_OpenAudio(&desired, nullptr) < 0) {
        fprintf(stderr, "Unable to open audio: %s\n", SDL_GetError());
    }

    SDL_LockAudio();
    for (auto& track: mTracks) {
        track.playing = false;
        track.paused = false;
        track.speed = 1;
    }
    SDL_UnlockAudio();

    mLeftSamples.resize(SAMPLE_SIZE * 4);
    mRightSamples.resize(SAMPLE_SIZE * 4);
}

sound::~sound()
{
    stopSound();
    stopAllTracks();

    SDL_CloseAudio();
}

void sound::bufferCallback(void* userdata, Uint8* stream, int len)
{
    Sint16* buf = (Sint16*)stream;

    auto thisPtr = static_cast<sound*>(userdata);

    memset(buf, 0, len);

    memset(thisPtr->mLeftSamples.data(), 0, len * sizeof(float));
    memset(thisPtr->mRightSamples.data(), 0, len * sizeof(float));

    len /= 2;

    constexpr int max_int_val = (1 << 16);

    // Fill the input buffers

    for (auto& track: thisPtr->mTracks) {
        int rIndex = 0;
        int lIndex = 0;

        if (track.playing && !track.paused) {
            Sint16* data = (Sint16*)track.data.data();

            for (int s = 0; s < len; s++) {
                // Stream the audio data
                if (s & 1) {
                    // Right channel
                    float fPos = track.pos;
                    float iPos = (int)fPos;

                    float v1 = (data[(int)iPos] * (track.vol / 2));
                    float fval = (v1 / max_int_val);

                    thisPtr->mRightSamples[rIndex] += fval;
                    ++rIndex;
                } else {
                    // Left channel
                    float fPos = track.pos;
                    float iPos = (int)fPos;

                    float v1 = (data[(int)iPos] * (track.vol / 2));
                    float fval = (v1 / max_int_val);

                    thisPtr->mLeftSamples[lIndex] += fval;
                    ++lIndex;
                }

                // Advance the track data position

                track.pos += track.speed;

                if (track.pos >= track.len) {
                    if (track.loop) {
                        track.pos -= track.len;
                    } else {
                        track.pos = 0;
                        track.playing = false;
                        break;
                    }
                }
            }
        }
    }

    // Fill the output buffer
    for (int i = 0, b = 0; i < len / 2; i++) {
        Sint16 left = (thisPtr->mLeftSamples[i]) * (max_int_val);
        Sint16 right = (thisPtr->mRightSamples[i]) * (max_int_val);
        buf[b++] = left;
        buf[b++] = right;
    }
}

void sound::loadTrack(const char* file, std::size_t track, float volume, bool loop /*=false*/)
{
    SDL_AudioSpec wave;
    SDL_AudioSpec desired;
    Uint8* data;
    Uint32 dlen;
    SDL_AudioCVT cvt;

    // Load the sound file and convert it to 16-bit stereo at 44Hz
    if (SDL_LoadWAV(file, &wave, &data, &dlen) == nullptr) {
        printf("Failed loading audio track %zu: %s\n", track, SDL_GetError());
        return;
    }

    printf("Loaded audio track %zu\n", track);

    desired = wave;
    desired.channels = 2;
    desired.freq = 44100;
    desired.format = AUDIO_S16SYS;
    desired.samples = SAMPLE_SIZE;
    desired.callback = sound::bufferCallback;

    SDL_BuildAudioCVT(&cvt,
                      wave.format,
                      wave.channels,
                      wave.freq,
                      AUDIO_S16SYS,
                      2,
                      44100);

    std::vector<Uint8> buffer(dlen * cvt.len_mult);
    cvt.buf = buffer.data();
    memcpy(cvt.buf, data, dlen);
    cvt.len = dlen;
    SDL_FreeWAV(data);

    if (SDL_ConvertAudio(&cvt) != 0) {
        printf("Failed to convert track %zu: %s\n", track, SDL_GetError());
    }

    SDL_LockAudio();
    mTracks[track].data = buffer;
    mTracks[track].len = cvt.len_cvt / 2;
    mTracks[track].pos = 0;
    mTracks[track].loop = loop;
    mTracks[track].vol = volume;
    mTracks[track].playing = false;
    SDL_UnlockAudio();
}

void sound::startSound()
{
    SDL_PauseAudio(0);
}

void sound::stopSound()
{
    SDL_PauseAudio(1);
}

void sound::playTrack(std::size_t track)
{
    SDL_LockAudio();
    mTracks[track].pos = 0;
    mTracks[track].playing = true;
    SDL_UnlockAudio();
}

void sound::stopTrack(std::size_t track)
{
    SDL_LockAudio();
    mTracks[track].pos = 0;
    mTracks[track].playing = false;
    SDL_UnlockAudio();
}

void sound::stopAllTracks()
{
    SDL_LockAudio();
    for (auto& track: mTracks) {
        track.pos = 0;
        track.playing = false;
    }
    SDL_UnlockAudio();
}

void sound::stopAllTracksBut(std::size_t track)
{
    SDL_LockAudio();
    for (std::size_t i = 0; i < NUM_TRACKS; i++) {
        if (i != track) {
            mTracks[i].pos = 0;
            mTracks[i].playing = false;
        }
    }
    SDL_UnlockAudio();
}

void sound::pauseTrack(std::size_t track)
{
    SDL_LockAudio();
    mTracks[track].paused = true;
    SDL_UnlockAudio();
}

void sound::unpauseTrack(std::size_t track)
{
    SDL_LockAudio();
    mTracks[track].paused = false;
    SDL_UnlockAudio();
}

void sound::pauseAllTracks()
{
    SDL_LockAudio();
    for (auto& track: mTracks) {
        track.paused = true;
    }
    SDL_UnlockAudio();
}

void sound::pauseAllTracksBut(std::size_t track)
{
    SDL_LockAudio();
    for (std::size_t i = 0; i < NUM_TRACKS; i++) {
        if (i != track) {
            mTracks[i].paused = true;
        }
    }
    SDL_UnlockAudio();
}

void sound::unpauseAllTracks()
{
    SDL_LockAudio();
    for (auto& track: mTracks) {
        track.paused = false;
    }
    SDL_UnlockAudio();
}

void sound::setTrackSpeed(std::size_t track, double speed)
{
    SDL_LockAudio();
    mTracks[track].speed = speed;
    SDL_UnlockAudio();
}

bool sound::isTrackPlaying(std::size_t track)
{
    SDL_LockAudio();
    bool playing = mTracks[track].playing;
    SDL_UnlockAudio();
    return playing;
}

int sound::playTrackGroup(std::size_t trackFirst, std::size_t trackLast)
{
    for (std::size_t i = trackFirst; i <= trackLast; i++) {
        if (!isTrackPlaying(i)) {
            playTrack(i);
            return i;
        }
    }
    // Just retrigger the first one
    playTrack(trackFirst);
    return trackFirst;
}
