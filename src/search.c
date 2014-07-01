#include <stdio.h>
#include <sys/stat.h>

#include "ofn.h"
#include "database.h"

int main(int argc, char** argv)
{
	int result;
	const char* filename = argv[1];

	if (argc == 1)
	{
		fprintf(stderr, "usage: %s <filename>\n", argv[0]);

		return 1;
	}

	result = ofn_init();

	if (result != 0)
	{
		printf("ofn_init failed\n");
	}

	// Verify that the file exists.
	struct stat buffer;

	if (stat(filename, &buffer) != 0)
	{
		fprintf(stderr, "file %s doesn't exist.\n", filename);

		return 1;
	}

	result = ofn_search(filename);

	ofn_close();

	return result;
}
