/** @file
 * Moduł wczytuje polecenia i przekształca je do postaci obsługiwalnej przez funkcje z @ref map.c.
 *
 * @author Jan Dziuba <	jd406140@students.mimuw.edu.pl>
 */

#include "Input.h"
#include "string.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * Początkowy rozmiar alokacji pamięci.
 */
#define INITIAL_MALLOC_SIZE 10

/**
 * Mnoźnik używany przy realokacji pamięci
 */
#define MULTIPLIER 2

/** @brief Czyta parametr z standardowego wejścia.
 * Parametr to ciąg znaków nie zawierający ';'
 * Parametry są oddzielone znakiem ';' lub '\\n'.
 * @param[out] pointerToHasLineEnded - wskaźnik na czy wczytywana linia się skończyła.
 * @return Parametr lub NULL, gdy nie udało się zaalokować pamięci.
 */
static const char* readLineParameter(bool *pointerToHasLineEnded) {

    int inputCharacter = 0;
    int maxNumberOfCharsInParameter = 0;
    int numberOfCharsInParameter = 0;
    char *parameter = NULL;
    char *temporary = NULL;

    *pointerToHasLineEnded = false;

    maxNumberOfCharsInParameter = INITIAL_MALLOC_SIZE;
    parameter = (char*)malloc(sizeof(char) * maxNumberOfCharsInParameter);
    if (parameter == NULL) {
        return NULL;
    }

    inputCharacter = getchar();
    numberOfCharsInParameter++;

    while ((inputCharacter != ';') && (inputCharacter != '\n')) {

        if (numberOfCharsInParameter > maxNumberOfCharsInParameter) {
            maxNumberOfCharsInParameter *= MULTIPLIER;
            temporary = parameter;
            parameter = (char*)realloc(temporary, sizeof(char) * maxNumberOfCharsInParameter);
            if (parameter == NULL) {
                free(temporary);
                return NULL;
            }
        }

        parameter[numberOfCharsInParameter - 1] = (char)inputCharacter;

        inputCharacter = getchar();
        numberOfCharsInParameter++;
    }

    if (numberOfCharsInParameter > maxNumberOfCharsInParameter) {
        maxNumberOfCharsInParameter *= MULTIPLIER;
        temporary = parameter;
        parameter = (char*)realloc(temporary, sizeof(char) * maxNumberOfCharsInParameter);
        if (parameter == NULL) {
            free(temporary);
            return NULL;
        }
    }

    parameter[numberOfCharsInParameter - 1] = '\0';

    if(inputCharacter == '\n') {
        *pointerToHasLineEnded = true;
    }

    return parameter;
}

/** @brief Usuwa polecenie wczytane z standardowego wejścia.
 * @param[in] numberOfParametersInLine - liczba parametrów w poleceniu.
 * @param[in,out] parametersOfLine - Tablica parametrów polecenia wczytanego za pomocą @ref readLine.
 */
static void deleteLine(int numberOfParametersInLine, char** parametersOfLine) {
    char *parameter = NULL;

    for(int index = 0; index < numberOfParametersInLine; index++) {
        parameter = parametersOfLine[index];
        free(parameter);
    }

    free(parametersOfLine);
}

/** @brief Wczytuje polecenie ze standardowego wejścia.
 * Polecenie to nie pusta, nie zaczynająca się od '#' linia z wejścia standardowego.
 * Polecenie zapisuje jako tablicę parametrów.
 * Parametr to ciąg znaków nie zawierający ';'
 * Parametry w linii są oddzielone znakiem ';' lub '\\n'.
 * @param[out] pointerToNumberOfParametersInLine - Wskaźnik na liczbę parametrów w poleceniu.
 * @param[out] pointerToHasInputEnded - Wskaźnik na czy dane wejściowe się skończyły.
 * @return Polecenie lub NULL, gdy linia nie jest poleceniem lub nie udało się zaalokować pamięci.
 */
