/** @file
 * Klasa przechowująca mapę dróg krajowych
 *
 * @author Jan Dziuba <	jd406140@students.mimuw.edu.pl>
 */

#include "map.h"
#include "string.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>

/**
 * Maksymalny dozwolony numer drogi krajowej.
 */
#define MAX_ROUTE_ID 999

/**
 * Struktura Przechowująca miasto.
 */
typedef struct City {
    struct RoadList *roadsFromThisCity; /**< Lista Dróg wychodzących z tego miasta. */
    unsigned long distanceInRouteSearch; /**< Odległość tego miasta od pierwszego miasta w algorytmie tworzenia
 * drogi krajowej. @ref assignDistancesOfRouteSearch */
    bool wasCityGivenDistanceInRouteSearch; /**< Czy miasto ma przypisaną odległość w algorytmie tworzenia
 * drogi krajowej. @ref assignDistancesOfRouteSearch */
    char *cityName; /**< Nazwa miasta */
} City;

/**
 * Lista miast.
 */
typedef struct CityList {
    struct CityList *next; /**< Następny element na liście. */
    struct City *city; /**< Wskaźnik na Miasto. */
} CityList;

/**
 * Struktura przechowująca drogę.
 *
 * Kolejność miast niewiadoma.
 */
typedef struct Road {
    unsigned length; /**< Długość drogi */
    struct City *city1; /**< Wskaźnik na pierwsze miasto które łączy droga */
    struct City *city2; /**< Wskaźnik na drugie miasto które łączy droga */
    int dateOfConstructionOrLastRepair; /**< Jeśli droga nie była remontowana zmienna przechowuje rok budowy.
 * Jeśli droga była remontowana zmienna przechowuje rok ostatniego remontu. */
    struct RouteList *RoutesContainingThisRoad; /**< Lista dróg krajowych zawierających tę drogę. */
} Road;

/**
 * Lista dróg.
 */
typedef struct RoadList {
    struct RoadList *next; /**< Następny element na liście. */
    struct Road *road; /**< Wskaźnik na drogę. */
} RoadList;

/**
 * Struktura przechowująca drogę krajową.
 *
 * Kolejność miast i dróg na liście jest taka, jak wypisuje je funkcja @ref getRouteDescription.
 */
typedef struct Route {
    unsigned routeId; /**< Numer identyfikacyjny drogi krajowej. */
    bool doesRouteWithThisIdExist; /**< Zmienna mówiąca czy droga krajowa zaalokowana pod tym adresem istnieje. */
    struct RoadList *roadsOfThisRoute; /**< Lista dróg wchodzących w skład tej drogi krajowej. */
    struct City *firstCity; /**< Wskaźnik na pierwsze miasto drogi krajowej. */
    struct City *lastCity; /**< Wskaźnik na ostatnie miasto drogi krajowej. */
} Route;

/**
 * Lista dróg krajowych.
 */
typedef struct RouteList {
    struct RouteList *next; /**< Następny element na liście. */
    struct Route *route; /**< Wskaźnik na drogę Krajową. */
} RouteList;

/**
 * Struktura przechowująca mapę dróg krajowych.
 */
typedef struct Map {
    struct CityList *cities; /**< Lista miast znajdujących się na mapie. */
    struct RoadList *roads; /**< Lista dróg znajdujących się na mapie. */
    struct Route *routeArray; /**< Tablica zawierająca drogi krajowe posortowane według numerów identyfikacyjnych.
 * Tablica zawiera drogę krajową o indeksie 0*/
} Map;


/** @brief Tworzy nową strukturę.
 * Tworzy nową, pustą strukturę niezawierającą żadnych miast, odcinków dróg ani
 * dróg krajowych.
 * @return Wskaźnik na utworzoną strukturę lub NULL, gdy nie udało się
 * zaalokować pamięci.
 */
Map* newMap(void) {

    Map *map = (Map*)malloc(sizeof(Map));
    if (map == NULL) {
        return NULL;
    }

    map->cities = NULL;
    map->roads = NULL;
    map->routeArray = (Route*)malloc(sizeof(Route) * (MAX_ROUTE_ID + 1));

    if (map->routeArray == NULL) {
        deleteMap(map);
        return NULL;
    }

    for (unsigned routeId = 0; routeId <= MAX_ROUTE_ID; routeId++) {
        map->routeArray[routeId].routeId = routeId;
        map->routeArray[routeId].doesRouteWithThisIdExist = false;
        map->routeArray[routeId].roadsOfThisRoute = NULL;
    }

    return map;
}

/** @brief Usuwa Listę dróg krajowych z drogi.
 * @param[in,out] road - Wskaźnik na drogę z której jest usuwana lista dróg krajowych zawierających tę drogę.
 */
static void deleteRouteListFromRoad(Road *road) {

    RouteList *routeList = NULL;

    if (road != NULL) {
        routeList = road->RoutesContainingThisRoad;

        while (routeList != NULL) {
            RouteList *previous = routeList;
            routeList = routeList->next;
            free(previous);
        }
        road->RoutesContainingThisRoad = NULL;
    }
}

/** @brief Usuwa listę dróg.
 *  nie usuwa dróg znajdujących się na liście.
 * @param[in,out] pointerToRoadList
 */
static void deleteRoadList(RoadList **pointerToRoadList) {

    RoadList *roadList = NULL;
    RoadList *previous = NULL;

    if (pointerToRoadList != NULL) {
        roadList = *pointerToRoadList;

        while (roadList != NULL) {
            previous = roadList;
            roadList = roadList->next;
            free(previous);
        }
        *pointerToRoadList = NULL;
    }
}

/** @brief Usuwa drogi krajowe z mapy.
 * Usuwa drogi krajowe z mapy oraz innych struktur ich dotyczących.
 * @param[in,out] map - Wskaźnik na mapę z której są usuwane drogi krajowe.
 */
static void deleteRoutesFromMap(Map *map) {

    RoadList *roadListOfMap = NULL;
    Route *routeArrayOfMap = NULL;

    if (map != NULL) {
        roadListOfMap = map->roads;
        routeArrayOfMap = map->routeArray;

        while (roadListOfMap != NULL) {
            deleteRouteListFromRoad(roadListOfMap->road);
            roadListOfMap = roadListOfMap->next;
        }

        if (routeArrayOfMap != NULL) {
            for (unsigned routeId = 0; routeId <= MAX_ROUTE_ID; routeId++) {
                deleteRoadList(&(routeArrayOfMap[routeId].roadsOfThisRoute));
            }
            free(routeArrayOfMap);
        }
        map->routeArray = NULL;
    }
}

/** @brief Usuwa drogi z mapy.
 * Usuwa drogi z mapy oraz innych struktur ich dotyczących.
 * Usuwa także drogi krajowe z mapy, które nie mogą istnieć bez dróg.
 * @param[in,out] map - Wskaźnik na mapę z której są usuwane drogi krajowe.
 */
static void deleteRoadsFromMap(Map *map) {

    CityList *cityListOfMap = NULL;
    RoadList *roadListOfMap = NULL;

    if (map != NULL) {
        cityListOfMap = map->cities;
        roadListOfMap = map->roads;

        deleteRoutesFromMap(map);

        while (cityListOfMap != NULL) {
            deleteRoadList(&cityListOfMap->city->roadsFromThisCity);
            cityListOfMap = cityListOfMap->next;
        }

        while (roadListOfMap != NULL) {
            deleteRouteListFromRoad(roadListOfMap->road);
            free(roadListOfMap->road);
            RoadList *previous = roadListOfMap;
            roadListOfMap = roadListOfMap->next;
            free(previous);
        }
        map->roads = NULL;
    }
}

/** @brief Usuwa miasta z mapy.
 * Usuwa miasta z mapy oraz innych struktur ich dotyczących.
 * Usuwa także drogi krajowe, oraz drogi z mapy, które nie mogą istnieć bez miast.
 * @param[in,out] map - Wskaźnik na mapę z której są usuwane drogi krajowe.
 */
static void deleteCitiesFromMap(Map *map) {

    CityList *cityListOfMap = NULL;

    if (map != NULL) {
        cityListOfMap = map->cities;

        deleteRoutesFromMap(map);
        deleteRoadsFromMap(map);

        while (cityListOfMap != NULL) {
            deleteRoadList(&cityListOfMap->city->roadsFromThisCity);

            if (cityListOfMap->city != NULL) {
                free(cityListOfMap->city->cityName);
            }
            free(cityListOfMap->city);
            CityList *previous = cityListOfMap;
            cityListOfMap = cityListOfMap->next;
            free(previous);
        }
        map->cities = NULL;
    }
}

/** @brief Usuwa miasto z którego nie wychodzą żadne drogi z mapy.
 * @param[in,out] map - Wskaźnik na mapę z której usuwane jest miasto.
 * @param[in,out] city - Wskaźnik na miasto, które jest usuwane z mapy.
 */
static void deleteCityContainingNoRoadsFromMap(Map *map, City *city) {

    CityList *citiesOfTheMap = map->cities;
    CityList *previousCity = NULL;
    City *currentCity = NULL;

    if (citiesOfTheMap != NULL) {
        currentCity = citiesOfTheMap->city;

        if (currentCity == city) {
            free(city->cityName);
            free(city);

            map->cities = citiesOfTheMap->next;
            free(citiesOfTheMap);
            return;
        }
    }

    previousCity = citiesOfTheMap;
    citiesOfTheMap = citiesOfTheMap->next;

    while (citiesOfTheMap != NULL) {
        currentCity = citiesOfTheMap->city;

        if (currentCity == city) {
            free(city->cityName);
            free(city);

            previousCity->next = citiesOfTheMap->next;
            free(citiesOfTheMap);
            return;
        }

        previousCity = citiesOfTheMap;
        citiesOfTheMap = citiesOfTheMap->next;
    }
}

/** @brief Usuwa strukturę.
 * Usuwa strukturę wskazywaną przez @p map.
 * Nic nie robi, jeśli wskaźnik ten ma wartość NULL.
 * @param[in] map        – wskaźnik na usuwaną strukturę.
 */
void deleteMap(Map *map) {

    deleteRoutesFromMap(map);
    deleteRoadsFromMap(map);
    deleteCitiesFromMap(map);

    free(map);
}

/** @brief Sprawdza czy nazwa miasta jest poprawna.
 * @param[in] city - nazwa miasta.
 * @return Wartość @p true, jeśli miasto ma poprawną nazwę.
 * Wartość @p false, jeśli nazwa miasta jest nie poprawna.
 */
static bool checkIfCityNameIsCorrect(const char *city) {

    int index = 0;

    if (city == NULL) {
        return false;
    }

    if (city[0] == '\0') {
        return false;
    }

    while (city[index] != '\0') {
        if (((city[index] >= 0) && (city[index] <= 31)) || (city[index] == ';')) {
            return false;
        }
        index++;
    }

    return true;
}

/** @brief Znajduje miasto o podanej nazwie.
 * @param[in] cityList - Lista miast, wśród której jest szukane miasto.
 * @param[in] cityName - nazwa miasta.
 * @return Wskaźnik na miasto, lub NULL gdy nie udało się znaleźć miasta.
 */
static City* findCityWithName(const CityList *cityList, const char *cityName) {

    City *city = NULL;

    while (cityList != NULL) {
        city = cityList->city;

        if (strcmp(city->cityName, cityName) == 0) {
            return city;
        }

        cityList = cityList->next;
    }

    return NULL;
}

/** @brief Znajduję miasto które łączy podana droga z podanym miastem.
 * @param[in] city - wskaźnik na miasto
 * @param[in] road - wskaźnik na drogę
 * @return Wskaźnik na miasto, lub NULL, jeśli podana droga nie wychodzi z podanego miasta.
 */
static City* getCityConnectedByRoad(const City *city, const Road *road) {

    if (road == NULL) {
        return NULL;
    }

    if (road->city1 == city) {
        return road->city2;
    }
    else if (road->city2 == city) {
        return road->city1;
    }
    else {
        return NULL;
    }
}

/** @brief Znajduje drogę łączącą podane miasta.
 * Kolejność podanych miast nie ma znaczenia.
 * @param[in] firstCity - Wskaźnik na pierwsze miasto, z którego wychodzi droga
 * @param[in] secondCity - Wskaźnik na drugie miasto, z którego wychodzi droga
 * @return Wskaźnik na drogę, jeśli droga łącząca miasta istnieje.
 * NULL, jeśli droga łącząca miasta nie istnieje.
 */
static Road* getRoadConnectingCities(const City *firstCity, const City *secondCity) {

    RoadList *roadsFromFirstCity = NULL;

    if ((firstCity == NULL) || (secondCity == NULL)) {
        return NULL;
    }

    roadsFromFirstCity = firstCity->roadsFromThisCity;

    while (roadsFromFirstCity != NULL) {
        if (getCityConnectedByRoad(firstCity, roadsFromFirstCity->road) == secondCity) {
            return roadsFromFirstCity->road;
        }

        roadsFromFirstCity = roadsFromFirstCity->next;
    }

    return NULL;
}

