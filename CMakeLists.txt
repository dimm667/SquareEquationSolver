cmake_minimum_required(VERSION 3.14.0)
project(SquareEquationSolver VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
  googletest
  # Specify the commit you depend on and update it regularly.
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(solver_test solver_test.cpp)

add_dependencies(solver_test gtest)
add_dependencies(solver_test gmock)
target_link_libraries(solver_test gtest_main)
target_link_libraries(solver_test gmock_main)

enable_testing()
add_test(NAME    solver_test
         COMMAND solver_test)


add_executable(SquareEquationSolver producer_consumer_pipe.h argument_parser.h solution_printer.h equation_issue.h equation_solution.h solver.h main.cpp)
set (CMAKE_CXX_FLAGS "-pthread")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