static const char*const* readLine(int *pointerToNumberOfParametersInLine, bool *pointerToHasInputEnded) {

    int numberOfParametersInLine = 0;
    int maxNumberOfParametersInLine = 0;
    int inputCharacter = 0;
    const char **parametersOfLine = NULL;
    const char **temporary;
    bool hasLineEnded = false;

    inputCharacter = getchar();

    if (inputCharacter == EOF) {
        *pointerToHasInputEnded = true;
        return NULL;
    }

    if (inputCharacter == '\n') {
        return NULL;
    }

    if (inputCharacter == '#') {
        while (inputCharacter != '\n') {
            inputCharacter = getchar();
        }
        return NULL;
    }

    ungetc(inputCharacter, stdin);

    maxNumberOfParametersInLine = INITIAL_MALLOC_SIZE;
    parametersOfLine = (const char**)malloc(sizeof(char*) * maxNumberOfParametersInLine);
    if (parametersOfLine == NULL) {
        *pointerToHasInputEnded = true;
        return NULL;
    }

    while (hasLineEnded == false) {
        numberOfParametersInLine++;

        if (numberOfParametersInLine > maxNumberOfParametersInLine) {
            maxNumberOfParametersInLine *= MULTIPLIER;
            temporary = parametersOfLine;
            parametersOfLine = (const char**)realloc(temporary, sizeof(char*) * maxNumberOfParametersInLine);
            if (parametersOfLine == NULL) {
                deleteLine(numberOfParametersInLine, (char**)temporary);
                *pointerToHasInputEnded = true;
                return NULL;
            }
        }

        parametersOfLine[numberOfParametersInLine - 1] = readLineParameter(&hasLineEnded);
        if (parametersOfLine[numberOfParametersInLine - 1] == NULL) {
            deleteLine(numberOfParametersInLine, (char**)parametersOfLine);
            *pointerToHasInputEnded = true;
            return NULL;
        }
    }

    *pointerToNumberOfParametersInLine = numberOfParametersInLine;

    return parametersOfLine;
}

/** @brief Sprawdza czy polecenie jest poleceniem dodania drogi.
 * @param[in] parametersOfLine - Tablica parametrów polecenia wczytanego za pomocą @ref readLine.
 * @param[in] numberOfParametersInLine - liczba parametrów w poleceniu.
 * @return Wartość @p true, jeśli polecenie jest poleceniem dodania drogi.
 * Wartość @p false, jeśli polecenie nie jest poleceniem dodania drogi.
 */
static bool checkIfLineIsAddRoadCommand(const char *const*parametersOfLine, int numberOfParametersInLine) {

    if (numberOfParametersInLine != 5) {
        return false;
    }

    if (strcmp(parametersOfLine[0], "addRoad") != 0) {
        return false;
    }

    if (checkIfStringCanBeRepresentedAsUnsigned(parametersOfLine[3]) == false) {
        return false;
    }

    if (checkIfStringCanBeRepresentedAsInt(parametersOfLine[4]) == false) {
        return false;
    }

    return true;
}

/** @brief Sprawdza czy polecenie jest poleceniem naprawy drogi.
 * @param[in] parametersOfLine - Tablica parametrów polecenia wczytanego za pomocą @ref readLine.
 * @param[in] numberOfParametersInLine - liczba parametrów w poleceniu.
 * @return Wartość @p true, jeśli polecenie jest poleceniem naprawy drogi.
 * Wartość @p false, jeśli polecenie nie jest poleceniem naprawy drogi.
 */
static bool checkIfLineIsRepairRoadCommand(const char *const*parametersOfLine, int numberOfParametersInLine) {

    if (numberOfParametersInLine != 4) {
        return false;
    }

    if (strcmp(parametersOfLine[0], "repairRoad") != 0) {
        return false;
    }

    if (checkIfStringCanBeRepresentedAsInt(parametersOfLine[3]) == false) {
        return false;
    }

    return true;
}

/** @brief Sprawdza czy polecenie jest poleceniem dodania drogi krajowej.
 * @param[in] parametersOfLine - Tablica parametrów polecenia wczytanego za pomocą @ref readLine.
 * @param[in] numberOfParametersInLine - liczba parametrów w poleceniu.
 * @return Wartość @p true, jeśli polecenie jest poleceniem dodania drogi krajowej.
 * Wartość @p false, jeśli polecenie nie jest poleceniem dodania drogi krajowej.
 */
static bool checkIfLineIsNewRouteCommand(const char *const*parametersOfLine, int numberOfParametersInLine) {

    if (numberOfParametersInLine != 4) {
        return false;
    }

    if (strcmp(parametersOfLine[0], "newRoute") != 0) {
        return false;
    }

    if (checkIfStringCanBeRepresentedAsUnsigned(parametersOfLine[1]) == false) {
        return false;
    }

    return true;
}