/** @brief Sprawdza poprawność parametrów funkcji @ref addRoad.
 * @param[in] map    – wskaźnik na strukturę przechowującą mapę dróg;
 * @param[in] city1      – wskaźnik na napis reprezentujący nazwę miasta;
 * @param[in] city2      – wskaźnik na napis reprezentujący nazwę miasta;
 * @param[in] length     – długość w km odcinka drogi;
 * @param[in] builtYear  – rok budowy odcinka drogi.
 * @return Wartość @p true, jeśli parametry są poprawne.
 * Wartość @p false, jeśli parametry są nie poprawne.
 */
static bool checkIfAddRoadParametersAreCorrect(const Map *map, const char *city1,
                                        const char *city2, unsigned length, int builtYear) {

    City *firstCity = NULL;
    City *secondCity = NULL;

    if (map == NULL) {
        return false;
    }

    if ((checkIfCityNameIsCorrect(city1) == false) || (checkIfCityNameIsCorrect(city2) == false)) {
        return false;
    }

    if (strcmp(city1, city2) == 0) {
        return false;
    }

    if (length <= 0) {
        return false;
    }

    if (builtYear == 0) {
        return false;
    }

    firstCity = findCityWithName(map->cities, city1);
    secondCity = findCityWithName(map->cities, city2);

    if (getRoadConnectingCities(firstCity, secondCity) != NULL) {
        return false;
    }

    return true;
}

/** @brief Sprawdza czy miasto o podanej nazwie istnieje wśród podanej listy miast.
 * @param[in] cityList - Lista miast wśród której szukane jest miasto.
 * @param[in] cityName - Nazwa szukanego miasta.
 * @return Wartość @p true, jeśli miasto istnieje wśród podanej listy miast.
 * Wartość @p false, jeśli miasto nie istnieje wśród podanej listy miast.
 */
static bool checkIfCityExists(const CityList *cityList, const char *cityName) {

    while (cityList != NULL) {

        if (strcmp(cityList->city->cityName, cityName) == 0) {
            return true;
        }

        cityList = cityList->next;
    }

    return false;
}

/** @brief Dodaję miasto o podanej nazwie do mapy.
 * @param[in,out] map - wskaźnik na mapę.
 * @param[in] cityName - nazwa miasta.
 * @return Wartość @p true, jeśli miasto zostało dodane.
 * Wartość @p false, jeśli nie udało się zaalokować pamięci.
 */
static bool addCity (Map *map, const char *cityName) {

    int numberOfCharsInCityName = 0;

    CityList *newCityListElement = (CityList*)malloc(sizeof(CityList));
    if (newCityListElement == NULL) {
        return false;
    }

    newCityListElement->next = map->cities;
    newCityListElement->city = (City*)malloc(sizeof(City));
    if (newCityListElement->city == NULL) {
        free(newCityListElement);
        return false;
    }

    while (cityName[numberOfCharsInCityName] != '\0') {
        numberOfCharsInCityName++;
    }

    numberOfCharsInCityName++;

    newCityListElement->city->cityName = (char*)malloc(sizeof(char) * numberOfCharsInCityName);
    if (newCityListElement->city->cityName == NULL) {
        free(newCityListElement->city);
        free(newCityListElement);
        return false;
    }

    newCityListElement->city->cityName = strcpy(newCityListElement->city->cityName, cityName);
    newCityListElement->city->wasCityGivenDistanceInRouteSearch = false;
    newCityListElement->city->distanceInRouteSearch = UINT_MAX;
    newCityListElement->city->roadsFromThisCity = NULL;

    map->cities = newCityListElement;

    return true;
}

/** @brief Dodaję miasta o podanych nazwach do mapy.
 * Jeżeli istnieje już na mapie miasto o podanej nazwie to go nie dodaje.
 * @param[in,out] map - Wskaźnik na mapę.
 * @param[in] city1 - Nazwa pierwszego miasta.
 * @param[in] city2 - Nazwa drugiego miasta.
 * @param[in] didCity1AlreadyExist - zmienna mówiąca czy miasto o pierwszej nazwie już istnieje na mapie.
 * @param[in] didCity2AlreadyExist - zmienna mówiąca czy miasto o drugiej nazwie już istnieje na mapie.
 * @return Wartość @p true, jeśli nie istniejące miasta zostały dodane.
 * Wartość @p false, jeśli nie udało się dodać nie istniejących miast.
 */
static bool addNotExistingCities(Map *map, const char *city1, const char *city2, bool didCity1AlreadyExist,
        bool didCity2AlreadyExist) {

    City *firstCity = NULL;

    if (didCity1AlreadyExist == false) {
        if (addCity(map, city1) == false) {
            return false;
        }
    }

    if (didCity2AlreadyExist == false) {
        if (addCity(map, city2) == false) {
            if (didCity1AlreadyExist == false) {
                firstCity = findCityWithName(map->cities, city1);
                deleteCityContainingNoRoadsFromMap(map, firstCity);
                return false;
            }
            else {
                return false;
            }
        }
    }

    return true;
}

/** @brief Tworzy nową drogę i zwraca wskaźnik na nią.
 * @param[in] firstCity - Wskaźnik na pierwsze miasto które łączy droga.
 * @param[in] secondCity - Wskaźnik na drugie miasto które łączy droga.
 * @param[in] length - Długość drogi.
 * @param[in] builtYear - Rok budowy drogi.
 * @return Wskaźnik na drogę, lub NULL gdy nie udało się zaalokować pamięci.
 */
static Road* createNewRoad(City *firstCity, City *secondCity, unsigned length, int builtYear) {

    Road *road = (Road*)malloc(sizeof(Road));
    if (road == NULL) {
        return NULL;
    }

    road->dateOfConstructionOrLastRepair = builtYear;
    road->length = length;
    road->RoutesContainingThisRoad = NULL;
    road->city1 = firstCity;
    if (road->city1 == NULL) {
        free(road);
        return NULL;
    }
    road->city2 = secondCity;
    if (road->city2 == NULL) {
        free(road);
        return NULL;
    }

    return road;
}

/** @brief Dodaję drogę do mapy.
 * @param[in,out] map - Wskaźnik na mapę.
 * @param[in] road - Wskaźnik na drogę.
 * @return Wartość @p true, jeśli udało się dodać drogę do mapy.
 * Wartość @p false, jeśli nie udało się zaalokować pamięci.
 */
static bool addRoadToRoadListOfMap(Map *map, Road *road) {

    RoadList *newRoadListElement = (RoadList*)malloc(sizeof(RoadList));
    if (newRoadListElement == NULL) {
        return false;
    }

    newRoadListElement->next = map->roads;
    newRoadListElement->road = road;

    map->roads = newRoadListElement;

    return true;
}

/** @brief Dodaję drogę do listy dróg wychodzących z miasta.
 * @param[in,out] city - Wskaźnik na miasto.
 * @param[in] road - Wskaźnik na drogę.
 * @return Wartość @p true, jeśli udało się dodać drogę do listy dróg wychodzących z miasta.
 * Wartość @p false, jeśli nie udało się zaalokować pamięci.
 */
static bool addRoadToRoadListOfCity(City *city, Road *road) {

    if (city == NULL) {
        return false;
    }

    RoadList *newRoadListElement = (RoadList*)malloc(sizeof(RoadList));
    if (newRoadListElement == NULL) {
        return false;
    }

    newRoadListElement->next = city->roadsFromThisCity;
    newRoadListElement->road = road;

    city->roadsFromThisCity = newRoadListElement;

    return true;
}

/** @brief Usuwa miasta dodane w funkcji @ref addRoad.
 * @param[in,out] map - Wskaźnik na mapę.
 * @param[in,out] firstCity - Wskaźnik na pierwsze miasto.
 * @param[in,out] secondCity - Wskaźnik na drugie miasto.
 * @param[in] wasCity1Added - Czy pierwsze miasto zostało dodane w tym wywołaniu funkcji @ref addRoad.
 * @param[in] wasCity2Added - Czy drugim miasto zostało dodane w tym wywołaniu funkcji @ref addRoad.
 */
static void deleteAddedCities(Map *map, City *firstCity, City *secondCity, bool wasCity1Added, bool wasCity2Added) {

    if (wasCity1Added == true) {
        deleteCityContainingNoRoadsFromMap(map, firstCity);
    }
    if (wasCity2Added == true) {
        deleteCityContainingNoRoadsFromMap(map, secondCity);
    }
}

/** @brief Usuwa podaną drogę z Listy dróg wychodzących z podanego miasta.
 * @param[in] road - Wskaźnik na drogę.
 * @param[in,out] city - Wskaźnik na miasto.
 */
static void deleteRoadFromCityRoadList(const Road *road, City *city) {

    RoadList *roadListElement = city->roadsFromThisCity;
    RoadList *previousRoadListElement = NULL;

    if (roadListElement != NULL) {
        if (roadListElement->road == road) {
            city->roadsFromThisCity = roadListElement->next;
            free(roadListElement);
            return;
        }
    }

    previousRoadListElement = roadListElement;
    roadListElement = roadListElement->next;

    while (roadListElement != NULL) {
        if (roadListElement->road == road) {
            previousRoadListElement->next = roadListElement->next;
            free(roadListElement);
            return;
        }

        previousRoadListElement = roadListElement;
        roadListElement = roadListElement->next;
    }
}

/** @brief Usuwa drogę nie naleźącą dożadnych dróg krajowych z mapy
 * @param[in,out] map - Wskaźnik na mapę.
 * @param[in,out] road  - Wskaźnik na drogę.
 */
static void deleteRoadContainingNoRoutesFromMap(Map *map, Road *road) {

    RoadList *roadListElement = map->roads;
    RoadList *previousRoadListElement = NULL;

    if (roadListElement != NULL) {
        if (roadListElement->road == road) {
            map->roads = roadListElement->next;
            free(road);
            free(roadListElement);
            return;
        }
    }

    previousRoadListElement = roadListElement;
    roadListElement = roadListElement->next;

    while (roadListElement != NULL) {
        if (roadListElement->road == road) {
            previousRoadListElement->next = roadListElement->next;
            free(road);
            free(roadListElement);
            return;
        }

        previousRoadListElement = roadListElement;
        roadListElement = roadListElement->next;
    }
}

/** @brief Dodaje do mapy odcinek drogi między dwoma różnymi miastami.
 * Jeśli któreś z podanych miast nie istnieje, to dodaje go do mapy, a następnie
 * dodaje do mapy odcinek drogi między tymi miastami.
 * @param[in,out] map    – wskaźnik na strukturę przechowującą mapę dróg;
 * @param[in] city1      – wskaźnik na napis reprezentujący nazwę miasta;
 * @param[in] city2      – wskaźnik na napis reprezentujący nazwę miasta;
 * @param[in] length     – długość w km odcinka drogi;
 * @param[in] builtYear  – rok budowy odcinka drogi.
 * @return Wartość @p true, jeśli odcinek drogi został dodany.
 * Wartość @p false, jeśli wystąpił błąd: któryś z parametrów ma niepoprawną
 * wartość, obie podane nazwy miast są identyczne, odcinek drogi między tymi
 * miastami już istnieje lub nie udało się zaalokować pamięci.
 */
bool addRoad(Map *map, const char *city1, const char *city2,
             unsigned length, int builtYear) {

    City *firstCity = NULL;
    City *secondCity = NULL;
    bool didCity1AlreadyExist;
    bool didCity2AlreadyExist;
    bool wasCity1Added;
    bool wasCity2Added;

    if (checkIfAddRoadParametersAreCorrect(map, city1, city2, length, builtYear) == false) {
        return false;
    }

    didCity1AlreadyExist = checkIfCityExists(map->cities, city1);
    didCity2AlreadyExist = checkIfCityExists(map->cities, city2);

    if (addNotExistingCities(map, city1, city2, didCity1AlreadyExist, didCity2AlreadyExist) == false) {
        return false;
    }

    wasCity1Added = !didCity1AlreadyExist;
    wasCity2Added = !didCity2AlreadyExist;

    firstCity = findCityWithName(map->cities, city1);
    secondCity = findCityWithName(map->cities, city2);

    Road *road = createNewRoad(firstCity, secondCity, length, builtYear);
    if (road == NULL) {
        return false;
    }

    if (addRoadToRoadListOfMap(map, road) == false) {
        free(road);
        deleteAddedCities(map, firstCity, secondCity, wasCity1Added, wasCity2Added);
        return false;
    }

    if (addRoadToRoadListOfCity(firstCity, road) == false) {

        deleteRoadContainingNoRoutesFromMap(map, road);
        deleteAddedCities(map, firstCity, secondCity, wasCity1Added, wasCity2Added);
        return false;
    }

    if (addRoadToRoadListOfCity(secondCity, road) == false) {

        deleteRoadFromCityRoadList(road, firstCity);
        deleteRoadContainingNoRoutesFromMap(map, road);
        deleteAddedCities(map, firstCity, secondCity, wasCity1Added, wasCity2Added);
        return false;
    }

    return true;
}

