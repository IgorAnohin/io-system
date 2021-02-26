#ifndef VECTOR_H
#define VECTOR_H

#include <linux/types.h>

typedef struct {
    uint32_t *buf;
    uint32_t len;
    uint32_t cap;
} vector;

vector* vector_init(void);
void vector_append(vector *v, uint32_t val);
void vector_destroy(vector *v);

#endif // VECTOR_H