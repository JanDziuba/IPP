#ifndef STRUCTURES_H
#define STRUCTURES_H
#include <stdint.h>

typedef struct History_t
/*equal histories are stored on circular linked lists*/
{
    struct History_t *next_in_relation;
    struct History_t *previous_in_relation;
    uint64_t *pointer_to_energy_of_relation;
    int *pointer_to_size_of_relation;
} History_t;

typedef struct Tree_of_histories_t 
/*history of particle is stored in tree 
 * with ancestors being prefixes of the history*/
{
    struct Tree_of_histories_t *son_0;
    struct Tree_of_histories_t *son_1;
    struct Tree_of_histories_t *son_2;
    struct Tree_of_histories_t *son_3;
    struct History_t *history;
} Tree_of_histories_t;

#endif /* STRUCTURES_H */
