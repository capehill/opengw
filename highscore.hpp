#pragma once

class highscore
{
  public:
    highscore(void);
    ~highscore(void);

    void init();
    void run();

    bool isHighScore(int score);

    void drawEnterScore();
    void drawTable();

  private:
    void addHighScore(char* name, int score);
    void load();
    void save();

    typedef struct
    {
        char name[4];
        int score;
    } ENTRY;

    ENTRY mEntries[10];

    int mEditCurrentPos { 0 };
    int mEditCurrentLetter[3];
    int mDebounceTimer { 0 };
    int mLastLetter;
    char mEditName[4] { 0, 0, 0, 0 };
    int mScore { 0 };
};
