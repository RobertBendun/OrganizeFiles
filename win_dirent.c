#include "win_dirent.h"
#include <windows.h>
#include <stdlib.h>
#include <string.h>

struct DIR
{
	HANDLE find_file;
	char *dirname;
};

DIR* opendir(char const *dirname)
{
	struct DIR *p = malloc(sizeof(struct DIR));
	
	p->find_file = INVALID_HANDLE_VALUE;
	p->dirname = malloc(strlen(dirname) + 1);
	strcpy(p->dirname, dirname);
	return p;
}

int closedir(DIR *dir)
{
	struct DIR *p = dir;
	free(p->dirname);
	
	if (p->find_file != INVALID_HANDLE_VALUE)
		return FindClose(p->find_file) ? 0 : -1;

	return -1;
}

struct dirent* readdir(DIR *dir)
{
	WIN32_FIND_DATAA find_data;
	struct DIR *p = dir;
	struct dirent *dirent_;
	int valid_handle = 0;

	dirent_ = malloc(sizeof(struct dirent));

	if (p->find_file == INVALID_HANDLE_VALUE) {
		p->find_file = FindFirstFileA(p->dirname, &find_data);
		valid_handle = p->find_file != INVALID_HANDLE_VALUE;
	}
	else
		valid_handle = FindNextFileA(p->find_file, &find_data);

	if (!valid_handle)
		return NULL;

	strcpy(dirent_->d_name, find_data.cFileName);
	dirent_->d_type = find_data.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY ? DT_DIR : DT_REG;
	return dirent_;
}