/** @brief Sprawdza czy polecenie jest poleceniem usunięcia drogi.
 * @param[in] parametersOfLine - Tablica parametrów polecenia wczytanego za pomocą @ref readLine.
 * @param[in] numberOfParametersInLine - liczba parametrów w poleceniu.
 * @return Wartość @p true, jeśli polecenie jest poleceniem usunięcia drogi.
 * Wartość @p false, jeśli polecenie nie jest poleceniem usunięcia drogi.
 */
static bool checkIfLineIsRemoveRoadCommand(const char *const*parametersOfLine, int numberOfParametersInLine) {

    if (numberOfParametersInLine != 3) {
        return false;
    }

    if (strcmp(parametersOfLine[0], "removeRoad") != 0) {
        return false;
    }

    return true;
}

/** @brief Sprawdza czy polecenie jest poleceniem rozszerzenia drogi krajowej.
 * @param[in] parametersOfLine - Tablica parametrów polecenia wczytanego za pomocą @ref readLine.
 * @param[in] numberOfParametersInLine - liczba parametrów w poleceniu.
 * @return Wartość @p true, jeśli polecenie jest poleceniem rozszerzenia drogi krajowej.
 * Wartość @p false, jeśli polecenie nie jest poleceniem rozszerzenia drogi krajowej.
 */
static bool checkIfLineIsExtendRouteCommand(const char *const*parametersOfLine, int numberOfParametersInLine) {

    if (numberOfParametersInLine != 3) {
        return false;
    }

    if (strcmp(parametersOfLine[0], "extendRoute") != 0) {
        return false;
    }

    if (checkIfStringCanBeRepresentedAsUnsigned(parametersOfLine[1]) == false) {
        return false;
    }

    return true;
}

/** @brief Sprawdza czy polecenie jest poleceniem udostępnienia informacji o drodze krajowej.
 * @param[in] parametersOfLine - Tablica parametrów polecenia wczytanego za pomocą @ref readLine.
 * @param[in] numberOfParametersInLine - liczba parametrów w poleceniu.
 * @return Wartość @p true, jeśli polecenie jest poleceniem udostępnienia informacji o drodze krajowej.
 * Wartość @p false, jeśli polecenie nie jest poleceniem udostępnienia informacji o drodze krajowej.
 */
static bool checkIfLineIsGetRouteDescriptionCommand(const char *const*parametersOfLine, int numberOfParametersInLine) {

    if (numberOfParametersInLine != 2) {
        return false;
    }

    if (strcmp(parametersOfLine[0], "getRouteDescription") != 0) {
        return false;
    }

    if (checkIfStringCanBeRepresentedAsUnsigned(parametersOfLine[1]) == false) {
        return false;
    }

    return true;
}

/** @brief Sprawdza czy polecenie jest poleceniem usunięcia drogi krajowej.
 * @param[in] parametersOfLine - Tablica parametrów polecenia wczytanego za pomocą @ref readLine.
 * @param[in] numberOfParametersInLine - liczba parametrów w poleceniu.
 * @return Wartość @p true, jeśli polecenie jest poleceniem usunięcia drogi krajowej.
 * Wartość @p false, jeśli polecenie nie jest poleceniem usunięcia drogi krajowej.
 */
static bool checkIfLineIsRemoveRouteCommand(const char *const*parametersOfLine, int numberOfParametersInLine) {

    if (numberOfParametersInLine != 2) {
        return false;
    }

    if (strcmp(parametersOfLine[0], "removeRoute") != 0) {
        return false;
    }

    if (checkIfStringCanBeRepresentedAsUnsigned(parametersOfLine[1]) == false) {
        return false;
    }

    return true;
}

/** @brief Wykonuje polecenie dodania drogi.
 * @param[in,out] map
 * @param[in] parametersOfLine - Tablica parametrów polecenia wczytanego za pomocą @ref readLine.
 * @param[in] numberOfParametersInLine - liczba parametrów w poleceniu.
 * @return Wartość @p true, jeśli udało się dodać drogę.
 * Wartość @p false, jeśli polecenie nie jest poleceniem dodania drogi lub nie udało się dodać drogi.
 */
