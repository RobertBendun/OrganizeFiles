#ifndef _WIN_DIRENT_H
#define _WIN_DIRENT_H

#define NAME_MAX 260

typedef void DIR;

enum
{
  DT_DIR,
  DT_REG
};

struct dirent
{
	char d_name[NAME_MAX];
	char d_type;
};

DIR* opendir(char const *dirname);
int closedir(DIR *dir);
struct dirent* readdir(DIR *dir);

#endif