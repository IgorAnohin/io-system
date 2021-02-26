#include "device.h"

#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#define NON_FIRST_BYTE_PREFIX 0x80

static dev_t num;
static struct class *dev_class;
static struct cdev device;

static vector *data_vector;

static ssize_t dev_read(struct file *f, char __user *buf, size_t len, loff_t *off) {
  if (data_vector == NULL) {
     printk(KERN_ERR "%s: failed to read data_vector from null pointer\n", THIS_MODULE->name);
     return -1;
  }

  uint32_t i;
  for (i = 0; i < data_vector->len; i++) {
    printk(KERN_DEBUG "%s: %d\n", THIS_MODULE->name, data_vector->buf[i]);
  }

  return 0;
}

uint32_t calculate_utf8_length(char *str, size_t str_length) {
  if (str == NULL) {
    printk(KERN_ERR "%s: failed to calculate length of null string\n", THIS_MODULE->name);
    return 0;
  }

  uint32_t len = 0;
  size_t cur_byte_index;
  for (cur_byte_index = 0; cur_byte_index < str_length; cur_byte_index++) {
    char byte = str[cur_byte_index];
    char highest_2_bits_prefix = byte & 0xc0;

    /* UTF8 symbol has 1-4 bytes:
       1 byte  word: 0xxxxxxx
       2 bytes word: 110xxxxx 10xxxxxx
       3 bytes word: 1110xxxx 10xxxxxx 10xxxxxx
       4 bytes word: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    */
    if (highest_2_bits_prefix != NON_FIRST_BYTE_PREFIX) {
      len++;
    }
  }
  return len;
}

static ssize_t dev_write(struct file *f, const char __user *buf, size_t len, loff_t *off) {
  char str[len];
  copy_from_user(str, buf, len);
  uint32_t symbols_count = calculate_utf8_length(str, len);
  vector_append(data_vector, symbols_count);
  return len;
}

static struct file_operations dev_fops = {
	.owner = THIS_MODULE,
	.read = dev_read,
	.write = dev_write
};

int symbol_device_init(char * device_name, int device_number, vector *data_vector_prt) {
  data_vector = data_vector_prt;

  unsigned int minor = 0;
  if (alloc_chrdev_region(&num, minor, device_number, device_name) < 0) {
    printk(KERN_ERR "%s: failed to allocate a char major number: device=%s\n", THIS_MODULE->name, device_name);
    return -1;
  }
  if ((dev_class = class_create(THIS_MODULE, "chardrv")) == NULL) {		
    printk(KERN_ERR "%s: failed to create device class: device=%s\n", THIS_MODULE->name, device_name);
    unregister_chrdev_region(num, device_number);
    return -1;
  }
  if (device_create(dev_class, NULL, num, NULL, device_name) == NULL) {
    printk(KERN_ERR "%s: failed to create char device: device=%s\n", THIS_MODULE->name, device_name);
    class_destroy(dev_class);
    unregister_chrdev_region(num, device_number);
    return -1;
  }
  cdev_init(&device, &dev_fops);
  if (cdev_add(&device, num, device_number) < 0) {
    printk(KERN_ERR "%s: failed to add char device to the system: device=%s\n", THIS_MODULE->name, device_name);
    device_destroy(dev_class, num);
    class_destroy(dev_class);
    unregister_chrdev_region(num, device_name);
    return -1;
  }
  return 0;
}

void symbol_device_destroy(int device_number) {
  cdev_del(&device);
  device_destroy(dev_class, num);
  class_destroy(dev_class);
  unregister_chrdev_region(num, device_number);
}