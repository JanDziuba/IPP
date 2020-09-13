#include "execute_command.h"
#include "structures.h"
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

static void alloc_tree_of_histories_element
(Tree_of_histories_t **pointer_to_history_tree)
{
    Tree_of_histories_t *history_tree = *pointer_to_history_tree;

    history_tree = (Tree_of_histories_t*)malloc(sizeof(Tree_of_histories_t));
    if(history_tree == NULL)
    {
        exit(1);
    }
    history_tree->son_0 = NULL;
    history_tree->son_1 = NULL;
    history_tree->son_2 = NULL;
    history_tree->son_3 = NULL;
    history_tree->history = NULL;

    *pointer_to_history_tree = history_tree;
}

static void alloc_history(History_t **pointer_to_history)
{
    History_t *history = *pointer_to_history;

    history = (History_t*)malloc(sizeof(History_t));
    if(history == NULL)
    {
        exit(1);
    }
    history->next_in_relation = history;
    history->previous_in_relation = history;
    history->pointer_to_energy_of_relation = NULL;
    history->pointer_to_size_of_relation = (int*)malloc(sizeof(int));
    if(history->pointer_to_size_of_relation == NULL)
    {
        exit(1);
    }

    *pointer_to_history = history;
}

static void remove_history_from_relation(Tree_of_histories_t *history_tree)
{
    History_t *history = history_tree->history;

    if(history != NULL)
    {
        if(*(history->pointer_to_size_of_relation) == 1)
        {
            free(history->pointer_to_size_of_relation);
            free(history->pointer_to_energy_of_relation);
            free(history);
            history = NULL;
        }
        else if(*(history->pointer_to_size_of_relation) >= 2)
        {
            (*(history->pointer_to_size_of_relation))--;
            history->next_in_relation->previous_in_relation =
            history->previous_in_relation;
            history->previous_in_relation->next_in_relation =
            history->next_in_relation;
            history->next_in_relation = NULL;
            history->previous_in_relation = NULL;
            free(history);
            history = NULL;
        }
    }
    history_tree->history = history;
}

void remove_history_tree_branch(Tree_of_histories_t **pointer_to_history_tree)
{
    Tree_of_histories_t *history_tree = *pointer_to_history_tree;

    if(history_tree != NULL)
    {
        remove_history_tree_branch(&(history_tree->son_0));
        remove_history_tree_branch(&(history_tree->son_1));
        remove_history_tree_branch(&(history_tree->son_2));
        remove_history_tree_branch(&(history_tree->son_3));
        remove_history_from_relation(history_tree);
        free(history_tree);
        history_tree = NULL;
    }

    *pointer_to_history_tree = history_tree;
}

static void make_2nd_argument_cycle_part_of_1st_argument_cycle
(History_t *history_1, History_t *history_2)
{
    History_t *temp;
    int size_of_relation_2;

    if(((*(history_1->pointer_to_energy_of_relation))%2 == 1) &&
        (*(history_2->pointer_to_energy_of_relation))%2 == 1)
    {
        *(history_1->pointer_to_energy_of_relation) =
        ((*(history_1->pointer_to_energy_of_relation))/2) +
        ((*(history_2->pointer_to_energy_of_relation))/2) + 1;
    }
    else
    {
        *(history_1->pointer_to_energy_of_relation) =
        ((*(history_1->pointer_to_energy_of_relation))/2) +
        ((*(history_2->pointer_to_energy_of_relation))/2);
    }
    *(history_1->pointer_to_size_of_relation) +=
    *(history_2->pointer_to_size_of_relation);
    temp = history_2;
    size_of_relation_2 = *(history_2->pointer_to_size_of_relation);
    free(history_2->pointer_to_energy_of_relation);
    free(history_2->pointer_to_size_of_relation);
    for(int i = 0; i < size_of_relation_2; i++)
    {
        temp->pointer_to_energy_of_relation =
        history_1->pointer_to_energy_of_relation;
        temp->pointer_to_size_of_relation =
        history_1->pointer_to_size_of_relation;
        temp = temp->next_in_relation;
    }
    history_2->previous_in_relation->next_in_relation =
    history_1->next_in_relation;
    history_1->next_in_relation->previous_in_relation =
    history_2->previous_in_relation;
    history_2->previous_in_relation = history_1;
    history_1->next_in_relation = history_2;
}

