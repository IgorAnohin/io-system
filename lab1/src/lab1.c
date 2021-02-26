#include <linux/module.h>

#include "vector.h"
#include "proc.h"
#include "device.h"

#define MODULE_NAME "lab1"
#define PROC_NAME "var1"
#define DEVICE_NAME "var1"
#define DEVICE_NUMBER 1

MODULE_LICENSE("GPL"); // to Fix GPL-incompatible module lab1.ko uses GPL-only symbol 'device_destroy'
MODULE_AUTHOR("Anokhin Igor, Gostev Mikhail");
MODULE_DESCRIPTION("Lab1, Var 1.");
MODULE_VERSION("1");

static vector *data_vector;

static int __init mod_init(void) {
  data_vector = vector_init();
  if (data_vector == NULL) {
    printk(KERN_ERR "Unable to create vector\n");
    return -1;
  }

  if (proc_init(MODULE_NAME, data_vector) < 0) {
    printk(KERN_ERR "Unable to init proc\n");
    vector_destroy(data_vector);
    return -1;
  }

  if (symbol_device_init(DEVICE_NAME, DEVICE_NUMBER, data_vector) < 0) {
    printk(KERN_ERR "Unable to init symbol device\n");
    proc_exit();
    vector_destroy(data_vector);
    return -1;
  }

  printk(KERN_INFO "%s: successfully started: device=%s, proc=%s\n", MODULE_NAME, DEVICE_NAME, PROC_NAME);
  return 0;
}

static void __exit mod_exit(void) {
  symbol_device_destroy(DEVICE_NUMBER);
  proc_exit();
  vector_destroy(data_vector);
  printk(KERN_INFO "%s: successfully release all resources\n", MODULE_NAME);
}

module_init(mod_init);
module_exit(mod_exit);
