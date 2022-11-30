cmake_minimum_required(VERSION 3.0.0)
project(lobajson VERSION 0.1.0)

add_subdirectory(loba)


enable_testing()
add_subdirectory(tests)

# define tests
add_test(
        NAME loba_test
        COMMAND $<TARGET_FILE:lobaJsonTest>
)
