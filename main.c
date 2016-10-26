#include <stdio.h>
#include <stdlib.h>

#include "codec.h"
#include "directory.h"
#include "node.h"

void printUsage(char** argv)
{
	fprintf(stderr, "USAGE: %s -[e/d] ", argv[0]);
	fprintf(stderr, "[input directory] [output directory]\n");
	exit(1);
}

void dir_debug()
{
#ifdef DEBUG
	char buf[64];
	getcwd(buf, 64);
	printf("DEBUG: cwd = %s\n", buf);
#endif
}


int main(int argc, char** argv)
{
	int (*op)(char*, char*);
	NODE** nodes;
	int num_of_nodes = 0;
	int i;

	nodes = calloc(MAX_FILE_SIZE, sizeof(NODE*));
	if(nodes == NULL) {
		perror("Call to calloc failed constructing node array");
		exit(EXIT_FAILURE);
	}
	for(i = 0; i < MAX_FILE_SIZE; i++)
		nodes[i] = NULL;

	if(argc != 4 || argv[1][0] != '-')
		printUsage(argv);

	if(argv[1][1] == 'e') {
		op = encodeFile;
	} else if(argv[1][1] == 'd') {
		op = decodeFile;
	} else {
		printUsage(argv);
	}


	if(!dir_exists(argv[3])) {
		printf("Directory %s does not exist. Creating...\n", argv[3]);
		if(mkdir(argv[3], 0755)) {
			perror("could not create output directory");
			exit(1);
		}
	}


#ifdef DEBUG
	printf("DEBUG: initial call to traverseDir\n");
#endif

	traverseDir(argv[2], argv[3], op, nodes, &num_of_nodes);

#ifdef DEBUG
	printf("DEBUG: creating report\n");
#endif

	createReport(argv[2],argv[3],nodes, num_of_nodes);

	cleanup(nodes, num_of_nodes);

	return 0;
}
