// This code is taken from the homework handout

#ifndef NODE_H_
#define NODE_H_

#define FILENAME_SIZE 64
#define FILETYPE_SIZE 15
#define MAX_FILE_SIZE 60

typedef struct node {
	char filename[FILENAME_SIZE];
	char filetype[FILETYPE_SIZE];
	int curr_size;
	int orig_size;
} NODE;

void updateSize(NODE** nodes, int num_of_nodes);
int createReport(char* inputDir,char* outputDir,NODE** nodes, int num_of_nodes);
void cleanup(NODE** nodes, int numNodes);
void sortNodes(NODE** nodes, int num_of_nodes);

#endif
