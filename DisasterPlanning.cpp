#include "DisasterPlanning.h"
using namespace std;
#include <Map>
#include <Set>
#include <Optional>

Optional<Set<string>> placeEmergencySupplies(const Map<string, Set<string>>& roadNetwork, int numCities) {
    if (numCities < 0) {
        error("Number of cities cannot be negative.");
    }

    Set<string> coveredCities;
    Set<string> allCities = roadNetwork.keys();
    return placeEmergencySuppliesHelper(roadNetwork, numCities, coveredCities, allCities);
}

Optional<Set<string>> placeEmergencySuppliesHelper(const Map<string, Set<string>>& roadNetwork,
                                                   int numCities,
                                                   Set<string> coveredCities,
                                                   const Set<string>& remainingCities) {
    // Check if all cities are covered
    if (remainingCities.isEmpty()) {
        return Set<string>(); // Return empty set as we have covered all
    }

    // Pick an uncovered city
    string uncoveredCity = *remainingCities.first();

    // Try covering this city by placing supplies in it or in its neighbors
    Set<string> neighbors = roadNetwork[uncoveredCity];

    // Option 1: Place supplies in the uncovered city itself
    if (numCities > 0) {
        coveredCities.add(uncoveredCity);
        Set<string> newRemainingCities = remainingCities - Set<string>{uncoveredCity};
        for (const string& neighbor : neighbors) {
            coveredCities.add(neighbor);
        }

        Optional<Set<string>> result = placeEmergencySuppliesHelper(roadNetwork, numCities - 1, coveredCities, newRemainingCities);
        if (result != Nothing) {
            result.value().add(uncoveredCity); // Include the city we just covered
            return result; // Successful coverage
        }

        // Backtrack
        coveredCities.remove(uncoveredCity);
        for (const string& neighbor : neighbors) {
            coveredCities.remove(neighbor);
        }
    }

    // Option 2: Place supplies in one of the neighbors
    for (const string& neighbor : neighbors) {
        if (numCities > 0 && !coveredCities.contains(neighbor)) {
            coveredCities.add(neighbor);
            Set<string> newRemainingCities = remainingCities - Set<string>{uncoveredCity};
            for (const string& city : roadNetwork[neighbor]) {
                coveredCities.add(city);
            }

            Optional<Set<string>> result = placeEmergencySuppliesHelper(roadNetwork, numCities - 1, coveredCities, newRemainingCities);
            if (result != Nothing) {
                result.value().add(neighbor); // Include the neighbor we just covered
                return result; // Successful coverage
            }

            // Backtrack
            coveredCities.remove(neighbor);
            for (const string& city : roadNetwork[neighbor]) {
                coveredCities.remove(city);
            }
        }
    }

    return Nothing; // No successful coverage found
}
    (void) roadNetwork;
    (void) numCities;
    return Nothing;
}


/* * * * * * * Test Helper Functions Below This Point * * * * * */
#include "GUI/SimpleTest.h"

/* This is a helper function that's useful for designing test cases. You give it a Map
 * of cities and what they're adjacent to, and it then produces a new Map where if city
 * A links to city B, then city B links back to city A. We recommend using this helper
 * function when writing tests, though you won't need it in your implementation of the main
 * canBeMadeDisasterReady function.
 */
Map<string, Set<string>> makeSymmetric(const Map<string, Set<string>>& source) {
    Map<string, Set<string>> result = source;

    for (const string& from: source) {
        for (const string& to: source[from]) {
            result[from] += to;
            result[to] += from;
        }
    }

    return result;
}

/* This helper function tests whether a city has been covered by a set of supply locations
 * and is used by our testing code. You're welcome to use it in your tests as well!
 */
bool isCovered(const string& city,
               const Map<string, Set<string>>& roadNetwork,
               const Set<string>& supplyLocations) {
    if (supplyLocations.contains(city)) return true;

    for (string neighbor: roadNetwork[city]) {
        if (supplyLocations.contains(neighbor)) return true;
    }

    return false;
}