static void relate_2_histories(Tree_of_histories_t *history_node_1,
                               Tree_of_histories_t *history_node_2)
{
    History_t *history_1 = history_node_1->history;
    History_t *history_2 = history_node_2->history;

    if((history_1->pointer_to_energy_of_relation == NULL) &&
        (history_2->pointer_to_energy_of_relation == NULL))
    {
        if(history_1 == history_2)
        {
            printf("OK\n");
        }
        else
        {
            fprintf(stderr, "ERROR\n");
        }
        return;
    }
    else if(history_1->pointer_to_energy_of_relation == NULL)
    {
        history_1->pointer_to_energy_of_relation =
        (uint64_t*)malloc(sizeof(uint64_t));
        if(history_1->pointer_to_energy_of_relation == NULL)
        {
            exit(1);
        }
        *(history_1->pointer_to_energy_of_relation) =
        *(history_2->pointer_to_energy_of_relation);
    }
    else if(history_2->pointer_to_energy_of_relation == NULL)
    {
        history_2->pointer_to_energy_of_relation =
        (uint64_t*)malloc(sizeof(uint64_t));
        if(history_2->pointer_to_energy_of_relation == NULL)
        {
            exit(1);
        }
        *(history_2->pointer_to_energy_of_relation) =
        *(history_1->pointer_to_energy_of_relation);
    }

    if(history_1->pointer_to_energy_of_relation !=
       history_2->pointer_to_energy_of_relation)
    {
        if(*(history_1->pointer_to_size_of_relation) >=
           *(history_2->pointer_to_size_of_relation))
        {
            make_2nd_argument_cycle_part_of_1st_argument_cycle
            (history_1, history_2);
        }
        else if(*(history_1->pointer_to_size_of_relation) <
                *(history_2->pointer_to_size_of_relation))
        {
            make_2nd_argument_cycle_part_of_1st_argument_cycle
            (history_2, history_1);
        }
    }
    printf("OK\n");

    history_node_1->history = history_1;
    history_node_2->history = history_2;
}

void execute_declare_command(char *command, int size_of_command,
                             Tree_of_histories_t **pointer_to_history_tree)
{
    Tree_of_histories_t *history_tree = *pointer_to_history_tree;
    Tree_of_histories_t *current_history;
    int first_char_of_history = 8;

    if(history_tree == NULL)
    {
        alloc_tree_of_histories_element(&history_tree);
    }
    current_history = history_tree;
    for(int i = first_char_of_history; i < size_of_command; i++)
    {
        if(command[i] == '0')
        {
            if(current_history->son_0 == NULL)
            {
                alloc_tree_of_histories_element(&(current_history->son_0));
                alloc_history(&(current_history->son_0->history));
                *(current_history->son_0->history->
                  pointer_to_size_of_relation) = 1;
            }
            current_history = current_history->son_0;

        }
        else if(command[i] == '1')
        {
            if(current_history->son_1 == NULL)
            {
                alloc_tree_of_histories_element(&(current_history->son_1));
                alloc_history(&(current_history->son_1->history));
                *(current_history->son_1->history->
                  pointer_to_size_of_relation) = 1;
            }
            current_history = current_history->son_1;

        }
        else if(command[i] == '2')
        {
            if(current_history->son_2 == NULL)
            {
                alloc_tree_of_histories_element(&(current_history->son_2));
                alloc_history(&(current_history->son_2->history));
                *(current_history->son_2->history->
                  pointer_to_size_of_relation) = 1;
            }
            current_history = current_history->son_2;
        }
        else if(command[i] == '3')
        {
            if(current_history->son_3 == NULL)
            {
                alloc_tree_of_histories_element(&(current_history->son_3));
                alloc_history(&(current_history->son_3->history));
                *(current_history->son_3->history->
                  pointer_to_size_of_relation) = 1;
            }
            current_history = current_history->son_3;
        }
    }
    printf("OK\n");

    *pointer_to_history_tree = history_tree;
}

void execute_remove_command(char *command, int size_of_command,
                            Tree_of_histories_t **pointer_to_history_tree)
{
    Tree_of_histories_t *history_tree = *pointer_to_history_tree;
    Tree_of_histories_t *current_history;
    Tree_of_histories_t *previous_history;
    int first_char_of_history = 7;
    int character = first_char_of_history;

    current_history = history_tree;
    previous_history = current_history;
    while((character < size_of_command) && (current_history != NULL))
    {
        previous_history = current_history;
        if(command[character] == '0')
        {
            current_history = current_history->son_0;
        }
        else if(command[character] == '1')
        {
            current_history = current_history->son_1;
        }
        else if(command[character] == '2')
        {
            current_history = current_history->son_2;
        }
        else if(command[character] == '3')
        {
            current_history = current_history->son_3;
        }
        character++;
    }
    remove_history_tree_branch(&current_history);
    character--;
    if(command[character] == '0')
    {
        previous_history->son_0 = NULL;
    }
    else if(command[character] == '1')
    {
        previous_history->son_1 = NULL;
    }
    else if(command[character] == '2')
    {
        previous_history->son_2 = NULL;
    }
    else if(command[character] == '3')
    {
        previous_history->son_3 = NULL;
    }
    printf("OK\n");

    *pointer_to_history_tree = history_tree;
}

