#include "sound.h"
#include "game.h"


#define SAMPLE_SIZE 512
#define NUM_TRACKS  200

sound::TRACK* sound::mTracks;
float* sound::mLeftSamples;
float* sound::mRightSamples;


sound::sound()
{
    OutputDebugString(L"Initing sound\n");

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK);

    sound::mTracks = new TRACK[NUM_TRACKS];

    for (int i=0; i < NUM_TRACKS; i++)
    {
        mTracks[i].data = NULL;
        mTracks[i].playing = FALSE;
        mTracks[i].paused = FALSE;
        mTracks[i].speed = 1;
    }

    mLeftSamples = new float [SAMPLE_SIZE*4];
    mRightSamples = new float [SAMPLE_SIZE*4];
}

sound::~sound()
{
    stopSound();
	stopAllTracks();

    SDL_LockAudio();

    for (int i=0; i < NUM_TRACKS; i++)
    {
        if (mTracks[i].data)
            delete mTracks[i].data;
    }

    delete sound::mTracks;

    delete mLeftSamples;
    delete mRightSamples;

    SDL_UnlockAudio();

    SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK);
}

void sound::bufferCallback(void *unused, Uint8 *stream, int len)
{
	Sint16 *buf = (Sint16 *)stream;

    // Clear the buffers
    memset(buf, 0, len);

    memset(mLeftSamples, 0, len * sizeof(float));
    memset(mRightSamples, 0, len * sizeof(float));

    len /= 2;

    int i, s;

	const int max_audioval = ((1<<(16-1))-1);
	const int min_audioval = -(1<<(16-1));
    const int max_int_val = (1<<16);

    // Fill the input buffers

    for (i=0; i<NUM_TRACKS; ++i)
    {
        int rIndex = 0;
        int lIndex = 0;

        TRACK* track = &mTracks[i];

        if (track->playing && !track->paused)
        {
        	Sint16 *data = (Sint16 *)track->data;

		    for (s=0; s < len; s++)
		    {
                // Stream the audio data
                if (s&1)
                {
                    // Right channel
                    float fPos = track->pos;
                    float iPos = (int)fPos;

                    float v1 = (data[(int)iPos] * (track->vol/2));
                    float fval = (v1 / max_int_val);


                    mRightSamples[rIndex] += fval;
                    ++rIndex;
                }
                else
                {
                    // Left channel
                    float fPos = track->pos;
                    float iPos = (int)fPos;

                    float v1 = (data[(int)iPos] * (track->vol/2));
                    float fval = (v1 / max_int_val);

                    mLeftSamples[lIndex] += fval;
                    ++lIndex;
                }

                // Advance the track data position

                track->pos += track->speed;

			    if (track->pos >= track->len)
			    {
                    if (track->loop)
                    {
                        track->pos -= track->len;
                    }
                    else
                    {
                        track->pos = 0;
                        track->playing = FALSE;
                        break;
                    }
			    }
		    }
        }
    }

    // Fill the output buffer
    for (int i=0, b=0; i<len/2; i++)
    {
        Sint16 left = (mLeftSamples[i]) * (max_int_val);
        Sint16 right = (mRightSamples[i]) * (max_int_val);
        buf[b++] = left;
        buf[b++] = right;
    }
}

