#ifndef LEVEL_DATA_H
#define LEVEL_DATA_H

typedef struct
{
    int width;
    int height;
    int mines;
    int music;
    int back;
    int id;
} DifficultyData;

typedef struct
{
    const char *name;
    DifficultyData difficulties[4]; // Easy, Normal, Hard, Expert
} LevelData;

typedef struct
{
    const char *name;
    LevelData *levels;
    int level_count;
} EpisodeData;

extern LevelData lvl_ep1[];
extern LevelData lvl_ep2[];
extern LevelData lvl_ep3[];

extern EpisodeData episodes[];
extern const int episode_count;

#endif