/** @file
 * Interfejs biblioteki operacji na napisach.
 *
 * @author Jan Dziuba <	jd406140@students.mimuw.edu.pl>
 */

#ifndef DROGI_STRING_H
#define DROGI_STRING_H

#include <stdbool.h>

/** @brief Zwraca liczbę charów w reprezentacji zmiennej typu unsigned jako napisu typu c.
 * @param[in] number
 * @return liczba charów
 */
int getNumberOfCharsInStringRepresentationOfUnsigned(unsigned number);

/** @brief Zwraca liczbę charów w reprezentacji zmiennej typu int jako napisu typu c.
 * @param[in] number
 * @return liczba charów
 */
int getNumberOfCharsInStringRepresentationOfInt(int number);

/** @brief Zwraca reprezentację zmiennej typu unsigned jako napisu typu c.
 * Zaalokowaną pamięć trzeba zwolnić za pomocą funkcji free.
 * @param[in] number
 * @return Utworzony napis lub NULL gdy nie udało się zaalokować
 * pamięci.
 */
const char* getStringRepresentationOfUnsigned(unsigned number);

/** @brief Zwraca reprezentację zmiennej typu int jako napisu typu c.
 * Zaalokowaną pamięć trzeba zwolnić za pomocą funkcji free.
 * @param[in] number
 * @return Utworzony napis lub NULL gdy nie udało się zaalokować
 * pamięci.
 */
const char* getStringRepresentationOfInt(int number);

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
                                                   int *pointerToIndexOfString, int stringInConstructionLength);

/** @brief Sprawdza czy napis przedstawia liczbę typu unsigned.
 * Akceptuje zera wiodące.
 * @param string
 * @return Wartość @p true, jeśli napis przedstawia liczbę typu unsigned.
 * Wartość @p false, jeśli napis nie przedstawia liczby typu unsigned.
 */
bool checkIfStringCanBeRepresentedAsUnsigned(const char* string);

/** @brief Sprawdza czy napis przedstawia liczbę typu int.
 * Akceptuje zera wiodące.
 * @param string
 * @return Wartość @p true, jeśli napis przedstawia liczbę typu int.
 * Wartość @p false, jeśli napis nie przedstawia liczby typu int.
 */
bool checkIfStringCanBeRepresentedAsInt(const char* string);

/** @brief Zwraca reprezentację napisu typu c jako liczby typu unsigned.
 * Akceptuje zera wiodące.
 * Jeśli napisu nie da się przedstawić jako liczbę typu unsigned, to zachowanie funkcji jest niezdefiniowane.
 * @param[in] string
 * @return reprezentacja napisu typu c jako liczby typu unsigned.
 */
unsigned getUnsignedRepresentationOfString(const char* string);

/** @brief Zwraca reprezentację napisu typu c jako liczby typu int.
 * Akceptuje zera wiodące.
 * Jeśli napisu nie da się przedstawić jako liczbę typu int, to zachowanie funkcji jest niezdefiniowane.
 * @param[in] string
 * @return reprezentacja napisu typu c jako liczby typu int.
 */
int getIntRepresentationOfString(const char* string);

/** @brief Zwraca liczbę charów w napisie typu c.
 * @param[in] string
 * @return liczba charów
 */
int getNumberOfCharsInString(const char *string);

#endif //DROGI_STRING_H
