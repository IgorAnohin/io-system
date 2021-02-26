#ifndef DEVICE_H
#define DEVICE_H

#include "vector.h"

int symbol_device_init(char * device_name, int device_number, vector *data_vector_prt);
void symbol_device_destroy(int device_number);

#endif // DEVICE_H