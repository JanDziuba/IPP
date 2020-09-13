#include <stdbool.h>
#include <stdlib.h>
#include "structures.h"
#include "read.h"
#include "execute_command.h"

int main()
{
    Tree_of_histories_t *history_tree = NULL;
    bool has_input_ended = false;
    while(has_input_ended == false)
    {
        has_input_ended = read_and_execute_the_command(&history_tree);
    }
    remove_history_tree_branch(&history_tree);
    return 0;
}
