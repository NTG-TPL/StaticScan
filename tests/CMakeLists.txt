set(test_target scan_tests)

add_executable(${test_target}
    main.cpp
    types_test.cpp
    scannable_test.cpp
    format_string_test.cpp
    match_test.cpp
    parse_test.cpp
    scan_test.cpp)

target_link_libraries(${test_target} PRIVATE ${target})

add_test(NAME ${test_target} COMMAND ${test_target})

set(include_flags "$<LIST:TRANSFORM,$<TARGET_PROPERTY:${target},INTERFACE_INCLUDE_DIRECTORIES>,PREPEND,-I>")
set(compile_fail_flags ${CMAKE_CXX${CMAKE_CXX_STANDARD}_STANDARD_COMPILE_OPTION} ${include_flags})
set(case_regex "^#(el)?if defined\\(([A-Z_]+)\\) +// expected-error: (.+)")

file(GLOB compile_fail_sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/compile_fail/*.cpp)
foreach(source ${compile_fail_sources})
    get_filename_component(group ${source} NAME_WE)
    file(STRINGS ${source} cases REGEX "${case_regex}")
    foreach(case ${cases})
        string(REGEX MATCH "${case_regex}" _ "${case}")

        set(macro ${CMAKE_MATCH_2})
        set(expected_error "${CMAKE_MATCH_3}")

        string(TOLOWER ${macro} name)
        set(test_name compile_fail.${group}.${name})
        add_test(NAME ${test_name}
                 COMMAND ${CMAKE_CXX_COMPILER} ${compile_fail_flags} -fsyntax-only -D${macro} ${source}
                 COMMAND_EXPAND_LISTS)
        set_tests_properties(${test_name} PROPERTIES PASS_REGULAR_EXPRESSION "${expected_error}")
    endforeach()
endforeach()
