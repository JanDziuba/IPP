#ifndef READ_H
#define READ_H

#define INITIAL_SIZE 4
#define MULTIPLIER 2

#include "structures.h"
#include <stdbool.h>

extern bool read_and_execute_the_command
(Tree_of_histories_t **pointer_to_history_tree);

#endif /* READ_H */
