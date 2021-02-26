#ifndef PROC_H
#define PROC_H

#include "vector.h"

int proc_init(char * proc_name, vector * data_vector);
void proc_exit(void);

#endif // PROC_H