/** @brief Sprawdza poprawność parametrów funkcji @ref repairRoad.
 * @param[in] map - Wskaźnik na mapę.
 * @param[in] city1 - Nazwa pierwszego miasta.
 * @param[in] city2 - Nazwa drugiego miasta.
 * @param[in] repairYear - Rok remontu
 * @return Wartość @p true, jeśli parametry są poprawne.
 * Wartość @p false, jeśli parametry są nie poprawne.
 */
static bool checkIfRepairRoadParametersAreCorrect(Map *map, const char *city1, const char *city2, int repairYear) {

    Road *road = NULL;
    City *firstCity = NULL;
    City *secondCity = NULL;

    if (map == NULL) {
        return false;
    }

    if ((checkIfCityNameIsCorrect(city1) == false) || (checkIfCityNameIsCorrect(city2) == false)) {
        return false;
    }

    if (strcmp(city1, city2) == 0) {
        return false;
    }

    if (repairYear == 0) {
        return false;
    }

    if ((checkIfCityExists(map->cities, city1) == false) || (checkIfCityExists(map->cities, city2) == false)) {
        return false;
    }

    firstCity = findCityWithName(map->cities, city1);
    secondCity = findCityWithName(map->cities, city2);

    road = getRoadConnectingCities(firstCity, secondCity);
    if (road == NULL) {
        return false;
    }

    if (road->dateOfConstructionOrLastRepair > repairYear) {
        return false;
    }

    return true;
}

/** @brief Modyfikuje rok ostatniego remontu odcinka drogi.
 * Dla odcinka drogi między dwoma miastami zmienia rok jego ostatniego remontu
 * lub ustawia ten rok, jeśli odcinek nie był jeszcze remontowany.
 * @param[in,out] map    – wskaźnik na strukturę przechowującą mapę dróg;
 * @param[in] city1      – wskaźnik na napis reprezentujący nazwę miasta;
 * @param[in] city2      – wskaźnik na napis reprezentujący nazwę miasta;
 * @param[in] repairYear – rok ostatniego remontu odcinka drogi.
 * @return Wartość @p true, jeśli modyfikacja się powiodła.
 * Wartość @p false, jeśli wystąpił błąd: któryś z parametrów ma niepoprawną
 * wartość, któreś z podanych miast nie istnieje, nie ma odcinka drogi między
 * podanymi miastami, podany rok jest wcześniejszy niż zapisany dla tego odcinka
 * drogi rok budowy lub ostatniego remontu.
 */
bool repairRoad(Map *map, const char *city1, const char *city2, int repairYear) {

    Road *road = NULL;
    City *firstCity = NULL;
    City *secondCity = NULL;

    if (checkIfRepairRoadParametersAreCorrect(map, city1, city2, repairYear) == false) {
        return false;
    }

    firstCity = findCityWithName(map->cities, city1);
    secondCity = findCityWithName(map->cities, city2);

    road = getRoadConnectingCities(firstCity, secondCity);

    road->dateOfConstructionOrLastRepair = repairYear;

    return true;
}

/** @brief Sprawdza poprawność parametrów funkcji @ref newRoute.
 * @param[in] map - Wskaźnik na mapę.
 * @param[in] routeId - Numer identyfikacyjny drogi krajowej.
 * @param[in] city1 - Nazwa pierwszego miasta.
 * @param[in] city2 - Nazwa drugiego miasta.
 * @return Wartość @p true, jeśli parametry są poprawne.
 * Wartość @p false, jeśli parametry są nie poprawne.
 */
static bool checkIfNewRouteParametersAreCorrect(Map *map, unsigned routeId, const char *city1, const char *city2) {

    if (map == NULL) {
        return false;
    }

    if ((routeId < 1) || (routeId > MAX_ROUTE_ID)) {
        return false;
    }

    if ((checkIfCityNameIsCorrect(city1) == false) || (checkIfCityNameIsCorrect(city2) == false)) {
        return false;
    }

    if (strcmp(city1, city2) == 0) {
        return false;
    }

    if ((checkIfCityExists(map->cities, city1) == false) || (checkIfCityExists(map->cities, city2) == false)) {
        return false;
    }

    if (map->routeArray == NULL) {
        return false;
    }

    if (map->routeArray[routeId].doesRouteWithThisIdExist == true) {
        return false;
    }

    return true;
}

/** @brief Sprawdza czy podana droga krajowa zawiera podane miasto.
 * @param[in] route - Wskaźnik na drogę krajową.
 * @param[in] city - Wskaźnik na miasto.
 * @return Wartość @p true, jeśli droga krajowa zawiera podane miasto.
 * Wartość @p false, jeśli droga krajowa nie zawiera podanego miasta.
 */
static bool checkIfRouteContainsCity(const Route *route, const City *city) {

    City *temporaryCity = NULL;
    RoadList *roadListElement = NULL;

    if (route == NULL) {
        return false;
    }

    temporaryCity = route->firstCity;
    roadListElement = route->roadsOfThisRoute;

    if (temporaryCity == city) {
        return true;
    }

    while (roadListElement != NULL) {
        temporaryCity = getCityConnectedByRoad(temporaryCity, roadListElement->road);
        if (temporaryCity == city) {
            return true;
        }
        roadListElement = roadListElement->next;
    }
    return false;
}

/** @brief Funkcja aktualizuje odległości od pierwszego miasta z funkcji @ref assignDistancesOfRouteSearch
 * do miast połączonych drogą z podanym miastem \p city.
 *  Funkcja jest częścią algorytmu dijkstry realizowanego w funkcji @ref assignDistancesOfRouteSearch.
 *  jeśli miasto połączone drogą z podanym miastem nie mam przypisanej odległości to ją mu przypisuje
 *  i dodaje je do \p pointerToCitiesWithAssignedNotOptimalDistances.
 *  Miastom drogi krajowej \p routeWhichCitiesCannotBeUsed nie aktualizujemy odległości, chyba że są równe \p lastCity.
 *  Odległości nie aktualizujemy za pomocą drogi \p roadWhichCannotBeUsed.
 * @param[in] city - Wskaźnik na podane miasto.
 * @param[in,out] pointerToCitiesWithAssignedNotOptimalDistances - Lista miast które mają przypisaną odległość
 * ale nie koniecznie optymalną.
 * @param[in] routeWhichCitiesCannotBeUsed - Droga krajowa której miastom nie aktualizujemy odległości,
 * chyba że są równe \p lastCity. Jeśli == NULL parametr pomijany
 * @param[in] roadWhichCannotBeUsed - Droga za pomocą której nie aktualizujemy odległości.
 * Jeśli == NULL parametr pomijany.
 * @param[in, out] lastCity - ostatnie miasto z funkcji @ref assignDistancesOfRouteSearch.
 * @return Wartość @p true, jeśli udało się zaktualizować odległości.
 * Wartość @p false, jeśli nie udało się zaalokować pamięci.
 */
static bool updateDistancesOfCityNeighbours(const City *city, CityList **pointerToCitiesWithAssignedNotOptimalDistances,
        const Route *routeWhichCitiesCannotBeUsed, const Road *roadWhichCannotBeUsed, City *lastCity) {

    City *currentCity = NULL;
    RoadList *roadsFromCityElement = city->roadsFromThisCity;
    CityList *citiesWithAssignedNotOptimalDistances = *pointerToCitiesWithAssignedNotOptimalDistances;
    CityList *temporary = NULL;

    while (roadsFromCityElement != NULL) {
        currentCity = getCityConnectedByRoad(city, roadsFromCityElement->road);

        if (((checkIfRouteContainsCity(routeWhichCitiesCannotBeUsed, currentCity) == false) ||
        (currentCity == lastCity)) && (roadsFromCityElement->road != roadWhichCannotBeUsed)) {

            if (currentCity->wasCityGivenDistanceInRouteSearch == false) {

                currentCity->distanceInRouteSearch = city->distanceInRouteSearch + roadsFromCityElement->road->length;
                currentCity->wasCityGivenDistanceInRouteSearch = true;

                temporary = (CityList*)malloc(sizeof(CityList));
                if (temporary == NULL) {
                    return false;
                }

                temporary->next = citiesWithAssignedNotOptimalDistances;
                temporary->city = currentCity;
                citiesWithAssignedNotOptimalDistances = temporary;
            }
            else if (currentCity->distanceInRouteSearch >
            city->distanceInRouteSearch + roadsFromCityElement->road->length) {
                currentCity->distanceInRouteSearch = city->distanceInRouteSearch + roadsFromCityElement->road->length;
            }
        }

        roadsFromCityElement = roadsFromCityElement->next;
    }

    *pointerToCitiesWithAssignedNotOptimalDistances = citiesWithAssignedNotOptimalDistances;

    return true;
}

/** @brief Znajduje miasto z najniźszą przypisaną odległością spośród podanej listy miast.
 * @param[in] citiesWithAssignedNotOptimalDistances - Lista miast spośród której szukane jest miasto.
 * @return Wskaźnik na miasto, lub NULL gdy lista jest pusta.
 */
static City* findCityWithLowestDistance(const CityList *citiesWithAssignedNotOptimalDistances) {

    City *cityWithLowestDistance = NULL;

    if (citiesWithAssignedNotOptimalDistances == NULL) {
        return NULL;
    }
    else {
        cityWithLowestDistance = citiesWithAssignedNotOptimalDistances->city;
        citiesWithAssignedNotOptimalDistances = citiesWithAssignedNotOptimalDistances->next;
    }

    while (citiesWithAssignedNotOptimalDistances != NULL) {
        if (cityWithLowestDistance->distanceInRouteSearch >
        citiesWithAssignedNotOptimalDistances->city->distanceInRouteSearch) {
            cityWithLowestDistance = citiesWithAssignedNotOptimalDistances->city;
        }
        citiesWithAssignedNotOptimalDistances = citiesWithAssignedNotOptimalDistances->next;
    }

    return cityWithLowestDistance;
}

/** @brief Usuwa miasto z podanej listy miast, nie usuwa samego miasta.
 * @param[in,out] pointerToCityList - Wskaźnik na listę miast.
 * @param[in] city - miasto usuwane z listy.
 * @return Wartość @p true, jeśli miasto zostało usunięte.
 * Wartość @p false, jeśli nie udało się znaleźć miasta na liście.
 */
static bool deleteCityFromCityList(CityList **pointerToCityList, const City *city) {

    CityList *cityListElement = NULL;
    CityList *temporary = NULL;

    if ((pointerToCityList == NULL) || (*pointerToCityList == NULL)) {
        return false;
    }

    cityListElement = *pointerToCityList;
    temporary = cityListElement;

    if (cityListElement->city == city) {
        *pointerToCityList = cityListElement->next;
        free(temporary);
        return true;
    }

    cityListElement = cityListElement->next;

    while ((cityListElement != NULL) && (cityListElement->city != city)) {
        temporary = cityListElement;
        cityListElement = cityListElement->next;
    }

    if (cityListElement->city == city) {
        temporary->next = cityListElement->next;
        free(cityListElement);
        return true;
    }
    else {
        return false;
    }
}

/** @brief Usuwa listę miast, nie usuwa miast.
 * @param[in,out] pointerToCityList - Wskaźnik na listę miast.
 */
static void deleteCityList(CityList **pointerToCityList) {

    CityList *cityList = *pointerToCityList;
    CityList *temporary = NULL;

    while (cityList != NULL) {
        temporary = cityList;
        cityList = cityList->next;
        free(temporary);
    }

    *pointerToCityList = NULL;
}

/** @brief Usuwa odległości z miast naleźących do podanej listy
 * @param[in,out] cityList
 */
void deleteDistancesOfRouteSearch(CityList *cityList) {

    while (cityList != NULL) {
        cityList->city->wasCityGivenDistanceInRouteSearch = false;
        cityList = cityList->next;
    }
}

/** @brief Funkcja przypisuje odległości od \p firstCity miastu lastCity oraz miastom bliższym od niego do firstCity.
 *  Miastom drogi krajowej \p routeWhichCitiesCannotBeUsed nie przypisujemy odległości, chyba że są równe \p lastCity.
 *  Odległości nie aktualizujemy za pomocą drogi \p roadWhichCannotBeUsed.
 * Funkcja działa na zasadzie algorytmu dijkstry.
 * odległość = długość najkrótszej drogi krajowej
 * @param[in,out] map - Wskaźnik na mapę.
 * @param[in,out] firstCity - Miasto od którego mierzone są odległości.
 * @param[in,out] lastCity - Najdalsze miasto któremu jest przypisywana odległość.
 * @param[in] routeWhichCitiesCannotBeUsed - Droga krajowa której miastom nie aktualizujemy odległości,
 * chyba że są równe \p lastCity. Jeśli == NULL parametr pomijany
 * @param[in] roadWhichCannotBeUsed - Droga za pomocą której nie aktualizujemy odległości.
 * Jeśli == NULL parametr pomijany.
 * @return Wartość @p true, jeśli udało się przypisać odległości.
 * Wartość @p false, jeśli nie udało się zaalokować pamięci, lub nie udało się przypisać odległości.
 */
