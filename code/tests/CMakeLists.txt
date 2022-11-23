add_executable(Tests 
    main.cpp
    scannerTests.cpp
    parserTests.cpp
    codegenTests.cpp
    )
target_link_libraries(Tests PRIVATE Catch2::Catch2)
target_link_libraries(Tests PUBLIC Infra)
target_link_libraries(Tests PRIVATE fmt::fmt)
