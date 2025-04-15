#include "WinSumLoseSum.h"
using namespace std;

#include <Set>
#include <Optional>

Optional<Set<int>> makeTarget(const Set<int>& elems, int target) {
    return makeTarget(elems, target, 0, Set<int>());
}

Optional<Set<int>> makeTarget(const Set<int>& elems, int target, int index, Set<int> currentSet) {
    // Base cases
    if (target == 0) {
        return currentSet; // Found a valid subset
    }
    if (index >= elems.size() || target < 0) {
        return Nothing; // No valid subset found
    }

    // Recursive case
    int elem = elems[index];

    // 1. Include the current element
    currentSet.add(elem);
    auto includeResult = makeTarget(elems, target - elem, index + 1, currentSet);
    if (includeResult != Nothing) {
        return includeResult; // Found solution including the current element
    }

    // 2. Exclude the current element
    currentSet.remove(elem); // You might not need this because of the new set passed
    return makeTarget(elems, target, index + 1, currentSet); // Check without including the current element
}
    (void) elems;
    (void) target;
    return Nothing;
}

/* * * * * Test Cases Below This Point * * * * */
#include "GUI/SimpleTest.h"

STUDENT_TEST("Test empty set") {
    Set<int> elems;
    Optional<Set<int>> result = makeTarget(elems, 0);
    // Expect to find an empty set which sums to 0
    assert(result != Nothing && result.value().isEmpty());
}

STUDENT_TEST("Test simple case") {
    Set<int> elems = {3, 1, 2, 5};
    Optional<Set<int>> result = makeTarget(elems, 5);
    // Expect to find a subset {5}
    assert(result != Nothing);
}



/* * * * * Provided Tests Below This Point * * * * */
PROVIDED_TEST("Works for an empty set of numbers.") {
    /* Can make 0, but not others. */
    EXPECT_EQUAL(makeTarget({ },  0), {});
    EXPECT_EQUAL(makeTarget({ },  1), Nothing);
    EXPECT_EQUAL(makeTarget({ }, -1), Nothing);
}

PROVIDED_TEST("Works for a one-element (singleton) set.") {
    /* Can make 0 and 137, but not others. */
    EXPECT_EQUAL(makeTarget({ 137 }, 0),   {});
    EXPECT_EQUAL(makeTarget({ 137 }, 137), {137});
    EXPECT_EQUAL(makeTarget({ 137 }, 1),   Nothing);
    EXPECT_EQUAL(makeTarget({ 137 }, -1),  Nothing);
}

PROVIDED_TEST("Works for a two-element (doubleton) set.") {
    EXPECT_EQUAL(makeTarget({ 1, 2 }, -1), Nothing);
    EXPECT_EQUAL(makeTarget({ 1, 2 }, 0), {});
    EXPECT_EQUAL(makeTarget({ 1, 2 }, 1), {1});
    EXPECT_EQUAL(makeTarget({ 1, 2 }, 2), {2});
    EXPECT_EQUAL(makeTarget({ 1, 2 }, 3), {1, 2});
    EXPECT_EQUAL(makeTarget({ 1, 2 }, 4), Nothing);
}

PROVIDED_TEST("Works for a three-element set.") {

    /* Can make 0, 1, 3, 4, 5, 7, and 8, but not others. */
    EXPECT_EQUAL(makeTarget({ 1, 3, 4 }, -1), Nothing);
    EXPECT_EQUAL(makeTarget({ 1, 3, 4 },  0), {});
    EXPECT_EQUAL(makeTarget({ 1, 3, 4 },  1), {1});
    EXPECT_EQUAL(makeTarget({ 1, 3, 4 },  2), Nothing);
    EXPECT_EQUAL(makeTarget({ 1, 3, 4 },  3), {3});
    EXPECT_NOT_EQUAL(makeTarget({ 1, 3, 4 }, 4), Nothing);
    EXPECT_EQUAL(makeTarget({ 1, 3, 4 },  5), {1, 4});
    EXPECT_EQUAL(makeTarget({ 1, 3, 4 },  6), Nothing);
    EXPECT_EQUAL(makeTarget({ 1, 3, 4 },  7), {3, 4});
    EXPECT_EQUAL(makeTarget({ 1, 3, 4 },  8), {1, 3, 4});
    EXPECT_EQUAL(makeTarget({ 1, 3, 4 },  9), Nothing);
}