static bool assignDistancesOfRouteSearch(Map *map, City *firstCity, City *lastCity,
        const Route *routeWhichCitiesCannotBeUsed, const Road *roadWhichCannotBeUsed) {

    City *currentCity = firstCity;

    CityList *citiesWithAssignedNotOptimalDistances = (CityList*)malloc(sizeof(CityList));
    if (citiesWithAssignedNotOptimalDistances == NULL) {
        return false;
    }

    citiesWithAssignedNotOptimalDistances->next = NULL;
    citiesWithAssignedNotOptimalDistances->city = firstCity;

    firstCity->distanceInRouteSearch = 0;
    firstCity->wasCityGivenDistanceInRouteSearch = true;

    while ((currentCity != lastCity) && (currentCity != NULL)) {

        if (updateDistancesOfCityNeighbours(currentCity, &citiesWithAssignedNotOptimalDistances,
                routeWhichCitiesCannotBeUsed, roadWhichCannotBeUsed, lastCity) == false) {
            deleteCityList(&citiesWithAssignedNotOptimalDistances);
            deleteDistancesOfRouteSearch(map->cities);
            return false;
        }

        deleteCityFromCityList(&citiesWithAssignedNotOptimalDistances, currentCity);
        currentCity = findCityWithLowestDistance(citiesWithAssignedNotOptimalDistances);
    }
    deleteCityList(&citiesWithAssignedNotOptimalDistances);

    if (currentCity == NULL) {
        deleteDistancesOfRouteSearch(map->cities);
        return false;
    }

    return true;
}

/** @brief Dodaje drogę na początek listy dróg.
 * @param[in] road - Wskaźnik na drogę.
 * @param[in,out] pointerToRoadList - Wskaźnik na listę dróg.
 * @return Wartość @p true, jeśli udało się dodać drogę.
 * Wartość @p false, jeśli nie udało się zaalokować pamięci.
 */
static bool addRoadToTopOfRoadList(Road* road, RoadList **pointerToRoadList) {

    RoadList *roadList = *pointerToRoadList;
    RoadList *newRoadListElement = NULL;

    newRoadListElement = (RoadList*)malloc(sizeof(RoadList));
    if (newRoadListElement == NULL) {
        return false;
    }

    newRoadListElement->road = road;
    newRoadListElement->next = roadList;

    roadList = newRoadListElement;

    *pointerToRoadList = roadList;

    return true;
}

/** @brief Usuwa drogi z początku Listy dróg do momentu aż napotka na liście drogę wychodzącą z podanego miasta.
 * Tą drogę też usuwa z listy.
 *  Jeśli na liście dróg nie ma drogi wychodzącej z podanego miasta wywołanie funkcji zakończy się błędem.
 * @param[in,out] pointerToRoadList
 * @param[in] city
 */
static void deleteRoadsFromTopOfRoadListUntilCity(RoadList **pointerToRoadList, const City *city) {

    RoadList *roadList = *pointerToRoadList;
    RoadList *temporary = NULL;

    while ((roadList->road->city1 != city) && (roadList->road->city2 != city)) {
        temporary = roadList;
        roadList = roadList->next;
        free(temporary);
    }

    temporary = roadList;
    roadList = roadList->next;
    free(temporary);

    *pointerToRoadList = roadList;
}

/** @brief Podaje najstarszą datę ostatniego remontu drogi (lub budowy drogi jeśli nie była remontowana)
 * z dróg naleźących do drogi krajowej
 * @param[in] route
 * @return Najstarsza data, lub INT_MAX gdy route == NULL
 */
static int getOldestDateOfConstructionOrLastRepairOfRoute(const Route *route) {

    int oldestDateOfConstructionOrLastRepairOfRoute = INT_MAX;
    RoadList *roadList = NULL;

    if (route != NULL) {
        if (route->roadsOfThisRoute != NULL) {
            roadList = route->roadsOfThisRoute;

            while (roadList != NULL) {
                if (oldestDateOfConstructionOrLastRepairOfRoute > roadList->road->dateOfConstructionOrLastRepair) {
                    oldestDateOfConstructionOrLastRepairOfRoute = roadList->road->dateOfConstructionOrLastRepair;
                }
                roadList = roadList->next;
            }
        }
    }
    return oldestDateOfConstructionOrLastRepairOfRoute;
}

/** @brief Podaje sumę długości dróg drogi krajowej
 * @param[in] route
 * @return Długość lub 0 jeśli route == NULL.
 */
static int getLengthOfRoute(const Route *route) {

    int lengthOfRoute = 0;
    RoadList *roadList = NULL;

    if (route != NULL) {
        if (route->roadsOfThisRoute != NULL) {
            roadList = route->roadsOfThisRoute;

            while (roadList != NULL) {
                lengthOfRoute += roadList->road->length;
                roadList = roadList->next;
            }
        }
    }
    return lengthOfRoute;
}

/** @brief Usuwa drogę krajową.
 * @param pointerToRoute
 */
static void deleteRoute(Route **pointerToRoute) {

    Route *route = *pointerToRoute;

    if (route != NULL) {
        deleteRoadList(&route->roadsOfThisRoute);
        free(route);

        route = NULL;

        *pointerToRoute = route;
    }
}

/** @brief Kopiuje drogę krajową
 * @param[in] routeOriginal - Wskaźnik na kopiowaną drogę krajową
 * @param[out] pointerToRouteCopy - Wskaźnik na wskaźnik na kopię drogi krajowej stworzoną przez funkcję.
 * @return Wartość @p true, jeśli udało się skopiować drogę krajową.
 * Wartość @p false, jeśli nie udało się zaalokować pamięci.
 */
static bool copyRoute(const Route *routeOriginal , Route **pointerToRouteCopy) {

    Route *routeCopy = NULL;
    RoadList *roadListOfCopy = NULL;
    RoadList *temporary = NULL;
    RoadList *previous = NULL;
    RoadList *roadListOfOriginal = routeOriginal->roadsOfThisRoute;

    routeCopy = (Route*)malloc(sizeof(Route));
    if (routeCopy == NULL) {
        return false;
    }

    routeCopy->doesRouteWithThisIdExist = true;
    routeCopy->firstCity = routeOriginal->firstCity;
    routeCopy->lastCity = routeOriginal->lastCity;
    routeCopy->routeId = routeOriginal->routeId;
    routeCopy->roadsOfThisRoute = NULL;

    if (roadListOfOriginal != NULL) {
        roadListOfCopy = (RoadList*)malloc(sizeof(RoadList));
        if (roadListOfCopy == NULL) {
            free(routeCopy);
            return false;
        }
        roadListOfCopy->next = NULL;
        roadListOfCopy->road = roadListOfOriginal->road;
        previous = roadListOfCopy;
        roadListOfOriginal = roadListOfOriginal->next;
    }

    while (roadListOfOriginal != NULL) {
        temporary = (RoadList*)malloc(sizeof(RoadList));
        if (temporary == NULL) {
            deleteRoute(&routeCopy);
            return false;
        }
        temporary->next = NULL;
        temporary->road = roadListOfOriginal->road;
        previous->next = temporary;
        previous = previous->next;
        roadListOfOriginal = roadListOfOriginal->next;
    }

    routeCopy->roadsOfThisRoute = roadListOfCopy;

    *pointerToRouteCopy = routeCopy;

    return true;
}

/** @brief Usuwa Listę dróg krajowych razem z drogami krajowymi znajdującymi się na tej liście.
 * @param[in,out] pointerToRouteList
 */
static void deleteRouteList(RouteList **pointerToRouteList) {

    RouteList *routeList = *pointerToRouteList;
    RouteList *temporary = NULL;

    while (routeList != NULL) {
        temporary = routeList;
        routeList = routeList->next;
        deleteRoute(&(temporary->route));
        free(temporary);
    }

    *pointerToRouteList = routeList;
}

/** @brief Kopiuje listę dróg krajowych. Skopiowana lista zawiera kopie dróg krajowych z orginalnej listy.
 * @param[in] routeListOriginal - Wskaźnik na kopiowaną listę dróg krajowych.
 * @param[out] pointerToRouteListCopy - Wskaźnik na wskaźnik na kopię listy dróg krajowych stworzoną przez funkcję.
 * @return Wartość @p true, jeśli udało się skopiować listę dróg krajowych.
 * Wartość @p false, jeśli nie udało się zaalokować pamięci.
 */
static bool copyRouteList(const RouteList *routeListOriginal , RouteList **pointerToRouteListCopy) {

    RouteList *routeListCopy = NULL;
    RouteList *routeListCopyElement = NULL;
    RouteList *previousRouteListCopyElement = NULL;

    if (routeListOriginal == NULL) {
        *pointerToRouteListCopy = NULL;
        return true;
    }

    routeListCopy = (RouteList*)malloc(sizeof(RouteList));
    if (routeListCopy == NULL) {
        return false;
    }

    routeListCopy->next = NULL;
    routeListCopy->route = NULL;
    if (copyRoute(routeListOriginal->route, &(routeListCopy->route)) == false) {
        free(routeListCopy);
        return false;
    }
    previousRouteListCopyElement = routeListCopy;
    routeListOriginal = routeListOriginal->next;

    while (routeListOriginal != NULL) {

        routeListCopyElement = (RouteList*)malloc(sizeof(RouteList));
        if (routeListCopyElement == NULL) {
            deleteRouteList(&routeListCopy);
            return false;
        }

        previousRouteListCopyElement->next = routeListCopyElement;

        routeListCopyElement->next = NULL;
        routeListCopyElement->route = NULL;
        if (copyRoute(routeListOriginal->route, &(routeListCopyElement->route)) == false) {
            deleteRouteList(&routeListCopy);
            return false;
        }
        previousRouteListCopyElement = routeListCopyElement;
        routeListOriginal = routeListOriginal->next;
    }

    *pointerToRouteListCopy = routeListCopy;

    return true;
}

/** @brief Funkcja buduje optymalną drogę krajową.
 *  Funkcja nie zadziała jeśli miasta nie mają przypisanych odległości @ref assignDistancesOfRouteSearch.
 *  Wywołanie funkcji z currentRoute->roadsOfThisRoute = NULL
 *  spowoduje stworzenie optymalnej drogi krajowej od firstCity do currentCity
 *  i zapisanie jej do *pointerToBestRoute.
 * @param[in,out] currentRoute - Wskaźnik na obecną drogę krajową.
 * @param[in,out] currentCity - Wskaźnik na pierwsze miasto obecnej drogi krajowej.
 * @param[in] firstCity - Wskaźnik na pierwsze miasto końcowej drogi krajowej.
 * @param[in,out] pointerToBestRoute - Wskaźnik na wskaźnik na optymalną drogę krajową
 * @return Wartość @p true, jeśli udało się zbudować drogę krajową.
 * Wartość @p false, jeśli nie udało się zaalokować pamięci, lub istnieje więcej niż jedna optymalna droga krajowa.
 */
