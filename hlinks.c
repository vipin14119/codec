#include "hlinks.h"

int add_hlink(ino_t p_inode, int num) 
{
	HLINK* tmp = malloc(sizeof(HLINK));
	if(tmp == NULL) {
		perror("can't make link");
		return -1;
	}

	tmp->inode = p_inode;
	tmp->num = num;
	tmp->next = head.next;
	head.next = tmp;

	return 0;
}

int search_dec(ino_t p_inode)
{
	HLINK* left = &head;
	HLINK* right = head.next;
	while(right != NULL && right->inode != p_inode) {
		right = right->next;
		left = left->next;
	}

	if(right == NULL) return -1;

	if(--(right->num) == 0) {
		left->next = right->next;
		free(right);
	}
	return 0;
}


void remove_hlinks()
{
	while(head.next != NULL) {
		HLINK* tmp = head.next;
		head.next = tmp->next;
		free(tmp);
	}
}
