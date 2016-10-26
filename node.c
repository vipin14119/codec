#include "directory.h"
#include "node.h"
#include "codec.h"
#include "hlinks.h"


/* cleanup: deallocates the nodes and the array of nodes.
 */
void cleanup(NODE** nodes, int numNodes)
{
	int i;
	for(i = 0; i < numNodes; i++) {
		if(nodes[i] != NULL) {
#ifdef DEBUG
			printf("Freeing node at index %i\n", i);
#endif
			free(nodes[i]);
		}
	}

#ifdef DEBUG
	printf("DEBUG: Freeing array of nodes\n");
#endif

	free(nodes);
	remove_hlinks();
}


/* createReport: sorts the array of nodes alphabetically, and then
 * outputs information about the nodes to the report file.
 * Returns 0 if report made successfully, -1 if error.
 */
int createReport(char* inputDir,char* outputDir,NODE** nodes, int num_of_nodes)
{
	sortNodes(nodes, num_of_nodes);

	if(chdir(outputDir)) {
		perror("could not change directory");
		return -1;
	}

	char* report = calloc(strlen(inputDir) + 12, sizeof(char));
	if(report == NULL) {
		perror("could not create report string");
		return -1;
	}
	strcpy(report, inputDir);
	strncat(report, "_report.txt", 12);

	FILE *fp = fopen(report, "w");
	if (fp == NULL)
	{
    	fprintf(stderr, "Error opening file!\n");
		return -1;
	}
	if(num_of_nodes ==0)
	{
		fprintf(stderr, "%s\n", "Report empty");
	}
	else
	{
		int i;
		for(i = 0; i<num_of_nodes; i++)
		{
			fprintf(fp, "%s, %s, %d, %d\n",
					nodes[i]->filename,
					nodes[i]->filetype,
					nodes[i]->orig_size,
					nodes[i]->curr_size);
		}
	}
	fclose(fp);

	free(report);

	return 0;
}


/* compareNodes: looks at the filenames of two nodes.
 * Returns 0 if node1's filename comes before node2's filename.
 * Returns 1 if node2's filename comes before node1's filename.
 * Analogous to the expression "node1 > node2"
 */
int compareNodes(NODE* node1, NODE* node2)
{
	int i = 0;
	while(node1->filename[i] != '\0' && node2->filename[i] != '\0') {
		if(node1->filename[i] < node2->filename[i])
			return 0;
		else if(node1->filename[i] > node2->filename[i])
			return 1;
		i += 1;
	}
	if(node1->filename[i] != '\0' && node2->filename[i] == '\0')
		return 1;
	else return 0;
}


/* sortNodes: I have no idea what this does....
 * Just does insertionsort on it.
 * TODO: quicksort
 */
void sortNodes(NODE** nodes, int num_of_nodes)
{
	int i, j;
	for(j = 1; j < num_of_nodes; j++) {
		NODE* key = nodes[j];
		i = j - 1;
		while(i >= 0 && compareNodes(nodes[i], key)) {
			nodes[i + 1] = nodes[i];
			i = i - 1;
		}
		nodes[i + 1] = key;
	}
}