void sound::loadTrack(char *file, int track, float volume, bool loop/*=false*/)
{
    int index;
    SDL_AudioSpec wave;
    SDL_AudioSpec desired;
    Uint8 *data;
    Uint32 dlen;
    SDL_AudioCVT cvt;

    // Load the sound file and convert it to 16-bit stereo at 44Hz
    if ( SDL_LoadWAV(file, &wave, &data, &dlen) == NULL )
    {
        TCHAR s[512];
        wsprintf(s, L"Failed loading audio track %d\n", track);
        OutputDebugString(s);
        return;
    }

    TCHAR s[512];
    wsprintf(s, L"Loaded audio track %d\n", track);
    OutputDebugString(s);

    desired = wave;
    desired.channels = 2;
    desired.freq = 44100;
    desired.format = AUDIO_S16;
    desired.samples = SAMPLE_SIZE;
    desired.callback = sound::bufferCallback;

    SDL_BuildAudioCVT(&cvt,
        wave.format,
        wave.channels,
        wave.freq,
        AUDIO_S16,
        2,
        44100);

    cvt.buf = (Uint8*)malloc(dlen*cvt.len_mult);
    memcpy(cvt.buf, data, dlen);
    cvt.len = dlen;
    SDL_FreeWAV(data);

    if (SDL_ConvertAudio(&cvt) != 0)
    {
        TCHAR s[256];
        wsprintf(s, L"Failed to convert track %d\n", track);
        OutputDebugString(s);
    }

    if (mTracks[track].data)
    {
        delete mTracks[track].data;
        mTracks[track].data = NULL;
    }
    SDL_LockAudio();
    mTracks[track].data = cvt.buf;
    mTracks[track].len = cvt.len_cvt/2;
    mTracks[track].pos = 0;
    mTracks[track].loop = loop;
    mTracks[track].vol = volume;
    mTracks[track].playing = FALSE;
    SDL_UnlockAudio();

    if ( SDL_OpenAudio(&desired, NULL) < 0 )
    {
        fprintf(stderr, "Unable to open audio: %s\n", SDL_GetError());
    }
}

void sound::startSound()
{
    SDL_PauseAudio(0);
}

void sound::stopSound()
{
    SDL_PauseAudio(1);
}

void sound::playTrack(int track)
{
    SDL_LockAudio();
    mTracks[track].pos = 0;
    mTracks[track].playing = TRUE;
    SDL_UnlockAudio();
}

void sound::stopTrack(int track)
{
    SDL_LockAudio();
    mTracks[track].pos = 0;
    mTracks[track].playing = FALSE;
    SDL_UnlockAudio();
}

void sound::stopAllTracks()
{
    SDL_LockAudio();
    for (int i=0; i<NUM_TRACKS; i++)
    {
        mTracks[i].pos = 0;
        mTracks[i].playing = FALSE;
    }
    SDL_UnlockAudio();
}

void sound::stopAllTracksBut(int track)
{
    SDL_LockAudio();
    for (int i=0; i<NUM_TRACKS; i++)
    {
        if (i != track)
        {
            mTracks[i].pos = 0;
            mTracks[i].playing = FALSE;
        }
    }
    SDL_UnlockAudio();
}

void sound::pauseTrack(int track)
{
    SDL_LockAudio();
    mTracks[track].paused = TRUE;
    SDL_UnlockAudio();
}

void sound::unpauseTrack(int track)
{
    SDL_LockAudio();
    mTracks[track].paused = FALSE;
    SDL_UnlockAudio();
}

void sound::pauseAllTracks()
{
    SDL_LockAudio();
    for (int i=0; i<NUM_TRACKS; i++)
    {
        mTracks[i].paused = TRUE;
    }
    SDL_UnlockAudio();
}

void sound::pauseAllTracksBut(int track)
{
    SDL_LockAudio();
    for (int i=0; i<NUM_TRACKS; i++)
    {
        if (i != track)
        {
            mTracks[i].paused = TRUE;
        }
    }
    SDL_UnlockAudio();
}

void sound::unpauseAllTracks()
{
    SDL_LockAudio();
    for (int i=0; i<NUM_TRACKS; i++)
    {
        mTracks[i].paused = FALSE;
    }
    SDL_UnlockAudio();
}

void sound::setTrackSpeed(int track, double speed)
{
    SDL_LockAudio();
    mTracks[track].speed = speed;
    SDL_UnlockAudio();
}

bool sound::isTrackPlaying(int track)
{
    SDL_LockAudio();
    bool playing = mTracks[track].playing;
    SDL_UnlockAudio();
    return playing;
}

int sound::playTrackGroup(int trackFirst, int trackLast)
{
    for (int i=trackFirst; i<=trackLast; i++)
    {
        if (!isTrackPlaying(i))
        {
            playTrack(i);
            return i;
        }
    }
    // Just retrigger the first one
    playTrack(trackFirst);
    return trackFirst;
}