/* * * * * * Test Cases Below This Point * * * * * */

STUDENT_TEST("Test placeEmergencySupplies with simple network") {
    Map<string, Set<string>> roadNetwork;
    roadNetwork["A"] = {"B", "C"};
    roadNetwork["B"] = {"A", "D"};
    roadNetwork["C"] = {"A", "D"};
    roadNetwork["D"] = {"B", "C", "E"};
    roadNetwork["E"] = {"D"};

    Optional<Set<string>> result = placeEmergencySupplies(roadNetwork, 2);
    assert(result != Nothing);
    // Check that the result includes the necessary cities for coverage
    assert(result.value().contains("D")); // Expected to cover all cities
}














/* * * * * Provided Tests Below This Point * * * * */

PROVIDED_TEST("Reports an error if numCities < 0") {
    EXPECT_ERROR(placeEmergencySupplies({ }, -137));
}

PROVIDED_TEST("Works for map with no cities.") {
    /* Should be able to find the solution whether there's 0 allowed cities,
     * 137 allowed cities, etc.
     */
    EXPECT_EQUAL(placeEmergencySupplies({ }, 0), {});
    EXPECT_EQUAL(placeEmergencySupplies({ }, 137), {});
}

PROVIDED_TEST("Works for map with one city.") {
    Map<string, Set<string>> map = makeSymmetric({
         { "Solipsist", {} }
    });

    /* Shouldn't matter how many cities we use, as long as it isn't zero! */
    EXPECT_EQUAL    (placeEmergencySupplies(map, 0), Nothing);
    EXPECT_NOT_EQUAL(placeEmergencySupplies(map, 1), Nothing);
    EXPECT_NOT_EQUAL(placeEmergencySupplies(map, 2), Nothing);
}

PROVIDED_TEST("Works for map with one city, and produces output.") {
    Map<string, Set<string>> map = makeSymmetric({
         { "Solipsist", {} }
    });

    EXPECT_EQUAL(placeEmergencySupplies(map, 0), Nothing);
    EXPECT_EQUAL(placeEmergencySupplies(map, 1), {"Solipsist"});
    EXPECT_EQUAL(placeEmergencySupplies(map, 2), {"Solipsist"});
}

PROVIDED_TEST("Works for map with two linked cities.") {
    Map<string, Set<string>> map = makeSymmetric({
         { "A", { "B" } },
         { "B", {     } }
    });

    EXPECT_EQUAL    (placeEmergencySupplies(map, 0), Nothing);
    EXPECT_NOT_EQUAL(placeEmergencySupplies(map, 1), Nothing);
    EXPECT_NOT_EQUAL(placeEmergencySupplies(map, 2), Nothing);
}

PROVIDED_TEST("Works for map with two linked cities, and produces output.") {
    Map<string, Set<string>> map = makeSymmetric({
         { "A", { "B" } },
    });

    EXPECT_EQUAL(placeEmergencySupplies(map, 0), Nothing);

    Optional<Set<string>> locations1 = placeEmergencySupplies(map, 1);
    EXPECT_NOT_EQUAL(locations1, Nothing);
    EXPECT_EQUAL(locations1.value().size(), 1);
    EXPECT(locations1.value().isSubsetOf({ "A", "B" }));

    Optional<Set<string>> locations2 = placeEmergencySupplies(map, 2);
    EXPECT_NOT_EQUAL(locations2, Nothing);
    EXPECT(locations2.value().size() <= 2);
    EXPECT(locations2.value().isSubsetOf({ "A", "B" }));
}

PROVIDED_TEST("Works for four disconnected cities.") {
    Map<string, Set<string>> map = makeSymmetric({
        { "A", { } },
        { "B", { } },
        { "C", { } },
        { "D", { } }
    });

    EXPECT_EQUAL(placeEmergencySupplies(map, 0), Nothing);
    EXPECT_EQUAL(placeEmergencySupplies(map, 1), Nothing);
    EXPECT_EQUAL(placeEmergencySupplies(map, 2), Nothing);
    EXPECT_EQUAL(placeEmergencySupplies(map, 3), Nothing);
    EXPECT_NOT_EQUAL(placeEmergencySupplies(map, 4), Nothing);
}

