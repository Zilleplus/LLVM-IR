add_library(Infra
    token.cpp
    ast.cpp
    astVisit.cpp
    astToString.cpp
    scanner.cpp
    parser.cpp
    codegen.cpp
)

target_include_directories(Infra PUBLIC "${CMAKE_CURRENT_LIST_DIR}/../")
target_link_libraries(Infra PRIVATE fmt::fmt)
target_link_libraries(Infra PUBLIC llvm::llvm)