static bool executeAddRoadCommand(Map *map, const char *const*parametersOfLine, int numberOfParametersInLine) {

    const char *city1 = NULL;
    const char *city2 = NULL;
    unsigned length;
    int builtYear;

    if (checkIfLineIsAddRoadCommand(parametersOfLine, numberOfParametersInLine) == false) {
        return false;
    }

    city1 = parametersOfLine[1];
    city2 = parametersOfLine[2];
    length = getUnsignedRepresentationOfString(parametersOfLine[3]);
    builtYear = getIntRepresentationOfString(parametersOfLine[4]);

    return addRoad(map, city1, city2, length, builtYear);
}

/** @brief Wykonuje polecenie naprawy drogi.
 * @param[in,out] map
 * @param[in] parametersOfLine - Tablica parametrów polecenia wczytanego za pomocą @ref readLine.
 * @param[in] numberOfParametersInLine - liczba parametrów w poleceniu.
 * @return Wartość @p true, jeśli udało się naprawić drogę.
 * Wartość @p false, jeśli polecenie nie jest poleceniem naprawy drogi lub nie udało się naprawić drogi.
 */
static bool executeRepairRoadCommand(Map *map, const char *const*parametersOfLine, int numberOfParametersInLine) {

    const char *city1 = NULL;
    const char *city2 = NULL;
    int repairYear;

    if (checkIfLineIsRepairRoadCommand(parametersOfLine, numberOfParametersInLine) == false) {
        return false;
    }

    city1 = parametersOfLine[1];
    city2 = parametersOfLine[2];
    repairYear = getIntRepresentationOfString(parametersOfLine[3]);

    return repairRoad(map, city1, city2, repairYear);
}

/** @brief Wykonuje polecenie usuwania drogi.
 * @param[in,out] map
 * @param[in] parametersOfLine - Tablica parametrów polecenia wczytanego za pomocą @ref readLine.
 * @param[in] numberOfParametersInLine - liczba parametrów w poleceniu.
 * @return Wartość @p true, jeśli udało się usunąć drogę.
 * Wartość @p false, jeśli polecenie nie jest poleceniem usuwania drogi lub nie udało się usunąć drogi.
 */
static bool executeRemoveRoadCommand(Map *map, const char *const*parametersOfLine, int numberOfParametersInLine) {

    const char *city1 = NULL;
    const char *city2 = NULL;

    if (checkIfLineIsRemoveRoadCommand(parametersOfLine, numberOfParametersInLine) == false) {
        return false;
    }

    city1 = parametersOfLine[1];
    city2 = parametersOfLine[2];

    return removeRoad(map, city1, city2);
}

/** @brief Wykonuje polecenie dodania drogi krajowej.
 * @param[in,out] map
 * @param[in] parametersOfLine - Tablica parametrów polecenia wczytanego za pomocą @ref readLine.
 * @param[in] numberOfParametersInLine - liczba parametrów w poleceniu.
 * @return Wartość @p true, jeśli udało się dodać drogę krajową.
 * Wartość @p false, jeśli polecenie nie jest poleceniem dodania drogi krajowej lub nie udało się dodać drogi krajowej.
 */
static bool executeNewRouteCommand(Map *map, const char *const*parametersOfLine, int numberOfParametersInLine) {

    unsigned routeId;
    const char *city1 = NULL;
    const char *city2 = NULL;

    if (checkIfLineIsNewRouteCommand(parametersOfLine, numberOfParametersInLine) == false) {
        return false;
    }

    routeId = getUnsignedRepresentationOfString(parametersOfLine[1]);
    city1 = parametersOfLine[2];
    city2 = parametersOfLine[3];

    return newRoute(map, routeId, city1, city2);
}

/** @brief Wykonuje polecenie rozszerzenia drogi krajowej.
 * @param[in,out] map
 * @param[in] parametersOfLine - Tablica parametrów polecenia wczytanego za pomocą @ref readLine.
 * @param[in] numberOfParametersInLine - liczba parametrów w poleceniu.
 * @return Wartość @p true, jeśli udało się rozszerzyć drogę krajową.
 * Wartość @p false, jeśli polecenie nie jest poleceniem rozszerzenia drogi krajowej
 * lub nie udało się rozszerzyć drogi krajowej.
 */
static bool executeExtendRouteCommand(Map *map, const char *const*parametersOfLine, int numberOfParametersInLine) {

    unsigned routeId;
    const char *city = NULL;

    if (checkIfLineIsExtendRouteCommand(parametersOfLine, numberOfParametersInLine) == false) {
        return false;
    }

    routeId = getUnsignedRepresentationOfString(parametersOfLine[1]);
    city = parametersOfLine[2];

    return extendRoute(map, routeId, city);
}