void execute_valid_command(char *command, int size_of_command,
                           Tree_of_histories_t *history_tree)
{
    int first_char_of_history = 6;
    int character = first_char_of_history;
    Tree_of_histories_t *current_history;

    current_history = history_tree;
    while((character < size_of_command) && (current_history != NULL))
    {
        if(command[character] == '0')
        {
            current_history = current_history->son_0;
        }
        else if(command[character] == '1')
        {
            current_history = current_history->son_1;
        }
        else if(command[character] == '2')
        {
            current_history = current_history->son_2;
        }
        else if(command[character] == '3')
        {
            current_history = current_history->son_3;
        }
        character++;
    }
    if(current_history == NULL)
    {
        printf("NO\n");
    }
    else
    {
        printf("YES\n");
    }
}

void execute_energy_assign_command(char *command, int size_of_command,
                                   Tree_of_histories_t *history_tree)
{
    int first_char_of_history = 7;
    int character = first_char_of_history;
    Tree_of_histories_t *current_history;
    uint64_t energy = 0;

    current_history = history_tree;
    while((command[character] != ' ') && (current_history != NULL))
    {
        if(command[character] == '0')
        {
            current_history = current_history->son_0;
        }
        else if(command[character] == '1')
        {
            current_history = current_history->son_1;
        }
        else if(command[character] == '2')
        {
            current_history = current_history->son_2;
        }
        else if(command[character] == '3')
        {
            current_history = current_history->son_3;
        }
        character++;
    }
    if(current_history == NULL)
    {
        fprintf(stderr, "ERROR\n");
    }
    else
    {
        character++;
        while(character < size_of_command)
        {
            energy *= 10;
            energy += (command[character] - '0');
            character++;
        }
        if(current_history->history->pointer_to_energy_of_relation == NULL)
        {
            current_history->history->pointer_to_energy_of_relation =
            (uint64_t*)malloc(sizeof(uint64_t));
        }
        *(current_history->history->pointer_to_energy_of_relation) = energy;
        printf("OK\n");
    }
}

void execute_energy_check_command(char *command, int size_of_command,
                                  Tree_of_histories_t *history_tree)
{
    int first_char_of_history = 7;
    int character = first_char_of_history;
    Tree_of_histories_t *current_history;
    uint64_t energy;

    current_history = history_tree;
    while((character < size_of_command) && (current_history != NULL))
    {
        if(command[character] == '0')
        {
            current_history = current_history->son_0;
        }
        else if(command[character] == '1')
        {
            current_history = current_history->son_1;
        }
        else if(command[character] == '2')
        {
            current_history = current_history->son_2;
        }
        else if(command[character] == '3')
        {
            current_history = current_history->son_3;
        }
        character++;
    }
    if(current_history == NULL)
    {
        fprintf(stderr, "ERROR\n");
    }
    else
    {
        if(current_history->history->pointer_to_energy_of_relation == NULL)
        {
            fprintf(stderr, "ERROR\n");
        }
        else
        {
            energy = *(current_history->history->
                       pointer_to_energy_of_relation);
            printf("%" PRIu64 "\n", energy);
        }
    }
}

void execute_equal_command(char *command, int size_of_command,
                           Tree_of_histories_t *history_tree)
{
    int first_char_of_history = 6;
    int character = first_char_of_history;
    Tree_of_histories_t *current_history_1;
    Tree_of_histories_t *current_history_2;

    current_history_1 = history_tree;
    while((command[character] != ' ') && (current_history_1 != NULL))
    {
        if(command[character] == '0')
        {
            current_history_1 = current_history_1->son_0;
        }
        else if(command[character] == '1')
        {
            current_history_1 = current_history_1->son_1;
        }
        else if(command[character] == '2')
        {
            current_history_1 = current_history_1->son_2;
        }
        else if(command[character] == '3')
        {
            current_history_1 = current_history_1->son_3;
        }
        character++;
    }
    if(current_history_1 == NULL)
    {
        fprintf(stderr, "ERROR\n");
    }
    else
    {
        character++;
        current_history_2 = history_tree;
        while((character < size_of_command) && (current_history_2 != NULL))
        {
            if(command[character] == '0')
            {
                current_history_2 = current_history_2->son_0;
            }
            else if(command[character] == '1')
            {
                current_history_2 = current_history_2->son_1;
            }
            else if(command[character] == '2')
            {
                current_history_2 = current_history_2->son_2;
            }
            else if(command[character] == '3')
            {
                current_history_2 = current_history_2->son_3;
            }
            character++;
        }
        if(current_history_2 == NULL)
        {
            fprintf(stderr, "ERROR\n");
        }
        else
        {
            relate_2_histories(current_history_1, current_history_2);
        }

    }
}