PROVIDED_TEST("Works for four disconnected cities, and produces output.") {
    Map<string, Set<string>> map = makeSymmetric({
        { "A", { } },
        { "B", { } },
        { "C", { } },
        { "D", { } }
    });

    EXPECT_EQUAL(placeEmergencySupplies(map, 0), Nothing);
    EXPECT_EQUAL(placeEmergencySupplies(map, 1), Nothing);
    EXPECT_EQUAL(placeEmergencySupplies(map, 2), Nothing);
    EXPECT_EQUAL(placeEmergencySupplies(map, 3), Nothing);
    EXPECT_EQUAL(placeEmergencySupplies(map, 4), {"A", "B", "C", "D"});
}

PROVIDED_TEST("Can solve ethene example, regardless of ordering.") {
    /* Because Map and Set internally store items in sorted order, the order
     * in which you iterate over the cities when making decisions is sensitive
     * to the order of those cities' names. This test looks at a map like
     * this one, trying out all possible orderings of the city names:
     *
     *             *
     *             |
     *        * -- * -- * -- *
     *                  |
     *                  *
     *
     * There are 6! = 720 possible permutations of the ordering of these six
     * cities. If your code is able to solve the problem correctly for all of
     * those orderings, there's a good chance that you're correctly
     * covering and uncovering cities at each step. On the other hand, if
     * your code runs into issues here, it may indicate that the way in which
     * you're covering and uncovering cities accidentally uncovers a city that
     * you have previously covered.
     */
    Vector<string> cities = { "A", "B", "C", "D", "E", "F" };
    do {
        /* Ethene arrangement:
         *
         *        0
         *       1234
         *         5
         */
        Map<string, Set<string>> map = makeSymmetric({
            { cities[2], { cities[0], cities[1], cities[3] } },
            { cities[3], { cities[4], cities[5] } }
        });

        /* We should be able to cover everything with two cities:
         * city 2 and city 3.
         */
        EXPECT_NOT_EQUAL(placeEmergencySupplies(map, 2), Nothing);

        /* We should not be able to cover everything with one city. */
        EXPECT_EQUAL(placeEmergencySupplies(map, 1), Nothing);
    } while (next_permutation(cities.begin(), cities.end()));
}

PROVIDED_TEST("Can solve ethene example, regardless of ordering, and produces output.") {
    /* Because Map and Set internally store items in sorted order, the order
     * in which you iterate over the cities when making decisions is sensitive
     * to the order of those cities' names. This test looks at a map like
     * this one, trying out all possible orderings of the city names:
     *
     *             *
     *             |
     *        * -- * -- * -- *
     *                  |
     *                  *
     *
     * There are 6! = 720 possible permutations of the ordering of these six
     * cities. If your code is able to solve the problem correctly for all of
     * those orderings, there's a good chance that you're correctly
     * covering and uncovering cities at each step. On the other hand, if
     * your code runs into issues here, it may indicate that the way in which
     * you're covering and uncovering cities accidentally uncovers a city that
     * you have previously covered.
     */
    Vector<string> cities = { "A", "B", "C", "D", "E", "F" };
    do {
        /* Ethene arrangement:
         *
         *        0
         *       1234
         *         5
         */
        Map<string, Set<string>> map = makeSymmetric({
            { cities[2], { cities[0], cities[1], cities[3] } },
            { cities[3], { cities[4], cities[5] } }
        });

        /* We should be able to cover everything with two cities:
         * city 2 and city 3.
         */
        EXPECT_EQUAL(placeEmergencySupplies(map, 2), { cities[2], cities[3] });

        /* We should not be able to cover everything with one city. */
        EXPECT_EQUAL(placeEmergencySupplies(map, 1), Nothing);
    } while (next_permutation(cities.begin(), cities.end()));
}

