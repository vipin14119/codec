#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef HLINKS_H_
#define HLINKS_H_

typedef struct hlink {
	ino_t inode;
	int num;
	struct hlink* next;
} HLINK;

HLINK head;

void remove_hlinks();
int search_dec(ino_t p_inode);
int add_hlink(ino_t p_inode, int num);

#endif
