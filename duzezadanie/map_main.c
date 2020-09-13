/** @file
 * Główny plik programu operacji na mapach drogowych.
 *
 * @author Jan Dziuba <	jd406140@students.mimuw.edu.pl>
 */

#include "map.h"
#include "Input.h"

#undef NDEBUG

/** @brief Główna funkcja programu operacji na mapach drogowych.
 * Funkcja czyta polecenia z standardowego wejścia i je wykonuje.
 * @return 0.
 */
int main() {

    int numberOfLine = 1;
    Map* map = newMap();
    bool hasInputEnded = false;

    while (hasInputEnded == false) {
        readLineAndExecuteCommand(map, numberOfLine, &hasInputEnded);
        numberOfLine++;
    }

    deleteMap(map);

    return 0;
}