PROVIDED_TEST("Works for six cities in a line, regardless of order.") {
    /* Because Map and Set internally store items in sorted order, the order
     * in which you iterate over the cities when making decisions is sensitive
     * to the order of those cities' names. This test looks at a map like
     * this one, trying out all possible orderings of the city names:
     *
     *        * -- * -- * -- * -- * -- *
     *
     * There are 6! = 720 possible permutations of the ordering of these six
     * cities in a line. If your code is able to solve the problem correctly
     * for all of those orderings, there's a good chance that you're correctly
     * covering and uncovering cities at each step. On the other hand, if
     * your code runs into issues here, it may indicate that the way in which
     * you're covering and uncovering cities accidentally uncovers a city that
     * you have previously covered.
     */
    Vector<string> cities = { "A", "B", "C", "D", "E", "F" };
    do {
        /* Linear arrangement. */
        Map<string, Set<string>> map;
        for (int i = 1; i + 1 < cities.size(); i++) {
            map[cities[i]] = { cities[i - 1], cities[i + 1] };
        }

        map = makeSymmetric(map);

        /* We should be able to cover everything with two cities, specifically,
         * the cities one spot in from the two sides.
         */
        EXPECT_NOT_EQUAL(placeEmergencySupplies(map, 2), Nothing);

        /* We should not be able to cover everything with one city. */
        EXPECT_EQUAL(placeEmergencySupplies(map, 1), Nothing);
    } while (next_permutation(cities.begin(), cities.end()));
}

PROVIDED_TEST("Works for six cities in a line, regardless of order, and produces output.") {
    /* Because Map and Set internally store items in sorted order, the order
     * in which you iterate over the cities when making decisions is sensitive
     * to the order of those cities' names. This test looks at a map like
     * this one, trying out all possible orderings of the city names:
     *
     *        * -- * -- * -- * -- * -- *
     *
     * There are 6! = 720 possible permutations of the ordering of these six
     * cities in a line. If your code is able to solve the problem correctly
     * for all of those orderings, there's a good chance that you're correctly
     * covering and uncovering cities at each step. On the other hand, if
     * your code runs into issues here, it may indicate that the way in which
     * you're covering and uncovering cities accidentally uncovers a city that
     * you have previously covered.
     */
    Vector<string> cities = { "A", "B", "C", "D", "E", "F" };
    do {
        /* Linear arrangement. */
        Map<string, Set<string>> map;
        for (int i = 1; i + 1 < cities.size(); i++) {
            map[cities[i]] = { cities[i - 1], cities[i + 1] };
        }

        map = makeSymmetric(map);

        /* We should be able to cover everything with two cities, specifically,
         * the cities one spot in from the two sides.
         */
        Optional<Set<string>> chosen = placeEmergencySupplies(map, 2);
        EXPECT_NOT_EQUAL(chosen, Nothing);

        EXPECT_EQUAL(chosen.value().size(), 2);
        EXPECT(chosen.value().contains(cities[1]));
        EXPECT(chosen.value().contains(cities[4]));

        /* We should not be able to cover everything with one city. */
        EXPECT_EQUAL(placeEmergencySupplies(map, 1), Nothing);
    } while (next_permutation(cities.begin(), cities.end()));
}

/* The "Don't Be Greedy" sample world. */
const Map<string, Set<string>> kDontBeGreedy = makeSymmetric({
    { "A", { "B" } },
    { "B", { "C", "D" } },
    { "C", { "D" } },
    { "D", { "F", "G" } },
    { "E", { "F" } },
    { "F", { "G" } },
});

PROVIDED_TEST("Solves \"Don't be Greedy\" from the handout.") {
    EXPECT_EQUAL(placeEmergencySupplies(kDontBeGreedy, 0), Nothing);
    EXPECT_EQUAL(placeEmergencySupplies(kDontBeGreedy, 1), Nothing);
    EXPECT_NOT_EQUAL(placeEmergencySupplies(kDontBeGreedy, 2), Nothing);
}

