#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "win_dirent.h"

char* downloads_path() /* windows only */
{
	static char path[1024];
	sprintf(path, "%s%s\\Downloads\\", getenv("HOMEDRIVE"), getenv("HOMEPATH"));
	return path;
}

void print_error()
{
    LPSTR messageBuffer = NULL;
    
    FormatMessageA(
    	FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    fprintf(stderr, "%s\n", messageBuffer);
    LocalFree(messageBuffer);
}

void movefile(char *src, char *dest, char *fname, char *p)
{
	strcat(src, fname);
	sprintf(dest + strlen(dest), "%s%s", p, fname);
	if (MoveFileA(src, dest))
		printf("Moved %s\n", fname);
	else {
		printf("%s\n%s\n\n", src, dest);
		print_error();
	}
}

int match_end(char *str, char *match)
{
	int i, j, s;
	s = strlen(match) - 1;

	for (i = strlen(str) - 1; i >= 0; --i) {
		for (j = 0; j <= s && i - j >= 0; ++j)
			if (tolower(str[i - j]) != tolower(match[s - j]))
				break;
		
		if (j >= s)
			return 1;
	}

	return 0;
}

#define move_if(condition, target) \
	if ((condition)) { \
			movefile(src_path, dest_path, file->d_name, target); \
			src_path[src_len] = dest_path[dest_len] = '\0'; \
		}

int main()
{
	char *p;
	DIR *dir;
	struct dirent *file;
	int dummy;
	char src_path[NAME_MAX], dest_path[NAME_MAX];
	int src_len, dest_len;

	if (!(p = downloads_path()))
		return 1;

	src_len = dest_len = strlen(p);
	strcpy(dest_path, p);

	sprintf(src_path, "%s*", p);
	dir = opendir(src_path);
	src_path[src_len] = '\0';

	while ((file = readdir(dir)))
		if (file->d_type == DT_DIR)
			continue;
		else move_if(match_end(file->d_name, ".part"),                 "!\\garbage\\")
		else move_if(sscanf(file->d_name, "[s%de%d]", &dummy, &dummy), "!\\video\\series\\")
		else move_if(match_end(file->d_name, ".mp4"),                  "!\\video\\")
		else move_if(match_end(file->d_name, ".pdf"),                  "!\\documents\\")
		else move_if(match_end(file->d_name, ".exe")
			|| match_end(file->d_name, ".msi"),                          "!\\applications\\")
		else move_if(match_end(file->d_name, ".zip")
			|| match_end(file->d_name, ".rar"),                          "!\\archives\\")
		else move_if(match_end(file->d_name, ".jpg")
			|| match_end(file->d_name, ".png"),                          "!\\photos\\")
		else move_if(match_end(file->d_name, ".cc")
			|| match_end(file->d_name, ".c"),                            "!\\programming\\")

	closedir(dir);
	return 0;
}