static bool buildBestRoute(Route *currentRoute, City *currentCity, const City *firstCity, Route **pointerToBestRoute) {

    RoadList *roadsFromCityElement = currentCity->roadsFromThisCity;
    City *neighbourCity = NULL;
    Road *road = NULL;
    Route *bestRoute = *pointerToBestRoute;
    int oldestDateOfConstructionOrLastRepairOfRoute;
    int oldestDateOfConstructionOrLastRepairOfTheBestRoute;

    /** Sprawdza czy currentRoute juź łączy docelowe miasta.
     *  Jeśli tak to sprawdza czy ma optymatny wiek.
     *  Jeśli tak to ją zapisuje do *pointerToBestRoute.
     */
    if (currentCity == firstCity) {
        oldestDateOfConstructionOrLastRepairOfRoute = getOldestDateOfConstructionOrLastRepairOfRoute(currentRoute);
        oldestDateOfConstructionOrLastRepairOfTheBestRoute = getOldestDateOfConstructionOrLastRepairOfRoute(bestRoute);

        if ((bestRoute == NULL) || (oldestDateOfConstructionOrLastRepairOfRoute >
        oldestDateOfConstructionOrLastRepairOfTheBestRoute)) {
            deleteRoute(&bestRoute);
            if (copyRoute(currentRoute, &bestRoute) == false) {
                return false;
            }
            *pointerToBestRoute = bestRoute;
            return true;
        }
        if (oldestDateOfConstructionOrLastRepairOfRoute == oldestDateOfConstructionOrLastRepairOfTheBestRoute) {
            return false;
        }
        *pointerToBestRoute = bestRoute;
        return true;
    }

    /** Dodaje do currentRoute takie drogi wychodzące z currentCity, że currentRoute ma możliwość takiego rozszerzenia,
     * że jak się ją rozszerzy to będzie łączyła docelowe miasta i będzie miała optymalną długośc.
     * Następnie wywołuję funkcję buildBestRoute dla tych rozszerzonych currentRoute.
     */
    while (roadsFromCityElement != NULL) {

        road = roadsFromCityElement->road;

        neighbourCity = getCityConnectedByRoad(currentCity, road);

        if ((neighbourCity->wasCityGivenDistanceInRouteSearch == true) &&
            (currentCity->distanceInRouteSearch == neighbourCity->distanceInRouteSearch + road->length)) {
            if (addRoadToTopOfRoadList(road, &(currentRoute->roadsOfThisRoute)) == false) {
                return false;
            }
            if (buildBestRoute(currentRoute, neighbourCity, firstCity, &bestRoute) == false) {
                *pointerToBestRoute = bestRoute;
                return false;
            }

            deleteRoadsFromTopOfRoadListUntilCity(&(currentRoute->roadsOfThisRoute), currentCity);
        }
        roadsFromCityElement = roadsFromCityElement->next;
    }
    *pointerToBestRoute = bestRoute;
    return true;
}

/** @brief Dodaje droge krajową do Listy dróg krajowych podanej drogi.
 * @param[in] route
 * @param[in,out] road
 * @return Wartość @p true, jeśli udało się dodać drogę krajową.
 * Wartość @p false, jeśli nie udało się zaalokować pamięci.
 */
static bool addRouteToRouteListOfRoad(Route *route, Road *road) {

    RouteList *temporary = NULL;

    if (road == NULL) {
        return false;
    }

    if (route == NULL) {
        return false;
    }

    temporary = road->RoutesContainingThisRoad;

    road->RoutesContainingThisRoad = (RouteList*)malloc(sizeof(RouteList));
    if (road->RoutesContainingThisRoad == NULL) {
        road->RoutesContainingThisRoad = temporary;
        return false;
    }
    road->RoutesContainingThisRoad->next = temporary;
    road->RoutesContainingThisRoad->route = route;

    return true;
}

/** @brief Usuwa droge krajową z Listy dróg krajowych podanej drogi.
 * @param[in] route
 * @param[in,out] road
 */
static void deleteRouteFromRouteListOfRoad(const Route *route, Road *road) {

    RouteList *temporary = NULL;
    RouteList *previous = NULL;

    if (road == NULL) {
        return;
    }

    if (route == NULL) {
        return;
    }

    temporary = road->RoutesContainingThisRoad;

    if (temporary == NULL) {
        return;
    }

    if (temporary->route == route) {
        road->RoutesContainingThisRoad = temporary->next;
        free(temporary);
        return;
    }

    previous = temporary;
    temporary = temporary->next;

    while ((temporary != NULL) && (temporary->route != route)) {
        previous = temporary;
        temporary = temporary->next;
    }

    if (temporary == NULL) {
        return;
    }

    previous->next = temporary->next;
    free(temporary);
}

/** @brief Usuwa drogę krajową route1 z list dróg krajowych należących do dróg należących do drogi krajowej route2.
 * @param[in] route1
 * @param[in,out] route2
 */
static void deleteRoute1FromRouteListsOfRoadsOfRoute2(const Route *route1, Route *route2) {

    RoadList *roadsOfRoute2 = NULL;
    Road *road = NULL;

    if (route2 == NULL) {
        return;
    }

    roadsOfRoute2 = route2->roadsOfThisRoute;

    while (roadsOfRoute2 != NULL) {
        road = roadsOfRoute2->road;

        deleteRouteFromRouteListOfRoad(route1, road);

        roadsOfRoute2 = roadsOfRoute2->next;
    }
}

/** @brief Dodaje drogę krajową route1 do list dróg krajowych należących do dróg należących do drogi krajowej route2.
 * @param[in] route1
 * @param[in,out] route2
 * @return Wartość @p true, jeśli udało się dodać drogę krajową.
 * Wartość @p false, jeśli nie udało się zaalokować pamięci.
 */
static bool addRoute1ToRouteListsOfRoadsOfRoute2(Route *route1, Route *route2) {

    RoadList *roadsOfRoute2 = NULL;
    Road *road = NULL;

    if (route2 == NULL) {
        return false;
    }

    roadsOfRoute2 = route2->roadsOfThisRoute;

    while (roadsOfRoute2 != NULL) {
        road = roadsOfRoute2->road;

        if (addRouteToRouteListOfRoad(route1, road) == false) {
            deleteRoute1FromRouteListsOfRoadsOfRoute2(route1, route2);
            return false;
        }

        roadsOfRoute2 = roadsOfRoute2->next;
    }
    return true;
}

/** @brief Łączy dwa różne miasta drogą krajową.
 * Tworzy drogę krajową pomiędzy dwoma miastami i nadaje jej podany numer.
 * Wśród istniejących odcinków dróg wyszukuje najkrótszą drogę. Jeśli jest
 * więcej niż jeden sposób takiego wyboru, to dla każdego wariantu wyznacza
 * wśród wybranych w nim odcinków dróg ten, który był najdawniej wybudowany lub
 * remontowany i wybiera wariant z odcinkiem, który jest najmłodszy.
 * @param[in,out] map    – wskaźnik na strukturę przechowującą mapę dróg;
 * @param[in] routeId    – numer drogi krajowej;
 * @param[in] city1      – wskaźnik na napis reprezentujący nazwę miasta;
 * @param[in] city2      – wskaźnik na napis reprezentujący nazwę miasta.
 * @return Wartość @p true, jeśli droga krajowa została utworzona.
 * Wartość @p false, jeśli wystąpił błąd: któryś z parametrów ma niepoprawną
 * wartość, istnieje już droga krajowa o podanym numerze, któreś z podanych
 * miast nie istnieje, obie podane nazwy miast są identyczne, nie można
 * jednoznacznie wyznaczyć drogi krajowej między podanymi miastami lub nie udało
 * się zaalokować pamięci.
 */
bool newRoute(Map *map, unsigned routeId, const char *city1, const char *city2) {

    Route *route = NULL;
    City *firstCity = NULL;
    City *lastCity = NULL;
    Route *bestRoute = NULL;

    if (checkIfNewRouteParametersAreCorrect(map, routeId, city1, city2) == false) {
        return false;
    }

    firstCity = findCityWithName(map->cities, city1);
    lastCity = findCityWithName(map->cities, city2);

    if (assignDistancesOfRouteSearch(map, firstCity, lastCity, NULL, NULL) == false) {
        return false;
    }

    route = (Route*)malloc(sizeof(Route));
    if (route == NULL) {
        deleteDistancesOfRouteSearch(map->cities);
        return false;
    }
    route->routeId = routeId;
    route->doesRouteWithThisIdExist = true;
    route->roadsOfThisRoute = NULL;
    route->firstCity = firstCity;
    route->lastCity = lastCity;

    if (buildBestRoute(route, lastCity, firstCity, &bestRoute) == false) {
        deleteDistancesOfRouteSearch(map->cities);
        deleteRoute(&route);
        deleteRoute(&bestRoute);
        return false;
    }

    deleteRoute(&route);
    deleteDistancesOfRouteSearch(map->cities);

    map->routeArray[routeId] = *bestRoute;
    free(bestRoute);

    if (addRoute1ToRouteListsOfRoadsOfRoute2(&(map->routeArray[routeId]), &(map->routeArray[routeId])) == false) {
        deleteRoadList(&(map->routeArray[routeId].roadsOfThisRoute));
        map->routeArray[routeId].doesRouteWithThisIdExist = false;
        return false;
    }

    return true;
}

/** @brief Sprawdza czy argumenty funkcji @ref extendRoute są poprawne.
 * @param[in] map
 * @param[in] routeId
 * @param[in] cityName
 * @return Wartość @p true, jeśli parametry są poprawne.
 * Wartość @p false, jeśli parametry nie są poprawne.
 */
static bool checkIfExtendRouteParametersAreCorrect(const Map *map, unsigned routeId, const char *cityName) {

    City *city = NULL;

    if (map == NULL) {
        return false;
    }

    if ((routeId < 1) || (routeId > MAX_ROUTE_ID)) {
        return false;
    }

    if (checkIfCityNameIsCorrect(cityName) == false) {
        return false;
    }

    if (checkIfCityExists(map->cities, cityName) == false) {
        return false;
    }

    if (map->routeArray == NULL) {
        return false;
    }

    if (map->routeArray[routeId].doesRouteWithThisIdExist == false) {
        return false;
    }

    city = findCityWithName(map->cities, cityName);

    if (checkIfRouteContainsCity(&(map->routeArray[routeId]), city) == true) {
        return false;
    }

    return true;
}

/** @brief Tworzy optymalne rozszerzenie drogi krajowej oldRoute zaczynające się w firstCity i kończace w lastCity
 * i zapisuje je do *pointerToBestRouteExtension.
 * @param[in,out] map
 * @param[in,out] firstCity
 * @param[in,out] lastCity
 * @param[out] pointerToBestRouteExtension
 * @param[in] oldRoute
 * @param[in] routeId
 * @return Wartość @p true, jeśli udało się stworzyć rozszerzenie.
 * Wartość @p false, jeśli nie udało się zaalokować pamięci,
 * lub nie da się jednoznacznie stworzyć optymalnego rozszerzenia.
 */
static bool buildRouteExtension(Map *map, City *firstCity, City *lastCity, Route **pointerToBestRouteExtension,
        const Route *oldRoute, unsigned routeId) {

    Route *routeExtension = NULL;
    Route *bestRouteExtension = NULL;

    if (assignDistancesOfRouteSearch(map, firstCity, lastCity, oldRoute, NULL) == false) {
        return false;
    }

    routeExtension = (Route*)malloc(sizeof(Route));
    if (routeExtension == NULL) {
        deleteDistancesOfRouteSearch(map->cities);
        return false;
    }
    routeExtension->routeId = routeId;
    routeExtension->doesRouteWithThisIdExist = false;
    routeExtension->roadsOfThisRoute = NULL;
    routeExtension->firstCity = firstCity;
    routeExtension->lastCity = lastCity;

    if (buildBestRoute(routeExtension, lastCity, firstCity, &bestRouteExtension) == false) {
        deleteDistancesOfRouteSearch(map->cities);
        deleteRoute(&routeExtension);
        deleteRoute(&bestRouteExtension);
        return false;
    }

    deleteRoute(&routeExtension);
    deleteDistancesOfRouteSearch(map->cities);

    *pointerToBestRouteExtension = bestRouteExtension;
    return true;
}

/** @brief Przedłuża drogę krajową *pointerToFirstRoute o drogę krajową *pointerToSecondRoute.
 * Przedłużona droga zaczyna się w pierwszym mieście *pointerToFirstRoute
 * i kończy w ostatnim mieście *pointerToSecondRoute.
 * Tą przedłużona drogę zapisuje do *pointerToNewRoute.
 * Następnie usuwa drogi krajowe *pointerToFirstRoute i *pointerToSecondRoute.
 * Funkcja działa poprawnie tylko gdy podane drogi krajowe się nie przecinają
 * i ostatnie miasto *pointerToFirstRoute to pierwsze miasto *pointerToSecondRoute.
 * @param[in,out] pointerToFirstRoute
 * @param[in,out] pointerToSecondRoute
 * @param[out] pointerToNewRoute
 */
static void connect2ndRouteTo1stRoute(Route **pointerToFirstRoute, Route **pointerToSecondRoute,
        Route **pointerToNewRoute) {

    Route *firstRoute = *pointerToFirstRoute;
    Route *secondRoute = *pointerToSecondRoute;
    Route *newRoute = NULL;
    RoadList *newRouteRoadListElement = NULL;

    newRoute = firstRoute;
    newRoute->lastCity = secondRoute->lastCity;
    newRouteRoadListElement = newRoute->roadsOfThisRoute;

    if (newRouteRoadListElement == NULL) {
        newRoute->roadsOfThisRoute = secondRoute->roadsOfThisRoute;
    }
    else {
        while (newRouteRoadListElement->next != NULL) {
            newRouteRoadListElement = newRouteRoadListElement->next;
        }

        newRouteRoadListElement->next = secondRoute->roadsOfThisRoute;
    }

    free(secondRoute);
    firstRoute = NULL;
    secondRoute = NULL;

    *pointerToNewRoute = newRoute;
    *pointerToFirstRoute = firstRoute;
    *pointerToSecondRoute = secondRoute;
}

