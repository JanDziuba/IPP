#include "check_command.h"
#include <string.h>
#include <stdbool.h>

bool check_if_command_is_a_correct_declare_command(char *command,
                                                   int size_of_command)
{
    bool is_command_correct = true;

    if(size_of_command >= 9)
    {
        char declare[9] = "DECLARE ";
        char first_8_chars[9];
        for(int i = 0; i < 8; i++)
        {
            first_8_chars[i] = command[i];
        }
        first_8_chars[8] = '\0';
        if(strcmp (first_8_chars, declare) == 0)
        {
            for(int i = 8; i < size_of_command; i++)
            {
                if((command[i] != '0') && (command[i] != '1')
                   && (command[i] != '2') && (command[i] != '3'))
                {
                    is_command_correct = false;
                }
            }
        }
        else
        {
            is_command_correct = false;
        }
    }
    else
    {
        is_command_correct = false;
    }
    return is_command_correct;
}

bool check_if_command_is_a_correct_remove_command(char *command,
                                                  int size_of_command)
{
    bool is_command_correct = true;

    if(size_of_command >= 8)
    {
        char remove[8] = "REMOVE ";
        char first_7_chars[8];
        for(int i = 0; i < 7; i++)
        {
            first_7_chars[i] = command[i];
        }
        first_7_chars[7] = '\0';
        if(strcmp (first_7_chars, remove) == 0)
        {
            for(int i = 7; i < size_of_command; i++)
            {
                if((command[i] != '0') && (command[i] != '1')
                   && (command[i] != '2') && (command[i] != '3'))
                {
                    is_command_correct = false;
                }
            }
        }
        else
        {
            is_command_correct = false;
        }
    }
    else
    {
        is_command_correct = false;
    }

    return is_command_correct;
}

bool check_if_command_is_a_correct_valid_command(char *command,
                                                 int size_of_command)
{
    bool is_command_correct = true;

    if(size_of_command >= 7)
    {
        char valid[7] = "VALID ";
        char first_6_chars[7];
        for(int i = 0; i < 6; i++)
        {
            first_6_chars[i] = command[i];
        }
        first_6_chars[6] = '\0';
        if(strcmp (first_6_chars, valid) == 0)
        {
            for(int i = 6; i < size_of_command; i++)
            {
                if((command[i] != '0') && (command[i] != '1')
                   && (command[i] != '2') && (command[i] != '3'))
                {
                    is_command_correct = false;
                }
            }
        }
        else
        {
            is_command_correct = false;
        }
    }
    else
    {
        is_command_correct = false;
    }

    return is_command_correct;
}

static bool check_if_energy_parameter_is_correct
(char *command, int size_of_command, int first_char_of_energy_parameter)
{
    bool is_command_correct = true;
    int number_of_digits_of_energy_parameter = 0;
    char max_energy[21] = "18446744073709551615";
    char energy_parameter[21];

    if(command[first_char_of_energy_parameter] == '0')
    {
        is_command_correct = false;
    }
    for(int i = first_char_of_energy_parameter; i < size_of_command; i++)
    {
        number_of_digits_of_energy_parameter++;
        if((command[i] < '0') || (command[i] > '9'))
        {
            is_command_correct = false;
        }
    }
    if(number_of_digits_of_energy_parameter >
       NUMBER_OF_DIGITS_OF_THE_MAX_ENRERGY)
    {
        is_command_correct = false;
    }
    else if(number_of_digits_of_energy_parameter ==
            NUMBER_OF_DIGITS_OF_THE_MAX_ENRERGY)
    {
        for(int i = first_char_of_energy_parameter; i < size_of_command; i++)
        {
            energy_parameter[(i - first_char_of_energy_parameter)] =
            command[i];
        }
        energy_parameter[NUMBER_OF_DIGITS_OF_THE_MAX_ENRERGY] =  '\0';
        if(strcmp (energy_parameter, max_energy) > 0)
        {
            is_command_correct = false;
        }
    }

    return is_command_correct;
}

bool check_if_command_is_a_correct_energy_assign_command(char *command,
                                                         int size_of_command)
{
    bool is_command_correct = true;
    char energy[8] = "ENERGY ";
    char first_7_chars[8];
    int character;
    int first_char_of_energy_parameter;

    if(size_of_command >= 8)
    {
        for(int i = 0; i < 7; i++)
        {
            first_7_chars[i] = command[i];
        }
        first_7_chars[7] = '\0';
        if(strcmp (first_7_chars, energy) == 0)
        {
            character = 7;
            while((character < size_of_command) && (command[character] != ' '))
            {
                if((command[character] != '0') &&
                   (command[character] != '1') &&
                   (command[character] != '2') && (command[character] != '3'))
                {
                    is_command_correct = false;
                }
                character++;
            }
            if(character >= (size_of_command - 1))
            {
                is_command_correct = false;
            }
            else if(command[7] == ' ')
            {
                is_command_correct = false;
            }
            if(is_command_correct == true)
            {
                first_char_of_energy_parameter = (character + 1);
                is_command_correct = check_if_energy_parameter_is_correct
                (command, size_of_command, first_char_of_energy_parameter);
            }
        }
        else
        {
            is_command_correct = false;
        }
    }
    else
    {
        is_command_correct = false;
    }

    return is_command_correct;
}

bool check_if_command_is_a_correct_energy_check_command(char *command,
                                                        int size_of_command)
{
    bool is_command_correct = true;

    if(size_of_command >= 8)
    {
        char energy[8] = "ENERGY ";
        char first_7_chars[8];
        for(int i = 0; i < 7; i++)
        {
            first_7_chars[i] = command[i];
        }
        first_7_chars[7] = '\0';
        if(strcmp (first_7_chars, energy) == 0)
        {
            for(int i = 7; i < size_of_command; i++)
            {
                if((command[i] != '0') && (command[i] != '1') &&
                   (command[i] != '2') && (command[i] != '3'))
                {
                    is_command_correct = false;
                }
            }
        }
        else
        {
            is_command_correct = false;
        }
    }
    else
    {
        is_command_correct = false;
    }

    return is_command_correct;
}

bool check_if_command_is_a_correct_equal_command(char *command,
                                                 int size_of_command)
{
    bool is_command_correct = true;

    if(size_of_command >= 7)
    {
        char equal[7] = "EQUAL ";
        char first_6_chars[7];
        for(int i = 0; i < 6; i++)
        {
            first_6_chars[i] = command[i];
        }
        first_6_chars[6] = '\0';
        if(strcmp (first_6_chars, equal) == 0)
        {
            int i = 6;
            while((i < size_of_command) && (command[i] != ' '))
            {
                if((command[i] != '0') && (command[i] != '1') &&
                   (command[i] != '2') && (command[i] != '3'))
                {
                    is_command_correct = false;
                }
                i++;
            }
            if(i >= (size_of_command - 1))
            {
                is_command_correct = false;
            }
            else if(command[6] == ' ')
            {
                is_command_correct = false;
            }
            else
            {
                for(int j = (i + 1); j < size_of_command; j++)
                {
                    if((command[j] != '0') && (command[j] != '1') &&
                       (command[j] != '2') && (command[j] != '3'))
                    {
                        is_command_correct = false;
                    }
                }
            }
        }
        else
        {
            is_command_correct = false;
        }
    }
    else
    {
        is_command_correct = false;
    }

    return is_command_correct;
}
