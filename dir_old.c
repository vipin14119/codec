#include "directory.h"

/* traverseDir: this function iterates through the directory entry of the given
 * input directory, holding on to the output directory for encoding/decoding and
 * holding onto the mode for choosing which operation to take.
 * TODO: implement the encoding/decoding procedure using the appropriate input
 */
int traverseDir(char* inputDir, char* outputDir, int mode)
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

		if(stat(in_name, &tmp)) {
			perror("stat failed");
			return 1;
		}

		// if the file in the input directory is another directory,
		// recursively call traverseDir on it, otherwise enocde/decode it
		if(S_ISDIR(tmp.st_mode)) {

			if(!dir_exists(out_name) && mkdir(out_name, 0755)) {
				perror("could not create directory");
				return 1;
			} else {
				traverseDir(in_name, out_name, mode);
			}

		} else if(mode == ENCODE) {

		} else { // mode == DECODE

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