/** @brief Funkcja będąca częścią @ref extendRoute.
 * Rozszerza drogę krajową *pointerToOldRoute o *pointerToRouteExtensionFromFirstCity
 * i zapisuje ją do * pointerToNewRoute.
 * Aktualizuje listy dróg krajowych w drogach.
 * Usuwa *pointerToRouteExtensionFromLastCity, *pointerToRouteExtensionFromFirstCity, *pointerToOldRoute.
 * @param[in,out] map - Wskaźnik na mapę.
 * @param[in] routeId - Numer drogi krajowej.
 * @param[in,out] pointerToOldRoute Wskaźnik na wskaźnik na drogę krajową przed rozszerzeniem.
 * @param[out] pointerToNewRoute Wskaźnik na wskaźnik na drogę krajową po rozszerzeniu.
 * @param[in,out] pointerToRouteExtensionFromFirstCity - Wskaźnik na wskaźnik na rozszerzenie *pointerToOldRoute
 * kończące się w piewrszym mieście *pointerToOldRoute.
 * @param[in,out] pointerToRouteExtensionFromLastCity - Wskaźnik na wskaźnik na rozszerzenie *pointerToOldRoute
 * zaczynające się w ostatnim mieście *pointerToOldRoute.
 * @return Wartość @p true, jeśli udało się rozszerzyć drogę krajową.
 * Wartość @p false, jeśli nie udało się zaalokować pamięci,
 * Jeśli funkcja zwraca false to zwalnia z pamięci odpowiednie struktury, zaalokowane w funkcji @ref extendRoute.
 */
static bool extendRouteFromFirstCity(Map *map, unsigned routeId, Route **pointerToOldRoute, Route **pointerToNewRoute,
                              Route **pointerToRouteExtensionFromFirstCity,
                              Route **pointerToRouteExtensionFromLastCity) {

    Route *oldRoute = *pointerToOldRoute;
    Route *newRoute = NULL;
    Route *routeExtensionFromFirstCity = *pointerToRouteExtensionFromFirstCity;
    Route *routeExtensionFromLastCity = *pointerToRouteExtensionFromLastCity;

    if (addRoute1ToRouteListsOfRoadsOfRoute2(&(map->routeArray[routeId]), routeExtensionFromFirstCity) == false) {
        deleteRoute(&routeExtensionFromFirstCity);
        deleteRoute(&routeExtensionFromLastCity);
        free(oldRoute);
        return false;
    }
    connect2ndRouteTo1stRoute(&routeExtensionFromFirstCity, &oldRoute, &newRoute);
    deleteRoute(&routeExtensionFromLastCity);

    *pointerToOldRoute = oldRoute;
    *pointerToNewRoute = newRoute;
    *pointerToRouteExtensionFromFirstCity = routeExtensionFromFirstCity;
    *pointerToRouteExtensionFromLastCity = routeExtensionFromLastCity;

    return true;
}

/** @brief Funkcja będąca częścią @ref extendRoute.
 * Rozszerza drogę krajową *pointerToOldRoute o *pointerToRouteExtensionFromLastCity
 * i zapisuje ją do * pointerToNewRoute.
 * Aktualizuje listy dróg krajowych w drogach.
 * Usuwa *pointerToRouteExtensionFromLastCity, *pointerToRouteExtensionFromFirstCity, *pointerToOldRoute.
 * @param[in,out] map - Wskaźnik na mapę.
 * @param[in] routeId - Numer drogi krajowej.
 * @param[in,out] pointerToOldRoute Wskaźnik na wskaźnik na drogę krajową przed rozszerzeniem.
 * @param[out] pointerToNewRoute Wskaźnik na wskaźnik na drogę krajową po rozszerzeniu.
 * @param[in,out] pointerToRouteExtensionFromFirstCity - Wskaźnik na wskaźnik na rozszerzenie *pointerToOldRoute
 * kończące się w piewrszym mieście *pointerToOldRoute.
 * @param[in,out] pointerToRouteExtensionFromLastCity - Wskaźnik na wskaźnik na rozszerzenie *pointerToOldRoute
 * zaczynające się w ostatnim mieście *pointerToOldRoute.
 * @return Wartość @p true, jeśli udało się rozszerzyć drogę krajową.
 * Wartość @p false, jeśli nie udało się zaalokować pamięci,
 * Jeśli funkcja zwraca false to zwalnia z pamięci odpowiednie struktury, zaalokowane w funkcji @ref extendRoute.
 */
static bool extendRouteFromLastCity(Map *map, unsigned routeId, Route **pointerToOldRoute, Route **pointerToNewRoute,
                             Route **pointerToRouteExtensionFromFirstCity,
                             Route **pointerToRouteExtensionFromLastCity) {

    Route *oldRoute = *pointerToOldRoute;
    Route *newRoute = NULL;
    Route *routeExtensionFromFirstCity = *pointerToRouteExtensionFromFirstCity;
    Route *routeExtensionFromLastCity = *pointerToRouteExtensionFromLastCity;

    if (addRoute1ToRouteListsOfRoadsOfRoute2(&(map->routeArray[routeId]), routeExtensionFromLastCity) == false) {
        deleteRoute(&routeExtensionFromFirstCity);
        deleteRoute(&routeExtensionFromLastCity);
        free(oldRoute);
        return false;
    }
    connect2ndRouteTo1stRoute(&oldRoute, &routeExtensionFromLastCity, &newRoute);
    deleteRoute(&routeExtensionFromFirstCity);

    *pointerToOldRoute = oldRoute;
    *pointerToNewRoute = newRoute;
    *pointerToRouteExtensionFromFirstCity = routeExtensionFromFirstCity;
    *pointerToRouteExtensionFromLastCity = routeExtensionFromLastCity;

    return true;
}

/** @brief Wydłuża drogę krajową do podanego miasta.
 * Dodaje do drogi krajowej nowe odcinki dróg do podanego miasta w taki sposób,
 * aby nowy fragment drogi krajowej był najkrótszy. Jeśli jest więcej niż jeden
 * sposób takiego wydłużenia, to dla każdego wariantu wyznacza wśród dodawanych
 * odcinków dróg ten, który był najdawniej wybudowany lub remontowany i wybiera
 * wariant z odcinkiem, który jest najmłodszy.
 * @param[in,out] map    – wskaźnik na strukturę przechowującą mapę dróg;
 * @param[in] routeId    – numer drogi krajowej;
 * @param[in] city       – wskaźnik na napis reprezentujący nazwę miasta.
 * @return Wartość @p true, jeśli droga krajowa została wydłużona.
 * Wartość @p false, jeśli wystąpił błąd: któryś z parametrów ma niepoprawną
 * nazwę, nie istnieje droga krajowa o podanym numerze, nie ma miasta o podanej
 * nazwie, przez podane miasto już przechodzi droga krajowa o podanym numerze,
 * podana droga krajowa kończy się w podanym mieście, nie można jednoznacznie
 * wyznaczyć nowego fragmentu drogi krajowej lub nie udało się zaalokować
 * pamięci.
 */
bool extendRoute(Map *map, unsigned routeId, const char *city) {

    Route *oldRoute = NULL;
    Route *newRoute = NULL;
    Route *bestRouteExtensionFromFirstCity = NULL;
    Route *bestRouteExtensionFromLastCity = NULL;
    City *cityRouteExtendsTo = NULL;
    City *firstCityOfOldRoute = NULL;
    City *lastCityOfOldRoute = NULL;
    int lengthOfRouteExtensionToFirstCity;
    int lengthOfRouteExtensionToLastCity;
    int oldestDateOfConstructionOrLastRepairOfRouteExtensionToFirstCity;
    int oldestDateOfConstructionOrLastRepairOfRouteExtensionToLastCity;

    if (checkIfExtendRouteParametersAreCorrect(map, routeId, city) == false) {
        return false;
    }

    oldRoute = (Route*)malloc(sizeof(Route));
    *oldRoute = map->routeArray[routeId];
    cityRouteExtendsTo = findCityWithName(map->cities, city);
    firstCityOfOldRoute = oldRoute->firstCity;
    lastCityOfOldRoute = oldRoute->lastCity;

    if (buildRouteExtension(map, cityRouteExtendsTo, firstCityOfOldRoute, &bestRouteExtensionFromFirstCity, oldRoute,
            routeId) == false) {
        if (buildRouteExtension(map, lastCityOfOldRoute, cityRouteExtendsTo, &bestRouteExtensionFromLastCity, oldRoute,
                routeId) == false) {
            free(oldRoute);
            return false;
        }
    } else {
        buildRouteExtension(map, lastCityOfOldRoute, cityRouteExtendsTo, &bestRouteExtensionFromLastCity, oldRoute,
                routeId);
    }

    if (bestRouteExtensionFromFirstCity == NULL) {
        if (extendRouteFromLastCity(map, routeId, &oldRoute, &newRoute, &bestRouteExtensionFromFirstCity,
                &bestRouteExtensionFromLastCity) == false) {
            return false;
        }
    }
    else if (bestRouteExtensionFromLastCity == NULL) {
        if (extendRouteFromFirstCity(map, routeId, &oldRoute, &newRoute, &bestRouteExtensionFromFirstCity,
                &bestRouteExtensionFromLastCity) == false) {
            return false;
        }
    }
    else {
        lengthOfRouteExtensionToFirstCity = getLengthOfRoute(bestRouteExtensionFromFirstCity);
        lengthOfRouteExtensionToLastCity = getLengthOfRoute(bestRouteExtensionFromLastCity);
        oldestDateOfConstructionOrLastRepairOfRouteExtensionToFirstCity =
                getOldestDateOfConstructionOrLastRepairOfRoute(bestRouteExtensionFromFirstCity);
        oldestDateOfConstructionOrLastRepairOfRouteExtensionToLastCity =
                getOldestDateOfConstructionOrLastRepairOfRoute(bestRouteExtensionFromLastCity);

        if (lengthOfRouteExtensionToFirstCity > lengthOfRouteExtensionToLastCity) {
            if (extendRouteFromLastCity(map, routeId, &oldRoute, &newRoute, &bestRouteExtensionFromFirstCity,
                    &bestRouteExtensionFromLastCity) == false) {
                return false;
            }
        }
        else if (lengthOfRouteExtensionToFirstCity < lengthOfRouteExtensionToLastCity) {
            if (extendRouteFromFirstCity(map, routeId, &oldRoute, &newRoute, &bestRouteExtensionFromFirstCity,
                    &bestRouteExtensionFromLastCity) == false) {
                return false;
            }
        }
        else if (oldestDateOfConstructionOrLastRepairOfRouteExtensionToFirstCity >
        oldestDateOfConstructionOrLastRepairOfRouteExtensionToLastCity) {
            if (extendRouteFromFirstCity(map, routeId, &oldRoute, &newRoute, &bestRouteExtensionFromFirstCity,
                    &bestRouteExtensionFromLastCity) == false) {
                return false;
            }
        }
        else if (oldestDateOfConstructionOrLastRepairOfRouteExtensionToFirstCity <
        oldestDateOfConstructionOrLastRepairOfRouteExtensionToLastCity) {
            if (extendRouteFromLastCity(map, routeId, &oldRoute, &newRoute, &bestRouteExtensionFromFirstCity,
                    &bestRouteExtensionFromLastCity) == false) {
                return false;
            }
        }
        else {
            deleteRoute(&bestRouteExtensionFromFirstCity);
            deleteRoute(&bestRouteExtensionFromLastCity);
            free(oldRoute);
            return false;
        }
    }

    map->routeArray[routeId] = *newRoute;
    free(oldRoute);
    free(newRoute);
    return true;
}

/** @brief Sprawdza czy parametry funkcji @ref removeRoad są poprawne.
 * @param[in] map
 * @param[in] city1
 * @param[in] city2
 * @return Wartość @p true, jeśli parametry są poprawne.
 * Wartość @p false, jeśli parametry nie są poprawne.
 */
static bool checkIfRemoveRoadParametersAreCorrect(const Map *map, const char *city1, const char *city2) {

    City *firstCity = NULL;
    City *secondCity = NULL;

    if (map == NULL) {
        return false;
    }

    if ((checkIfCityNameIsCorrect(city1) == false) || (checkIfCityNameIsCorrect(city2) == false)) {
        return false;
    }

    if (strcmp(city1, city2) == 0) {
        return false;
    }

    if (checkIfCityExists(map->cities, city1) == false) {
        return false;
    }

    if (checkIfCityExists(map->cities, city2) == false) {
        return false;
    }

    firstCity = findCityWithName(map->cities, city1);
    secondCity = findCityWithName(map->cities, city2);

    if (getRoadConnectingCities(firstCity, secondCity) == NULL) {
        return false;
    }

    return true;
}

/** @brief Usuwa drogę z drogi krajowej.
 * Uzupełnia drogę krajową, tak żeby była najkrótsza.
 * Aktualizuje listy dróg krajowych w drogach.
 * @param[in,out] map
 * @param[in] road
 * @param[in,out] route
 * @return Wartość @p true, jeśli udało się Usunąć drogę z drogi krajowej.
 * Wartość @p false, jeśli nie udało się zaalokować pamięci, lub nie da się uzupełnić drogi krajowej.
 */
