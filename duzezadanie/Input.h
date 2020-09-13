/** @file
 * Interfejs modułu wczytującego polecenia.
 *
 * @author Jan Dziuba <	jd406140@students.mimuw.edu.pl>
 */

#ifndef DROGI_INPUT_H
#define DROGI_INPUT_H

#include <stdbool.h>
#include "map.h"

/** @brief Czyta linię ze standardowego wejścia, sprawdza czy jest poleceniem i je wykonuje.
 * Puste linie i linie zaczynające się znakiem # ignoruje.
 * Jeśli polecenie jest niepoprawne wypisuje wiadomość na standardowe wyjście diagnostyczne.
 * @param[in,out] map
 * @param[in] numberOfLine - numer czytanej linii.
 * @param[out] pointerToHasInputEnded - Wskaźnik na czy dane wejściowe się skończyły.
 */
void readLineAndExecuteCommand(Map *map, int numberOfLine, bool *pointerToHasInputEnded);

#endif //DROGI_INPUT_H
