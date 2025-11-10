#define BOOST_TEST_MODULE MarkerTests
#include <boost/test/included/unit_test.hpp>
#include <sstream>
#include "../include/marker_app.hpp"

BOOST_AUTO_TEST_SUITE(SetZerosTests)

BOOST_AUTO_TEST_CASE(set_zeros_removes_matching_thread_index) {
    int arr[5] = {1, 2, 2, 3, 2};
    set_zeros(arr, 5, 2);

    BOOST_CHECK_EQUAL(arr[0], 1);
    BOOST_CHECK_EQUAL(arr[1], 0);
    BOOST_CHECK_EQUAL(arr[2], 0);
    BOOST_CHECK_EQUAL(arr[3], 3);
    BOOST_CHECK_EQUAL(arr[4], 0);
}

BOOST_AUTO_TEST_CASE(set_zeros_no_matching_elements) {
    int arr[4] = {1, 1, 1, 1};
    set_zeros(arr, 4, 2);

    for (int i = 0; i < 4; ++i) {
        BOOST_CHECK_EQUAL(arr[i], 1);
    }
}

BOOST_AUTO_TEST_CASE(set_zeros_empty_array) {
    int arr[1] = {5};
    set_zeros(arr, 0, 5);
    BOOST_CHECK_EQUAL(arr[0], 5);
}

BOOST_AUTO_TEST_CASE(set_zeros_all_elements_match) {
    int arr[3] = {4, 4, 4};
    set_zeros(arr, 3, 4);

    for (int i = 0; i < 3; ++i) {
        BOOST_CHECK_EQUAL(arr[i], 0);
    }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(AllThreadsTerminatedTests)

BOOST_AUTO_TEST_CASE(all_threads_terminated_all_true) {
    bool flags[3] = {true, true, true};
    BOOST_CHECK(all_threads_terminated(flags, 3));
}

BOOST_AUTO_TEST_CASE(all_threads_terminated_one_false) {
    bool flags[4] = {true, false, true, true};
    BOOST_CHECK(!all_threads_terminated(flags, 4));
}

BOOST_AUTO_TEST_CASE(all_threads_terminated_all_false) {
    bool flags[2] = {false, false};
    BOOST_CHECK(!all_threads_terminated(flags, 2));
}

BOOST_AUTO_TEST_CASE(all_threads_terminated_single_true) {
    bool flags[1] = {true};
    BOOST_CHECK(all_threads_terminated(flags, 1));
}

BOOST_AUTO_TEST_CASE(all_threads_terminated_single_false) {
    bool flags[1] = {false};
    BOOST_CHECK(!all_threads_terminated(flags, 1));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(PrintArrayTests)

BOOST_AUTO_TEST_CASE(print_array_standard_output) {
    int arr[3] = {1, 0, 2};
    std::ostringstream oss;
    print_array(arr, 3, oss);

    BOOST_CHECK_EQUAL(oss.str(), "Array: 1 0 2 \n");
}

BOOST_AUTO_TEST_CASE(print_array_single_element) {
    int arr[1] = {5};
    std::ostringstream oss;
    print_array(arr, 1, oss);

    BOOST_CHECK_EQUAL(oss.str(), "Array: 5 \n");
}

BOOST_AUTO_TEST_CASE(print_array_all_zeros) {
    int arr[4] = {0, 0, 0, 0};
    std::ostringstream oss;
    print_array(arr, 4, oss);

    BOOST_CHECK_EQUAL(oss.str(), "Array: 0 0 0 0 \n");
}

BOOST_AUTO_TEST_CASE(print_array_negative_numbers) {
    int arr[2] = {-1, -5};
    std::ostringstream oss;
    print_array(arr, 2, oss);

    BOOST_CHECK_EQUAL(oss.str(), "Array: -1 -5 \n");
}

BOOST_AUTO_TEST_SUITE_END()