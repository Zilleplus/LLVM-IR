add_library(Infra
    token.cpp
    ast.cpp
    astVisit.cpp
    astToString.cpp
    scanner.cpp
)

target_include_directories(Infra PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(Infra PRIVATE fmt::fmt)