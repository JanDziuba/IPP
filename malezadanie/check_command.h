#ifndef CHECK_COMMAND_H
#define CHECK_COMMAND_H

#include <stdbool.h>

#define NUMBER_OF_DIGITS_OF_THE_MAX_ENRERGY 20

extern bool check_if_command_is_a_correct_declare_command(char *command,
                                                          int size_of_command);

extern bool check_if_command_is_a_correct_remove_command(char *command,
                                                         int size_of_command);

extern bool check_if_command_is_a_correct_valid_command(char *command,
                                                        int size_of_command);

extern bool check_if_command_is_a_correct_energy_assign_command
(char *command, int size_of_command);

extern bool check_if_command_is_a_correct_energy_check_command
(char *command, int size_of_command);

extern bool check_if_command_is_a_correct_equal_command
(char *command, int size_of_command);

#endif /* CHECK_COMMAND_H */
