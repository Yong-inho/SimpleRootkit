#include <linux/fs.h>

#define BUF_LEN 256

unsigned long hide_file(const char* filePath);
unsigned long unhide_file_all(void);
//void toggle_file_visible(const char* filePath, int flag);