PROVIDED_TEST("Solves \"Don't be Greedy\" from the handout, and produces output.") {
    EXPECT_EQUAL(placeEmergencySupplies(kDontBeGreedy, 0), Nothing);
    EXPECT_EQUAL(placeEmergencySupplies(kDontBeGreedy, 1), Nothing);
    EXPECT_EQUAL(placeEmergencySupplies(kDontBeGreedy, 2), {"B", "F"});
}

PROVIDED_TEST("Solves \"Don't be Greedy,\" regardless of ordering, and produces output.") {
    /* Because Map and Set internally store items in sorted order, the order
     * in which you iterate over the cities when making decisions is sensitive
     * to the order of those cities' names. This test looks at a map like
     * this one, trying out all possible orderings of the city names:
     *
     *     0       4
     *     |       |
     *     1 - 2 - 3
     *      \ / \ /
     *       5   6
     *
     * There are 7! = 5,040 possible permutations of the ordering of these seven
     * cities. If your code is able to solve the problem correctly for all of
     * those orderings, there's a good chance that you're correctly
     * covering and uncovering cities at each step. On the other hand, if
     * your code runs into issues here, it may indicate that the way in which
     * you're covering and uncovering cities accidentally uncovers a city that
     * you have previously covered.
     */
    Vector<string> cities = { "A", "B", "C", "D", "E", "F", "G" };
    do {
        Map<string, Set<string>> map = makeSymmetric({
            { cities[1], { cities[0], cities[2], cities[5] } },
            { cities[2], { cities[3], cities[5], cities[6] } },
            { cities[3], { cities[4], cities[6] } },
        });

        /* We should be able to cover everything with two cities. */
        EXPECT_EQUAL(placeEmergencySupplies(map, 2), { cities[1], cities[3] });
        EXPECT_EQUAL(placeEmergencySupplies(map, 1), Nothing);
    } while (next_permutation(cities.begin(), cities.end()));
}

PROVIDED_TEST("Stress test: 6 x 6 grid.") {
    Map<string, Set<string>> grid;

    /* Build the grid. */
    char maxRow = 'F';
    int  maxCol = 6;
    for (char row = 'A'; row <= maxRow; row++) {
        for (int col = 1; col <= maxCol; col++) {
            if (row != maxRow) {
                grid[row + to_string(col)] += (char(row + 1) + to_string(col));
            }
            if (col != maxCol) {
                grid[row + to_string(col)] += (char(row) + to_string(col + 1));
            }
        }
    }
    grid = makeSymmetric(grid);


    /* 10x factor of safety relative to my middle-of-the-line computer. */
    EXPECT_COMPLETES_IN(20.0,
        EXPECT_NOT_EQUAL(placeEmergencySupplies(grid, 10), Nothing);
    );
}

PROVIDED_TEST("Stress test: 6 x 6 grid, with output.") {
    Optional<Set<string>> locations;
    char maxRow = 'F';
    int  maxCol = 6;

    Map<string, Set<string>> grid;

    /* Build the grid. */
    for (char row = 'A'; row <= maxRow; row++) {
        for (int col = 1; col <= maxCol; col++) {
            if (row != maxRow) {
                grid[row + to_string(col)] += (char(row + 1) + to_string(col));
            }
            if (col != maxCol) {
                grid[row + to_string(col)] += (char(row) + to_string(col + 1));
            }
        }
    }
    grid = makeSymmetric(grid);

    /* 10x factor of safety relative to my middle-of-the-line computer. */
    EXPECT_COMPLETES_IN(20.0,
        locations = placeEmergencySupplies(grid, 10);
    );
    EXPECT_NOT_EQUAL(locations, Nothing);
    EXPECT_LESS_THAN_OR_EQUAL_TO(locations.value().size(), 10);

    for (char row = 'A'; row <= maxRow; row++) {
        for (int col = 1; col <= maxCol; col++) {
            EXPECT(isCovered(row + to_string(col), grid, locations.value()));
        }
    }
}

