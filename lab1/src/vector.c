#include "vector.h"

#include <linux/fs.h>
#include <linux/slab.h>

#define VECTOR_ELEMENT_SIZE sizeof(int)
#define INIT_VECTOR_CAP 20

vector* vector_init(void) {
  vector *v = kmalloc(sizeof(vector), GFP_USER);
  if (v == NULL) {
    printk(KERN_ERR "Failed to allocate memory for vector\n");
    return NULL;
  }

  v->buf = kmalloc(VECTOR_ELEMENT_SIZE * INIT_VECTOR_CAP, GFP_USER);
  if (v->buf == NULL) {
    printk(KERN_ERR "Failed to allocate memory for vector buffer\n");
    kfree(v);
    return NULL;
  }

  v->len = 0;
  v->cap = INIT_VECTOR_CAP;
  return v;
}

static void increase_vector_size(vector *v) {
    uint32_t cap = v->cap * 2;
    uint32_t *buf = kmalloc(cap * VECTOR_ELEMENT_SIZE, GFP_USER);
    if (buf == NULL) {
      printk(KERN_ERR "failed to allocate memory for bigger vector buffer\n");
      return;
    }
    
    memcpy(buf, v->buf, VECTOR_ELEMENT_SIZE * v->cap);
    kfree(v->buf);

    v->buf = buf;
    v->cap = cap;
}

void vector_append(vector *v, uint32_t val) {
  if (v == NULL) {
    printk(KERN_ERR "Failed to append to null vector\n");
    return;
  }

  if (v->len == v->cap) {
    increase_vector_size(v);
  }

  v->buf[v->len] = val;
  v->len++;
}

void vector_destroy(vector *v) {
  kfree(v->buf);
  kfree(v);
}