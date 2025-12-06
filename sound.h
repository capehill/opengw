#pragma once

#include "SDL.h"


class sound
{
public:
    sound();
    ~sound();

    void loadTrack(const char *file, int track, float volume, bool loop=false);
    void playTrack(int track);
    void stopTrack(int track);
    void stopAllTracks();
    void stopAllTracksBut(int track);
    int playTrackGroup(int trackFirst, int trackLast);
    void pauseTrack(int track);
    void unpauseTrack(int track);
    void pauseAllTracks();
    void unpauseAllTracks();
    void pauseAllTracksBut(int track);
    bool isTrackPlaying(int track);

    void startSound();
    void stopSound();

    void setTrackSpeed(int track, double speed);

private:

    static void bufferCallback(void *unused, Uint8 *stream, int len);

    typedef struct {
        Uint8 *data;
        bool loop;
        bool playing;
        bool paused;
        float vol;
        double speed;
        double pos;
        Uint32 len;
    } TRACK;

    static TRACK* mTracks;

    static float* mLeftSamples;
    static float* mRightSamples;
};