static bool removeRoadFromRoute(Map *map, const Road *road, Route *route) {

    City *firstCityOfRoadInRoute = NULL;
    City *secondCityOfRoadInRoute = NULL;
    City *currentCity = NULL;
    RoadList *roadsOfRoute = NULL;
    RoadList *temporary = NULL;
    Route *routeConnectingCitiesOfTheRoad = NULL;
    Route *bestRouteConnectingCitiesOfTheRoad = NULL;

    roadsOfRoute = route->roadsOfThisRoute;
    currentCity = route->firstCity;

    while ((roadsOfRoute != NULL) && (roadsOfRoute->road != road)) {
        currentCity = getCityConnectedByRoad(currentCity, roadsOfRoute->road);
        roadsOfRoute = roadsOfRoute->next;
    }

    if (roadsOfRoute == NULL) {
        return false;
    }

    firstCityOfRoadInRoute = currentCity;
    secondCityOfRoadInRoute = getCityConnectedByRoad(currentCity, road);

    if (assignDistancesOfRouteSearch(map, firstCityOfRoadInRoute, secondCityOfRoadInRoute, route, road) == false) {
        return false;
    }

    routeConnectingCitiesOfTheRoad = (Route*)malloc(sizeof(Route));
    if (routeConnectingCitiesOfTheRoad == NULL) {
        deleteDistancesOfRouteSearch(map->cities);
        return false;
    }
    routeConnectingCitiesOfTheRoad->routeId = route->routeId;
    routeConnectingCitiesOfTheRoad->doesRouteWithThisIdExist = false;
    routeConnectingCitiesOfTheRoad->roadsOfThisRoute = NULL;
    routeConnectingCitiesOfTheRoad->firstCity = firstCityOfRoadInRoute;
    routeConnectingCitiesOfTheRoad->lastCity = secondCityOfRoadInRoute;

    if (buildBestRoute(routeConnectingCitiesOfTheRoad, secondCityOfRoadInRoute, firstCityOfRoadInRoute,
            &bestRouteConnectingCitiesOfTheRoad) == false) {
        deleteDistancesOfRouteSearch(map->cities);
        deleteRoute(&routeConnectingCitiesOfTheRoad);
        deleteRoute(&bestRouteConnectingCitiesOfTheRoad);
        return false;
    }

    deleteDistancesOfRouteSearch(map->cities);
    deleteRoute(&routeConnectingCitiesOfTheRoad);

    if (addRoute1ToRouteListsOfRoadsOfRoute2(&(map->routeArray[route->routeId]), bestRouteConnectingCitiesOfTheRoad) ==
    false) {
        deleteRoute(&bestRouteConnectingCitiesOfTheRoad);
        return false;
    }

    roadsOfRoute = route->roadsOfThisRoute;

    if (roadsOfRoute->road == road) {
        temporary = roadsOfRoute;
        route->roadsOfThisRoute = bestRouteConnectingCitiesOfTheRoad->roadsOfThisRoute;
        roadsOfRoute = bestRouteConnectingCitiesOfTheRoad->roadsOfThisRoute;
        while (roadsOfRoute->next != NULL) {
            roadsOfRoute = roadsOfRoute->next;
        }
        roadsOfRoute->next = temporary->next;
        free(temporary);
    }
    else {
        while (roadsOfRoute->next->road != road) {
            roadsOfRoute = roadsOfRoute->next;
        }
        temporary = roadsOfRoute->next;
        roadsOfRoute->next = bestRouteConnectingCitiesOfTheRoad->roadsOfThisRoute;
        roadsOfRoute = bestRouteConnectingCitiesOfTheRoad->roadsOfThisRoute;
        while (roadsOfRoute->next != NULL) {
            roadsOfRoute = roadsOfRoute->next;
        }
        roadsOfRoute->next = temporary->next;
        free(temporary);
    }

    free(bestRouteConnectingCitiesOfTheRoad);

    return true;
}

/** @brief Usuwa drogę z listy dróg krajowych.
 * Uzupełnia drogi krajowe, tak żeby była najkrótsze.
 * Aktualizuje listy dróg krajowych w drogach.
 * @param[in,out] map
 * @param[in] road
 * @param[in,out] routeList
 * @return Wartość @p true, jeśli udało się Usunąć drogę z listy dróg krajowych.
 * Wartość @p false, jeśli nie udało się zaalokować pamięci, lub nie da się uzupełnić drogi krajowej.
 * Jeśli funkcja zwraca false, to nie przywraca pierwotnej postaci routeList.
 */
static bool removeRoadFromRouteList(Map *map, Road *road, RouteList *routeList) {

    while (routeList != NULL) {
        if (removeRoadFromRoute(map, road, routeList->route) == false) {
            return false;
        }
        routeList = routeList->next;
    }
    return true;
}

/** @brief Usuwa odcinek drogi między dwoma różnymi miastami.
 * Usuwa odcinek drogi między dwoma miastami. Jeśli usunięcie tego odcinka drogi
 * powoduje przerwanie ciągu jakiejś drogi krajowej, to uzupełnia ją
 * istniejącymi odcinkami dróg w taki sposób, aby była najkrótsza. Jeśli jest
 * więcej niż jeden sposób takiego uzupełnienia, to dla każdego wariantu
 * wyznacza wśród dodawanych odcinków drogi ten, który był najdawniej wybudowany
 * lub remontowany i wybiera wariant z odcinkiem, który jest najmłodszy.
 * @param[in,out] map    – wskaźnik na strukturę przechowującą mapę dróg;
 * @param[in] city1      – wskaźnik na napis reprezentujący nazwę miasta;
 * @param[in] city2      – wskaźnik na napis reprezentujący nazwę miasta.
 * @return Wartość @p true, jeśli odcinek drogi został usunięty.
 * Wartość @p false, jeśli z powodu błędu nie można usunąć tego odcinka drogi:
 * któryś z parametrów ma niepoprawną wartość, nie ma któregoś z podanych miast,
 * nie istnieje droga między podanymi miastami, nie da się jednoznacznie
 * uzupełnić przerwanego ciągu drogi krajowej lub nie udało się zaalokować
 * pamięci.
 */
bool removeRoad(Map *map, const char *city1, const char *city2) {

    Road *road = NULL;
    City *firstCity = NULL;
    City *secondCity = NULL;
    RouteList *newRoutesOfRemovedRoad = NULL;
    RouteList *routesOfRemovedRoadElement = NULL;
    RouteList *temporary = NULL;

    if (checkIfRemoveRoadParametersAreCorrect(map, city1, city2) == false) {
        return false;
    }

    firstCity = findCityWithName(map->cities, city1);
    secondCity = findCityWithName(map->cities, city2);

    road = getRoadConnectingCities(firstCity, secondCity);

    if (copyRouteList(road->RoutesContainingThisRoad, &newRoutesOfRemovedRoad) == false) {
        return false;
    }

    if (removeRoadFromRouteList(map, road, newRoutesOfRemovedRoad) == false) {
        deleteRouteList(&newRoutesOfRemovedRoad);
        return false;
    }

    routesOfRemovedRoadElement = road->RoutesContainingThisRoad;

    while (routesOfRemovedRoadElement != NULL) {
        deleteRoadList(&routesOfRemovedRoadElement->route->roadsOfThisRoute);
        routesOfRemovedRoadElement->route->roadsOfThisRoute = newRoutesOfRemovedRoad->route->roadsOfThisRoute;

        temporary = routesOfRemovedRoadElement;
        routesOfRemovedRoadElement = routesOfRemovedRoadElement->next;
        free(temporary);
        temporary = newRoutesOfRemovedRoad;
        newRoutesOfRemovedRoad = newRoutesOfRemovedRoad->next;
        free(temporary->route);
        free(temporary);
    }

    deleteRoadFromCityRoadList(road, firstCity);
    deleteRoadFromCityRoadList(road, secondCity);

    deleteRoadContainingNoRoutesFromMap(map, road);

    return true;
}

/** @brief Sprawdza czy parametry funkcji @ref getRouteDescription są poprawne.
 * @param[in] map
 * @param[in] routeId
 * @return Wartość @p true, jeśli parametry są poprawne.
 * Wartość @p false, jeśli parametry nie są poprawne.
 */
static bool checkIfGetRouteDescriptionParametersAreCorrect(const Map *map, unsigned routeId) {

    if (map == NULL) {
        return false;
    }

    if ((routeId < 1) || (routeId > MAX_ROUTE_ID)) {
        return false;
    }

    if (map->routeArray == NULL) {
        return false;
    }

    if (map->routeArray[routeId].doesRouteWithThisIdExist == false) {
        return false;
    }

    return true;
}

/** @brief Zwraca liczbę charów potrzebną dla napisu generowanego przez funkcję @ref getRouteDescription.
 * @param[in] route
 * @param[in] routeId
 * @return liczba charów
 */
static int getNumberOfCharsOfRouteDescription(const Route *route, unsigned routeId) {

    int numberOfCharsOfRouteDescription = 0;
    RoadList *roadsOfRoute = NULL;
    City *city = NULL;

    roadsOfRoute = route->roadsOfThisRoute;
    city = route->firstCity;

    numberOfCharsOfRouteDescription += (getNumberOfCharsInStringRepresentationOfUnsigned(routeId) - 1);

    ///char dla ';'
    numberOfCharsOfRouteDescription++;

    numberOfCharsOfRouteDescription += (getNumberOfCharsInString(city->cityName) - 1);

    while (roadsOfRoute != NULL) {
        ///char dla ';'
        numberOfCharsOfRouteDescription++;

        numberOfCharsOfRouteDescription +=
                (getNumberOfCharsInStringRepresentationOfUnsigned(roadsOfRoute->road->length) - 1);

        ///char dla ';'
        numberOfCharsOfRouteDescription++;

        numberOfCharsOfRouteDescription +=
                (getNumberOfCharsInStringRepresentationOfInt(roadsOfRoute->road->dateOfConstructionOrLastRepair) - 1);

        ///char dla ';'
        numberOfCharsOfRouteDescription++;

        city = getCityConnectedByRoad(city, roadsOfRoute->road);

        numberOfCharsOfRouteDescription += (getNumberOfCharsInString(city->cityName) - 1);

        roadsOfRoute = roadsOfRoute->next;
    }

    /// char dla '\0'
    numberOfCharsOfRouteDescription++;

    return numberOfCharsOfRouteDescription;
}

/** @brief Udostępnia informacje o drodze krajowej.
 * Zwraca wskaźnik na napis, który zawiera informacje o drodze krajowej. Alokuje
 * pamięć na ten napis. Zwraca pusty napis, jeśli nie istnieje droga krajowa
 * o podanym numerze. Zaalokowaną pamięć trzeba zwolnić za pomocą funkcji free.
 * Informacje wypisywane są w formacie:
 * numer drogi krajowej;nazwa miasta;długość odcinka drogi;rok budowy lub
 * ostatniego remontu;nazwa miasta;długość odcinka drogi;rok budowy lub
 * ostatniego remontu;nazwa miasta;…;nazwa miasta.
 * Kolejność miast na liście jest taka, aby miasta @p city1 i @p city2, podane
 * w wywołaniu funkcji @ref newRoute, które utworzyło tę drogę krajową, zostały
 * wypisane w tej kolejności.
 * @param[in,out] map    – wskaźnik na strukturę przechowującą mapę dróg;
 * @param[in] routeId    – numer drogi krajowej.
 * @return Wskaźnik na napis lub NULL, gdy nie udało się zaalokować pamięci.
 */
