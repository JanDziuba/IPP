/** @file
 * Biblioteka operacji na napisach.
 *
 * @author Jan Dziuba <	jd406140@students.mimuw.edu.pl>
 */

#include "string.h"

#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>

/** @brief Zwraca liczbę charów w reprezentacji zmiennej typu unsigned jako napisu typu c.
 * @param[in] number
 * @return liczba charów
 */
int getNumberOfCharsInStringRepresentationOfUnsigned(unsigned number) {

    int numberOfChars = 0;

    if (number == 0) {
        return 2;
    }

    while (number != 0) {
        numberOfChars++;
        number = number / 10;
    }

    numberOfChars++;

    return numberOfChars;
}

/** @brief Zwraca liczbę charów w reprezentacji zmiennej typu int jako napisu typu c.
 * @param[in] number
 * @return liczba charów
 */
int getNumberOfCharsInStringRepresentationOfInt(int number) {

    int numberOfChars = 0;

    if (number == 0) {
        return 2;
    }

    ///char na znak minus
    if (number < 0) {
        numberOfChars++;
    }

    while (number != 0) {
        numberOfChars++;
        number = number / 10;
    }

    numberOfChars++;

    return numberOfChars;
}

/** @brief Zwraca reprezentację zmiennej typu unsigned jako napisu typu c.
 * @param[in] number
 * @return Utworzony napis lub NULL gdy nie udało się zaalokować
 * pamięci.
 */
const char* getStringRepresentationOfUnsigned(unsigned number) {

    int numberOfChars = getNumberOfCharsInStringRepresentationOfUnsigned(number);
    int index = (numberOfChars - 1);
    char* string = NULL;

    string = (char*)malloc(sizeof(char) * numberOfChars);
    if (string == NULL) {
        return NULL;
    }

    if (number == 0) {
        string[0] = '0';
        string[1] = '\0';
        return string;
    }

    string[index] = '\0';
    index--;

    while (number != 0) {
        string[index] = (number % 10 + '0');
        number = number / 10;
        index--;
    }

    return string;
}

/** @brief Zwraca reprezentację zmiennej typu int jako napisu typu c.
 * @param[in] number
 * @return Utworzony napis lub NULL gdy nie udało się zaalokować
 * pamięci.
 */
const char* getStringRepresentationOfInt(int number) {

    int numberOfChars = getNumberOfCharsInStringRepresentationOfInt(number);
    int index = (numberOfChars - 1);
    char* string = NULL;

    string = (char*)malloc(sizeof(char) * numberOfChars);
    if (string == NULL) {
        return NULL;
    }


    if (number == 0) {
        string[0] = '0';
        string[1] = '\0';
        return string;
    }

    string[index] = '\0';
    index--;

    if (number >= 0) {
        while (number != 0) {
            string[index] = (number % 10 + '0');
            number = number / 10;
            index--;
        }
        return string;
    }

    while (number != 0) {
        string[index] = (((number % 10) * (-1)) + '0');
        number = number / 10;
        index--;
    }

    string[index] = '-';

    return string;
}

/** @brief Dodaje chary z napisu subString na koniec napisu w trakcie tworzenia stringInConstruction.
 * Koniec nie jest końcem zaalokowanej pamięci, lecz pierwszym niezapisany jeszcze charem tego napisu.
 * Aby funkcja dzialała poprawnie w stringInConstruction musi być przynajmniej tyle niezapisanych charów
 * co liczba charów w subString.
 * @param[in] subString - dodawany napis
 * @param[in,out] stringInConstruction - tworzony napis
 * @param[in,out] pointerToIndexOfString - Wskaźnik na index
 * @param[in] stringInConstructionLength - ilość charów zaalokowanych w stringInConstruction
 * @return Wartość @p true, jeśli udało się dodać napis.
 * Wartość @p false, jeśli argumenty były nieprawidłowe.
 */
bool addSubstringToStringStartingOnIndexedPosition(const char *subString, char* stringInConstruction,
                                                   int *pointerToIndexOfString, int stringInConstructionLength) {

    int indexOfString = *pointerToIndexOfString;
    int indexOfSubstring = 0;

    if ((subString == NULL) || (stringInConstruction == NULL) || (indexOfString < 0) ||
        (stringInConstructionLength <= 0)) {
        return false;
    }

    while (subString[indexOfSubstring] != '\0') {
        if (indexOfString >= stringInConstructionLength) {
            return false;
        }
        stringInConstruction[indexOfString] = subString[indexOfSubstring];
        indexOfString++;
        indexOfSubstring++;
    }

    *pointerToIndexOfString = indexOfString;
    return true;
}


/** @brief Sprawdza czy napis przedstawia liczbę typu unsigned.
 * Akceptuje zera wiodące.
 * @param string
 * @return Wartość @p true, jeśli napis przedstawia liczbę typu unsigned.
 * Wartość @p false, jeśli napis nie przedstawia liczby typu unsigned.
 */
