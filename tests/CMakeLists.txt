add_library(Catch2ForFXT STATIC Catch2/catch_amalgamated.cpp)
target_include_directories(Catch2ForFXT SYSTEM PUBLIC Catch2)

# Add test executable
add_executable(fxt_tests
        AppendTests.cpp
        ApplyTests.cpp
        FailureTests.cpp
        FlatTupleTests.cpp
        GetTests.cpp
        ValueTests.cpp
        TransformTests.cpp
        TransformErrorTests.cpp
        AndThenTests.cpp
        OrElseTests.cpp
        SelectTests.cpp
        ValueOrTests.cpp
        CurryTests.cpp
        DropTests.cpp
        TakeTests.cpp
        MatchTests.cpp
        ToOptionalTests.cpp
        ToExpectedTests.cpp
        TapTests.cpp
        TapErrorAndNoneTests.cpp
        TupleAppendTests.cpp
        TupleElementTests.cpp
        TupleSizeTests.cpp
        TypeTests.cpp
)

target_link_libraries(fxt_tests
        PRIVATE
        fxt::fxt
        Catch2ForFXT
)