char const* getRouteDescription(Map *map, unsigned routeId) {

    char *routeDescription = NULL;
    int numberOfCharsOfRouteDescription;
    Route *route = NULL;
    int indexOfRouteDescription = 0;
    RoadList *roadsOfRoute = NULL;
    City *city = NULL;
    const char *temporaryString = NULL;

    if (checkIfGetRouteDescriptionParametersAreCorrect(map, routeId) == false) {
        routeDescription = (char*)malloc(sizeof(char));
        if (routeDescription == NULL) {
            return NULL;
        }

        routeDescription[0] = '\0';

        return routeDescription;
    }

    route = &(map->routeArray[routeId]);

    numberOfCharsOfRouteDescription = getNumberOfCharsOfRouteDescription(route, routeId);
    routeDescription = (char*)malloc(sizeof(char) * numberOfCharsOfRouteDescription);
    if (routeDescription == NULL) {
        return NULL;
    }


    roadsOfRoute = route->roadsOfThisRoute;
    city = route->firstCity;

    temporaryString = getStringRepresentationOfUnsigned(routeId);
    if (addSubstringToStringStartingOnIndexedPosition(temporaryString, routeDescription, &indexOfRouteDescription,
            numberOfCharsOfRouteDescription) == false) {
        free(routeDescription);
        free((char*)temporaryString);
        return NULL;
    }
    free((char*)temporaryString);

    routeDescription[indexOfRouteDescription] = ';';
    indexOfRouteDescription++;

    temporaryString = city->cityName;
    if (addSubstringToStringStartingOnIndexedPosition(temporaryString, routeDescription, &indexOfRouteDescription,
            numberOfCharsOfRouteDescription) == false) {
        free(routeDescription);
        return NULL;
    }

    while (roadsOfRoute != NULL) {
        routeDescription[indexOfRouteDescription] = ';';
        indexOfRouteDescription++;

        temporaryString = getStringRepresentationOfUnsigned(roadsOfRoute->road->length);
        if (addSubstringToStringStartingOnIndexedPosition(temporaryString, routeDescription, &indexOfRouteDescription,
                numberOfCharsOfRouteDescription) == false) {
            free(routeDescription);
            free((char*)temporaryString);
            return NULL;
        }
        free((char*)temporaryString);

        routeDescription[indexOfRouteDescription] = ';';
        indexOfRouteDescription++;

        temporaryString = getStringRepresentationOfInt(roadsOfRoute->road->dateOfConstructionOrLastRepair);
        if (addSubstringToStringStartingOnIndexedPosition(temporaryString, routeDescription, &indexOfRouteDescription,
                numberOfCharsOfRouteDescription) == false) {
            free(routeDescription);
            free((char*)temporaryString);
            return NULL;
        }
        free((char*)temporaryString);

        routeDescription[indexOfRouteDescription] = ';';
        indexOfRouteDescription++;

        city = getCityConnectedByRoad(city, roadsOfRoute->road);
        temporaryString = city->cityName;
        if (addSubstringToStringStartingOnIndexedPosition(temporaryString, routeDescription, &indexOfRouteDescription,
                numberOfCharsOfRouteDescription) == false) {
            free(routeDescription);
            return NULL;
        }

        roadsOfRoute = roadsOfRoute->next;
    }

    routeDescription[indexOfRouteDescription] = '\0';

    return routeDescription;
}

/** @brief Sprawdza czy parametry funkcji @ref removeRoute są poprawne.
 * @param[in] map
 * @param[in] routeId
 * @return Wartość @p true, jeśli parametry są poprawne.
 * Wartość @p false, jeśli parametry nie są poprawne.
 */
static bool checkIfRemoveRouteParametersAreCorrect(const Map *map, unsigned routeId) {

    if (map == NULL) {
        return false;
    }

    if ((routeId < 1) || (routeId > MAX_ROUTE_ID)) {
        return false;
    }

    if (map->routeArray == NULL) {
        return false;
    }

    if (map->routeArray[routeId].doesRouteWithThisIdExist == false) {
        return false;
    }

    return true;
}

/** @brief Usuwa z mapy dróg drogę krajową o podanym numerze.
 * Jeśli podana droga krajowa nie istnieje lub podany numer jest niepoprawny,
 * niczego nie zmienia w mapie dróg, dając wynik false.
 * Nie usuwa odcinków dróg ani miast.
 * @param[in,out] map
 * @param[in] routeId
 * @return Wartość @p true, jeśli droga krajowa została usunięta.
 * Wartość @p false, jeśli któryś z parametrów ma niepoprawną wartość,
 * lub nie istnieje droga krajowa o podanym numerze.
 */
bool removeRoute(Map *map, unsigned routeId) {

    Route *route = NULL;

    if (checkIfRemoveRouteParametersAreCorrect(map, routeId) == false) {
        return false;
    }

    route = &map->routeArray[routeId];

    deleteRoute1FromRouteListsOfRoadsOfRoute2(route, route);

    deleteRoadList(&route->roadsOfThisRoute);

    route->doesRouteWithThisIdExist = false;

    return true;
}


/** @brief Sprawdza czy nazwa miasta w poleceniu z funkcji @ref newCustomRoute się powtarza.
 * polecenie musi być w odpowiednim formacie gwarantowanym przez @ref checkIfLineIsCorrectNewCustomRouteCommand.
 * @param[in] parametersOfLine - Tablica parametrów polecenia wczytanego za pomocą @ref readLine.
 * @param[in] numberOfParametersInLine - liczba parametrów w poleceniu.
 * @return Wartość @p true, jeśli nazwa miasta się powtarza.
 * Wartość @p false, jeśli nazwa miasta się nie powtarza.
 */
static bool checkIfCityNameRepeats(const char *const*parametersOfLine, int numberOfParametersInLine) {

    int indexOfCityName1 = 1;
    int indexOfCityName2;

    while (indexOfCityName1 < numberOfParametersInLine) {
        indexOfCityName2 = indexOfCityName1 + 3;

        while (indexOfCityName2 < numberOfParametersInLine) {
            if (strcmp(parametersOfLine[indexOfCityName2], parametersOfLine[indexOfCityName1]) == 0) {
                return true;
            }
            indexOfCityName2 += 3;
        }
        indexOfCityName1 += 3;
    }

    return false;
}

/** @brief Sprawdza czy parametry funkcji @ref newCustomRoute są poprawne.
 * @param[in] map
 * @param[in] parametersOfLine - Tablica parametrów polecenia wczytanego za pomocą @ref readLine.
 * @param[in] numberOfParametersInLine - liczba parametrów w poleceniu.
 * @return Wartość @p true, jeśli parametry są poprawne.
 * Wartość @p false, jeśli parametry nie są poprawne.
 */
static bool checkIfLineIsCorrectNewCustomRouteCommand(const Map *map, const char *const*parametersOfLine,
        int numberOfParametersInLine) {

    int index = 0;
    unsigned routeId;
    unsigned roadLength;
    int dateOfConstructionOrLastRepairOfRoad;
    City *currentCity = NULL;
    City *previousCity = NULL;
    Road *currentRoad = NULL;

    if (map == NULL) {
        return false;
    }

    if ((numberOfParametersInLine%3 != 2) || (numberOfParametersInLine < 5)) {
        return false;
    }

    if (checkIfStringCanBeRepresentedAsUnsigned(parametersOfLine[index]) == false) {
        return false;
    }
    routeId = getUnsignedRepresentationOfString(parametersOfLine[index]);
    index++;

    if ((routeId < 1) || (routeId > MAX_ROUTE_ID)) {
        return false;
    }

    if (map->routeArray == NULL) {
        return false;
    }

    if (map->routeArray[routeId].doesRouteWithThisIdExist == true) {
        return false;
    }

    if (checkIfCityNameIsCorrect(parametersOfLine[index]) == false) {
        return false;
    }
    currentCity = findCityWithName(map->cities, parametersOfLine[index]);
    index++;

    while (index < numberOfParametersInLine) {

        if (checkIfStringCanBeRepresentedAsUnsigned(parametersOfLine[index]) == false) {
            return false;
        }
        roadLength = getUnsignedRepresentationOfString(parametersOfLine[index]);
        if (roadLength <= 0) {
            return false;
        }
        index++;

        if (checkIfStringCanBeRepresentedAsInt(parametersOfLine[index]) == false) {
            return false;
        }
        dateOfConstructionOrLastRepairOfRoad = getIntRepresentationOfString(parametersOfLine[index]);
        if (dateOfConstructionOrLastRepairOfRoad == 0) {
            return false;
        }
        index++;

        if (checkIfCityNameIsCorrect(parametersOfLine[index]) == false) {
            return false;
        }
        previousCity = currentCity;
        currentCity = findCityWithName(map->cities, parametersOfLine[index]);
        index++;

        currentRoad = getRoadConnectingCities(currentCity, previousCity);
        if (currentRoad != NULL) {
            if (currentRoad->length != roadLength) {
                return false;
            }

            if (currentRoad->dateOfConstructionOrLastRepair > dateOfConstructionOrLastRepairOfRoad) {
                return false;
            }
        }
    }

    if (checkIfCityNameRepeats(parametersOfLine, numberOfParametersInLine) == true) {
        return false;
    }

    return true;
}

/** @brief Tworzy nową drogę krajową na podstawie polecenia.
 * Format polecenia powinien być taki sam, jak w wyniku funkcji getRouteDescription.
 * To polecenie tworzy drogę krajową o podanym numerze i przebiegu.
 * Jeśli jakieś miasto lub odcinek drogi nie istnieje, to go tworzy.
 * Jeśli odcinek drogi już istnieje, ale ma wcześniejszy rok budowy lub ostatniego remontu,
 * to modyfikuje ten atrybut odcinka drogi.
 * Za błąd uznajemy, jeśli odcinek drogi już istnieje,
 * ale ma inną długość albo późniejszy rok budowy lub ostatniego remontu.
 * @param[in,out] map
 * @param[in] parametersOfLine - Tablica parametrów polecenia wczytanego za pomocą @ref readLine.
 * @param[in] numberOfParametersInLine - liczba parametrów w poleceniu.
 * @return Wartość @p true, jeśli droga krajowa została utworzona.
 * Wartość @p false, jeśli wystąpił błąd: któryś z parametrów ma niepoprawną
 * wartość, istnieje już droga krajowa o podanym numerze, lub nie udało
 * się zaalokować pamięci.
 */
bool newCustomRoute(Map *map, const char *const*parametersOfLine, int numberOfParametersInLine) {

    int index = 0;
    unsigned routeId;
    unsigned roadLength;
    int dateOfConstructionOrLastRepairOfRoad;
    City *currentCity = NULL;
    City *previousCity = NULL;
    Road *currentRoad = NULL;
    RoadList *roadsOfRoute = NULL;
    RoadList *currentRoadsOfRouteElement = NULL;
    RoadList *previousRoadsOfRouteElement = NULL;
    City *firstCity = NULL;
    City *lastCity = NULL;

    if (checkIfLineIsCorrectNewCustomRouteCommand(map, parametersOfLine, numberOfParametersInLine) == false) {
        return false;
    }

    routeId = getUnsignedRepresentationOfString(parametersOfLine[index]);
    index++;

    currentCity = findCityWithName(map->cities, parametersOfLine[index]);
    if (currentCity == NULL) {
        if (addCity(map, parametersOfLine[index]) == false) {
            return false;
        }
        currentCity = findCityWithName(map->cities, parametersOfLine[index]);
    }
    firstCity = currentCity;

    index++;

    while (index < numberOfParametersInLine) {
        roadLength = getUnsignedRepresentationOfString(parametersOfLine[index]);

        index++;
        dateOfConstructionOrLastRepairOfRoad = getIntRepresentationOfString(parametersOfLine[index]);

        index++;
        previousCity = currentCity;
        currentCity = findCityWithName(map->cities, parametersOfLine[index]);
        if (currentCity == NULL) {
            if (addCity(map, parametersOfLine[index]) == false) {
                deleteRoadList(&roadsOfRoute);
                return false;
            }
            currentCity = findCityWithName(map->cities, parametersOfLine[index]);
            if (addRoad(map, parametersOfLine[index - 3], parametersOfLine[index], roadLength,
                    dateOfConstructionOrLastRepairOfRoad) == false) {
                deleteRoadList(&roadsOfRoute);
                return false;
            }
            currentRoad = getRoadConnectingCities(currentCity, previousCity);
        }
        else {
            currentRoad = getRoadConnectingCities(currentCity, previousCity);
            if (currentRoad == NULL) {
                if (addRoad(map, parametersOfLine[index - 3], parametersOfLine[index], roadLength,
                        dateOfConstructionOrLastRepairOfRoad) == false) {
                    deleteRoadList(&roadsOfRoute);
                    return false;
                }
                currentRoad = getRoadConnectingCities(currentCity, previousCity);
            }
            else {
                currentRoad->dateOfConstructionOrLastRepair = dateOfConstructionOrLastRepairOfRoad;
            }
        }
        lastCity = currentCity;

        previousRoadsOfRouteElement = currentRoadsOfRouteElement;
        currentRoadsOfRouteElement = (RoadList*)malloc(sizeof(RoadList));
        if (currentRoadsOfRouteElement == NULL) {
            deleteRoadList(&roadsOfRoute);
            return false;
        }
        currentRoadsOfRouteElement->next = NULL;
        currentRoadsOfRouteElement->road = currentRoad;

        if (previousRoadsOfRouteElement == NULL) {
            roadsOfRoute = currentRoadsOfRouteElement;
        }
        else {
            previousRoadsOfRouteElement->next = currentRoadsOfRouteElement;
        }

        index++;
    }

    map->routeArray[routeId].doesRouteWithThisIdExist = true;
    map->routeArray[routeId].roadsOfThisRoute = roadsOfRoute;
    map->routeArray[routeId].routeId = routeId;
    map->routeArray[routeId].firstCity = firstCity;
    map->routeArray[routeId].lastCity = lastCity;

    return true;
}