bool checkIfStringCanBeRepresentedAsUnsigned(const char* string) {

    int numberOfCharsInStringRepresentationOfUINT_MAX;
    const char* stringRepresentationOfUINT_MAX = NULL;
    int stringIndex = 0;
    int numberOfCharsInString = getNumberOfCharsInString(string);

    if (string == NULL) {
        return false;
    }

    while (string[stringIndex] != '\0') {
        if(isdigit((int)string[stringIndex]) == 0) {
            return false;
        }
        stringIndex++;
    }

    numberOfCharsInStringRepresentationOfUINT_MAX = getNumberOfCharsInStringRepresentationOfUnsigned(UINT_MAX);

    if (numberOfCharsInString > numberOfCharsInStringRepresentationOfUINT_MAX) {
        return false;
    }
    else if (numberOfCharsInString == numberOfCharsInStringRepresentationOfUINT_MAX) {
        stringRepresentationOfUINT_MAX = getStringRepresentationOfUnsigned(UINT_MAX);

        if(strcmp(string, stringRepresentationOfUINT_MAX) > 0) {
            free((char*)stringRepresentationOfUINT_MAX);
            return false;
        }
        free((char*)stringRepresentationOfUINT_MAX);
    }

    return true;
}

/** @brief Sprawdza czy napis przedstawia liczbę typu int.
 * Akceptuje zera wiodące.
 * @param string
 * @return Wartość @p true, jeśli napis przedstawia liczbę typu int.
 * Wartość @p false, jeśli napis nie przedstawia liczby typu int.
 */
bool checkIfStringCanBeRepresentedAsInt(const char* string) {

    int numberOfCharsInStringRepresentationOfINT_MAX;
    const char* stringRepresentationOfINT_MAX = NULL;
    int numberOfCharsInStringRepresentationOfINT_MIN;
    const char* stringRepresentationOfINT_MIN = NULL;
    int stringIndex = 0;
    int numberOfCharsInString = getNumberOfCharsInString(string);

    if (string == NULL) {
        return false;
    }

    if (string[0] == '-') {
        stringIndex = 1;

        while (string[stringIndex] != '\0') {
            if(isdigit((int)string[stringIndex]) == 0) {
                return false;
            }
            stringIndex++;
        }

        numberOfCharsInStringRepresentationOfINT_MIN = getNumberOfCharsInStringRepresentationOfInt(INT_MIN);

        if (numberOfCharsInString > numberOfCharsInStringRepresentationOfINT_MIN) {
            return false;
        }
        else if (numberOfCharsInString == numberOfCharsInStringRepresentationOfINT_MIN) {
            stringRepresentationOfINT_MIN = getStringRepresentationOfInt(INT_MIN);

            if(strcmp(string, stringRepresentationOfINT_MIN) > 0) {
                free((char*)stringRepresentationOfINT_MIN);
                return false;
            }
            free((char*)stringRepresentationOfINT_MIN);
        }
    }
    else {
        while (string[stringIndex] != '\0') {
            if(isdigit((int)string[stringIndex]) == 0) {
                return false;
            }
            stringIndex++;
        }

        numberOfCharsInStringRepresentationOfINT_MAX = getNumberOfCharsInStringRepresentationOfInt(INT_MAX);

        if (numberOfCharsInString > numberOfCharsInStringRepresentationOfINT_MAX) {
            return false;
        }
        else if (numberOfCharsInString == numberOfCharsInStringRepresentationOfINT_MAX) {
            stringRepresentationOfINT_MAX = getStringRepresentationOfInt(INT_MAX);

            if(strcmp(string, stringRepresentationOfINT_MAX) > 0) {
                free((char*)stringRepresentationOfINT_MAX);
                return false;
            }
            free((char*)stringRepresentationOfINT_MAX);
        }
    }

    return true;
}

/** @brief Zwraca reprezentację napisu typu c jako liczby typu unsigned.
 * Akceptuje zera wiodące.
 * Jeśli napisu nie da się przedstawić jako liczbę typu unsigned, to zachowanie funkcji jest niezdefiniowane.
 * @param[in] string
 * @return reprezentacja napisu typu c jako liczby typu unsigned.
 */
unsigned getUnsignedRepresentationOfString(const char* string) {

    int stringIndex = 0;
    unsigned number = 0;

    while (string[stringIndex] != '\0') {
        number *= 10;
        number += (string[stringIndex] - '0');

        stringIndex++;
    }

    return number;
}

/** @brief Zwraca reprezentację napisu typu c jako liczby typu int.
 * Akceptuje zera wiodące.
 * Jeśli napisu nie da się przedstawić jako liczbę typu int, to zachowanie funkcji jest niezdefiniowane.
 * @param[in] string
 * @return reprezentacja napisu typu c jako liczby typu int.
 */
int getIntRepresentationOfString(const char* string) {

    int stringIndex = 0;
    int number = 0;

    if (string[0] == '-') {
        stringIndex = 1;

        while (string[stringIndex] != '\0') {
            number *= 10;
            number -= (string[stringIndex] - '0');

            stringIndex++;
        }
    }
    else {
        while (string[stringIndex] != '\0') {
            number *= 10;
            number += (string[stringIndex] - '0');

            stringIndex++;
        }
    }

    return number;
}

/** @brief Zwraca liczbę charów w napisie typu c.
 * @param[in] string
 * @return liczba charów
 */
int getNumberOfCharsInString(const char *string) {

    int numberOfChars = 0;

    while (string[numberOfChars] != '\0') {
        numberOfChars++;
    }

    numberOfChars++;

    return numberOfChars;
}