#include "read.h"
#include <stdlib.h>
#include <stdio.h>
#include "check_command.h"
#include "execute_command.h"
#include "structures.h"

static void read_the_command(char **pointer_to_command,
                             int *pointer_to_size_of_command,
                             char *pointer_to_next_character)
{
    char next_character = *pointer_to_next_character;
    char *command = *pointer_to_command;
    int max_size_of_command = INITIAL_SIZE;
    int size_of_command = *pointer_to_size_of_command;

    command = (char*)malloc(max_size_of_command  *sizeof(char));
    if(command == NULL)
    {
        exit(1);
    }
    next_character = getchar();
    while((next_character != EOF) && (next_character != '\n'))
    {
        size_of_command++;
        if(size_of_command >= max_size_of_command)
        {
            max_size_of_command = max_size_of_command * MULTIPLIER;
            command = (char*)realloc(command,
                                     max_size_of_command * sizeof(char));
            if(command == NULL)
            {
                exit(1);
            }
        }
        command[size_of_command - 1] = next_character;
        next_character = getchar();
    }
    *pointer_to_command = command;
    *pointer_to_size_of_command = size_of_command;
    *pointer_to_next_character = next_character;
}

bool read_and_execute_the_command
(Tree_of_histories_t **pointer_to_history_tree)
{
    Tree_of_histories_t *history_tree = *pointer_to_history_tree;
    char next_character;
    bool has_input_ended = false;
    bool is_command_correct = false;
    char *command = NULL;
    int size_of_command = 0;

    read_the_command(&command, &size_of_command, &next_character);
    if((next_character == EOF) && (size_of_command >= 1))
    {
        has_input_ended = true;
        is_command_correct = false;
    }
    else if((next_character == EOF) && (size_of_command < 1))
    {
        has_input_ended = true;
        is_command_correct = true;
    }
    else if(next_character != EOF)
    {
        if((size_of_command < 1) || (command[0] == '#'))
        {
            is_command_correct = true;
        }
        else
        {
            if(true == check_if_command_is_a_correct_declare_command
               (command, size_of_command))
            {
                is_command_correct = true;
                execute_declare_command(command, size_of_command,
                                        &history_tree);
            }
            else if(true == check_if_command_is_a_correct_remove_command
                    (command, size_of_command))
            {
                is_command_correct = true;
                execute_remove_command(command, size_of_command,
                                       &history_tree);
            }
            else if(true == check_if_command_is_a_correct_valid_command
                    (command, size_of_command))
            {
                is_command_correct = true;
                execute_valid_command(command, size_of_command, history_tree);
            }
            else if(true == check_if_command_is_a_correct_energy_assign_command
                    (command, size_of_command))
            {
                is_command_correct = true;
                execute_energy_assign_command(command, size_of_command,
                                              history_tree);
            }
            else if(true == check_if_command_is_a_correct_energy_check_command
                    (command, size_of_command))
            {
                is_command_correct = true;
                execute_energy_check_command(command, size_of_command,
                                             history_tree);
            }
            else if(true == check_if_command_is_a_correct_equal_command
                    (command, size_of_command))
            {
                is_command_correct = true;
                execute_equal_command(command, size_of_command, history_tree);
            }
        }
    }
    if(is_command_correct == false)
    {
        fprintf(stderr, "ERROR\n");
    }

    *pointer_to_history_tree = history_tree;
    free(command);
    return has_input_ended;
}
