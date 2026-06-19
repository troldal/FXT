add_library(Catch2ForFXT STATIC Catch2/catch_amalgamated.cpp)
target_include_directories(Catch2ForFXT SYSTEM PUBLIC Catch2)

# Add test executable
add_executable(fxt_tests
        AppendTests.cpp
        ApplyTests.cpp
        ApplyAppendTests.cpp
        ApplyReplaceTests.cpp
        AsArrayTests.cpp
        AttemptTests.cpp
        FailureTests.cpp
        FlatTupleTests.cpp
        GetTests.cpp
        ImmutableTests.cpp
        LazyTests.cpp
        ValueTests.cpp
        TransformTests.cpp
        TransformErrorTests.cpp
        TupleTransformTests.cpp
        AndThenTests.cpp
        OrElseTests.cpp
        SelectTests.cpp
        ValueOrTests.cpp
        CurryTests.cpp
        DropTests.cpp
        TakeTests.cpp
        MatchTests.cpp
        MasArrayTests.cpp
        MonadicTupleAppendTests.cpp
        MonadicTuplePrependTests.cpp
        ToOptionalTests.cpp
        ToExpectedTests.cpp
        TeeTests.cpp
        TeeErrorAndNoneTests.cpp
        TupleAppendTests.cpp
        TupleCatTests.cpp
        TupleElementTests.cpp
        TuplePrependTests.cpp
        TupleReverseTests.cpp
        TupleSizeTests.cpp
        TypeTests.cpp
        VariantIndexTests.cpp
        VariantVisitTests.cpp
        VariantWhenTests.cpp
        VariantTransformWhenTests.cpp
        WithTests.cpp
        JoinTests.cpp
        SequenceTests.cpp
        SequenceTupleTests.cpp
        ForEachTests.cpp
)

target_link_libraries(fxt_tests
        PRIVATE
        fxt::fxt
        Catch2ForFXT
)