/** @brief Wykonuje polecenie udostępnienia informacji o drodze krajowej.
 * @param[in,out] map
 * @param[in] parametersOfLine - Tablica parametrów polecenia wczytanego za pomocą @ref readLine.
 * @param[in] numberOfParametersInLine - liczba parametrów w poleceniu.
 * @return Wartość @p true, jeśli udało się udostępnić informacje o drodze krajowej.
 * Wartość @p false, jeśli polecenie nie jest poleceniem udostępnienia informacji o drodze krajowej.
 * lub nie udało się udostępnić informacji o drodze krajowej.
 */
static bool executeGetRouteDescriptionCommand(Map *map, const char *const*parametersOfLine,
        int numberOfParametersInLine) {

    unsigned routeId;
    char const* routeDescription = NULL;

    if (checkIfLineIsGetRouteDescriptionCommand(parametersOfLine, numberOfParametersInLine) == false) {
        return false;
    }

    routeId = getUnsignedRepresentationOfString(parametersOfLine[1]);

    routeDescription = getRouteDescription(map, routeId);
    if (routeDescription == NULL) {
        return false;
    }

    printf("%s\n", routeDescription);

    free((char*)routeDescription);

    return true;
}

/** @brief Wykonuje polecenie usunięcia drogi krajowej.
 * @param[in,out] map
 * @param[in] parametersOfLine - Tablica parametrów polecenia wczytanego za pomocą @ref readLine.
 * @param[in] numberOfParametersInLine - liczba parametrów w poleceniu.
 * @return Wartość @p true, jeśli udało się usunąć drogę krajową.
 * Wartość @p false, jeśli polecenie nie jest poleceniem usunięcia drogi krajowej.
 * lub nie udało się usunąć drogi krajowej.
 */
static bool executeRemoveRouteCommand(Map *map, const char *const*parametersOfLine, int numberOfParametersInLine) {

    unsigned routeId;

    if (checkIfLineIsRemoveRouteCommand(parametersOfLine, numberOfParametersInLine) == false) {
        return false;
    }

    routeId = getUnsignedRepresentationOfString(parametersOfLine[1]);

    return removeRoute(map, routeId);
}

/** @brief Czyta linię ze standardowego wejścia, sprawdza czy jest poleceniem i je wykonuje.
 * Puste linie i linie zaczynające się znakiem # ignoruje.
 * Jeśli polecenie jest niepoprawne wypisuje wiadomość na standardowe wyjście diagnostyczne.
 * @param[in,out] map
 * @param[in] numberOfLine - numer czytanej linii.
 * @param[out] pointerToHasInputEnded - Wskaźnik na czy dane wejściowe się skończyły.
 */
void readLineAndExecuteCommand(Map *map, int numberOfLine, bool *pointerToHasInputEnded) {

    int numberOfParametersInLine = 0;
    const char *const*parametersOfLine = NULL;
    bool result = false;
    bool hasInputEnded = false;

    parametersOfLine = readLine(&numberOfParametersInLine, &hasInputEnded);
    if (parametersOfLine == NULL) {
        result = true;
    }
    else {
        if (executeAddRoadCommand(map, parametersOfLine, numberOfParametersInLine) == true) {
            result = true;
        }
        else if (executeRepairRoadCommand(map, parametersOfLine, numberOfParametersInLine) == true) {
            result = true;
        }
        else if (executeRemoveRoadCommand(map, parametersOfLine, numberOfParametersInLine) == true) {
            result = true;
        }
        else if (executeNewRouteCommand(map, parametersOfLine, numberOfParametersInLine) == true) {
            result = true;
        }
        else if (executeExtendRouteCommand(map, parametersOfLine, numberOfParametersInLine) == true) {
            result = true;
        }
        else if (executeGetRouteDescriptionCommand(map, parametersOfLine, numberOfParametersInLine) == true) {
            result = true;
        }
        else if (newCustomRoute(map, parametersOfLine, numberOfParametersInLine) == true) {
            result = true;
        }
        else if (executeRemoveRouteCommand(map, parametersOfLine, numberOfParametersInLine) == true) {
            result = true;
        }
    }
    deleteLine(numberOfParametersInLine, (char**)parametersOfLine);

    if (result == false) {
        fprintf(stderr, "ERROR %d\n", numberOfLine);
    }

    *pointerToHasInputEnded = hasInputEnded;
}
