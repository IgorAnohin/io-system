#include "proc.h"

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/uaccess.h>

static struct proc_dir_entry* entry;
static vector *data_for_reading_vector;

ssize_t proc_read(struct file *f, char __user *buf, size_t len, loff_t *off) {
  static int last_read_idx = 0;
  printk(KERN_INFO "%s: current position offset %lld\n", THIS_MODULE->name, *off);

  if (data_for_reading_vector == NULL) {
    printk(KERN_ERR "%s: failed to read values from null results\n", THIS_MODULE->name);
    return -1;
  }

  if (last_read_idx == data_for_reading_vector->len) {
    last_read_idx = 0;
    return 0;
  }

  char res[12]; // max 32-bit number (10 chars) + null-terminator + newline
  int res_len = sprintf(res, "%d\n", data_for_reading_vector->buf[last_read_idx]);
  
  if (copy_to_user(buf, res, res_len) != 0)
  {
    return -EFAULT;
  }
  last_read_idx++;
  return res_len;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = proc_read
};

int proc_init(char * proc_name, vector * data_vector)
{
    data_for_reading_vector = data_vector;
	entry = proc_create(proc_name, 0444, NULL, &fops);
    if (entry == NULL) {
        printk(KERN_ERR "Unable to create %s proc entry\n", proc_name);
    }
	printk(KERN_INFO "%s: proc file is created\n", THIS_MODULE->name);
	return 0;
}

void proc_exit(void)
{
	proc_remove(entry);
	printk(KERN_INFO "%s: proc file is deleted\n", THIS_MODULE->name);
}