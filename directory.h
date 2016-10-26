#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "node.h"

#ifndef __DIRECTORY_H__
#define __DIRECTORY_H__

#define ENCODE 1
#define DECODE 0

int traverseDir(char* inputDir, char* outputDir, int (*operation)(char*, char*), NODE** nodes, int *numNodes);
char* makeName(char* inputDir, struct dirent * entry);
int decodeFile(char* in_name, char* out_name);
int encodeFile(char* in_name, char* out_name);
int dir_exists(char* dir);
int check_hlinks(struct stat* ptmp);

#endif
