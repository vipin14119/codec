#include "directory.h"
#include "node.h"
#include "codec.h"
#include "hlinks.h"



/* traverseDir: this function iterates through the directory entry of the given
 * input directory, holding on to the output directory for encoding/decoding and
 * holding onto the mode for choosing which operation to take.
 */
int traverseDir(char* inputDir, char* outputDir, int (*operation)(char*, char*), NODE** nodes, int *numNodes)
{
	DIR * dirp = opendir(inputDir);
	struct dirent * entry;
	struct stat tmp;
	char * in_name;
	char * out_name;

	if(dirp == NULL) {
		perror("could not open directory");
		return 1;
	}

	entry = readdir(dirp);	// .
	entry = readdir(dirp);	// ..
	entry = readdir(dirp);

	while(entry != NULL) {
		in_name = makeName(inputDir, entry);
		if(in_name == NULL) {
			perror("could not make input name");
			return 1;
		}

		out_name = makeName(outputDir, entry);
		if(out_name == NULL) {
			perror("could not make output name");
			return 1;
		}

#ifdef DEBUG
		printf("DEBUG: in_name = %50s\t", in_name);
		printf("DEBUG: out_name = %s\n", out_name);
#endif

		NODE *temp= malloc(sizeof(NODE));
		if(temp == NULL) {
			perror("Call to malloc failed");
			return 1;
		}

		if(lstat(in_name, &tmp)) {
			perror("stat failed");
			return 1;
		}

		if(S_ISLNK(tmp.st_mode)) {
			strncpy(temp->filename, entry->d_name, FILENAME_SIZE);
			strncpy(temp->filetype, "sym link", FILETYPE_SIZE);
			temp->orig_size = 0;
			temp->curr_size = 0;
			nodes[(*numNodes)++] = temp;

		} else if(S_ISDIR(tmp.st_mode)) {

			strncpy(temp->filename, entry->d_name, FILENAME_SIZE);
			strncpy(temp->filetype, "directory", FILETYPE_SIZE);
			temp->orig_size = 0;
			temp->curr_size =0;
			nodes[(*numNodes)++] = temp;

			if(!dir_exists(out_name) && mkdir(out_name, 0755)) {
				perror("could not create directory");
				return 1;
			} else {
				traverseDir(in_name, out_name, operation, nodes, numNodes);
			}

		} else if(S_ISREG(tmp.st_mode)) {

			// if it's a hard link, no encoding
			if(check_hlinks(&tmp)) {
				strncpy(temp->filename, entry->d_name, FILENAME_SIZE);
				strncpy(temp->filetype, "hard link", FILETYPE_SIZE);
				temp->orig_size = 0;
				temp->curr_size = 0;

			} else {
				// regular file: actually do encoding/decoding
				strncpy(temp->filename, entry->d_name, FILENAME_SIZE);
				strncpy(temp->filetype, "regular file", FILETYPE_SIZE);
				temp->orig_size = tmp.st_size;

				operation(in_name, out_name);

				//update current size
				if(stat(out_name, &tmp)) {
					perror("stat failed");
					return 1;
				}
				temp->curr_size = tmp.st_size;
			}
			nodes[(*numNodes)++] = temp;

		} 

		free(in_name);
		free(out_name);

		entry = readdir(dirp);
	}

	if(closedir(dirp)) {
		perror("could not close directory");
		return 1;
	}

	return 0;
}

/* makeName: creates a string with the name of the file of interest
 * Does not print message if calloc fails, so should be checked in the calling
 * function.
 * makes a call to calloc, so should be freed.
 */
char* makeName(char* inputDir, struct dirent * entry)
{
	int input_length = strlen(inputDir);
	char* name = calloc(input_length + entry->d_reclen + 2, sizeof(char));
	if(name == NULL) {
		return NULL;
	}
	strncpy(name, inputDir, input_length);
	if(name[input_length - 1] != '/') strncat(name, "/", 2);
	strncat(name, entry->d_name, entry->d_reclen);
	return name;
}


/* dir_exists: does exactly what you think it does. Used to see if the
 * output directory exists, and if it doesn't, the main function makes it
 */
int dir_exists(char* dir)
{
	DIR* tmp = opendir(dir);
	if(tmp == NULL) return 0;
	closedir(tmp);
	return 1;
}


/* encodeFile: given the input and output file names, encodes the file using
 * the appropriate API.
 */
int encodeFile(char* in_name, char* out_name)
{
	uint8_t inp[3];
	uint8_t out[4];
	int notEmpty=0;
	int i;
	FILE * fpFrom= fopen(in_name,"r");
	if(fpFrom == NULL) {
		perror("could not open file for reading");
		return 1;
	}

	FILE * fpTo= fopen(out_name, "w");
	if(fpTo == NULL) {
		perror("could not open file for reading");
		return 1;
	}

	while((i = fread(inp, sizeof(uint8_t), 3, fpFrom)) != 0)
	{
		if(i < 3) {
			int j;
			for(j = i;j < 3; j++) inp[j] = 0;
		}
		notEmpty =1;
		fwrite(out, sizeof(uint8_t), encode_block(inp, out, i), fpTo);
	}
	if(notEmpty)//write a new line if not empty
	{
		fputc((unsigned char) 0x0A,fpTo);
	}
	fclose(fpFrom);
	fclose(fpTo);
	return 0;
}


/* decodeFile: give the input and output file names, decodes the file using
 * the appropriate API.
 */
int decodeFile(char* in_name, char* out_name)
{
	uint8_t inp[4];
	uint8_t out[3];
	int i, j;
	FILE * fpFrom= fopen(in_name,"r");
	if(fpFrom == NULL) {
		perror("could not open file for reading");
		return 1;
	}

	FILE * fpTo= fopen(out_name, "w");
	if(fpTo == NULL) {
		perror("could not open file for reading");
		return 1;
	}

	while((i = fread(inp, sizeof(uint8_t), 4, fpFrom)) != 0)
	{
		int valid = 1;
		for(j = 0; j < i; j++) {
			if(!is_valid_char(inp[j]))
				valid = 0;
		}
		if(valid) {
			fwrite(out, sizeof(uint8_t), decode_block(inp, out), fpTo);
		}
	}
	fclose(fpFrom);
	fclose(fpTo);
	return 0;
}


int check_hlinks(struct stat* ptmp)
{
	if(ptmp->st_nlink > 1 && search_dec(ptmp->st_ino) == -1) {
		add_hlink(ptmp->st_ino, ptmp->st_nlink);
		return 1;
	} else
		return 0;
}
