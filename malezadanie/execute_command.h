#ifndef EXECUTE_COMMAND_H
#define EXECUTE_COMMAND_H

#include "structures.h"

extern void remove_history_tree_branch
(Tree_of_histories_t **pointer_to_history_tree);

extern void execute_declare_command
(char *command, int size_of_command,
 Tree_of_histories_t **pointer_to_history_tree);

extern void execute_remove_command
(char *command, int size_of_command,
Tree_of_histories_t **pointer_to_history_tree);

extern void execute_valid_command
(char *command, int size_of_command, Tree_of_histories_t *history_tree);

extern void execute_energy_assign_command
(char *command, int size_of_command, Tree_of_histories_t *history_tree);

extern void execute_energy_check_command
(char *command, int size_of_command, Tree_of_histories_t *history_tree);

extern void execute_equal_command
(char *command, int size_of_command, Tree_of_histories_t *history_tree);

#endif /* EXECUTE_COMMAND_H */

