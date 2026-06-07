#==============================================================================
# Demo executables
#==============================================================================

add_executable(fxt_test main.cpp)
target_link_libraries(fxt_test PRIVATE fxt::fxt)

add_executable(fxt_and_then_demo AndThen.cpp)
target_link_libraries(fxt_and_then_demo PRIVATE fxt::fxt)

add_executable(fxt_transform_demo Transform.cpp)
target_link_libraries(fxt_transform_demo PRIVATE fxt::fxt)

add_executable(fxt_or_else_demo OrElse.cpp)
target_link_libraries(fxt_or_else_demo PRIVATE fxt::fxt)

add_executable(fxt_value_demo Value.cpp)
target_link_libraries(fxt_value_demo PRIVATE fxt::fxt)

add_executable(fxt_value_or_demo ValueOr.cpp)
target_link_libraries(fxt_value_or_demo PRIVATE fxt::fxt)

add_executable(fxt_curry_demo Curry.cpp)
target_link_libraries(fxt_curry_demo PRIVATE fxt::fxt)

add_executable(fxt_drop_demo Drop.cpp)
target_link_libraries(fxt_drop_demo PRIVATE fxt::fxt)

add_executable(fxt_take_demo Take.cpp)
target_link_libraries(fxt_take_demo PRIVATE fxt::fxt)

add_executable(fxt_match_demo Match.cpp)
target_link_libraries(fxt_match_demo PRIVATE fxt::fxt)

add_executable(fxt_transform_error_demo TransformError.cpp)
target_link_libraries(fxt_transform_error_demo PRIVATE fxt::fxt)

add_executable(fxt_to_optional_demo ToOptional.cpp)
target_link_libraries(fxt_to_optional_demo PRIVATE fxt::fxt)

add_executable(fxt_to_expected_demo ToExpected.cpp)
target_link_libraries(fxt_to_expected_demo PRIVATE fxt::fxt)

add_executable(fxt_tap_demo Tee.cpp)
target_link_libraries(fxt_tap_demo PRIVATE fxt::fxt)

add_executable(fxt_tap_error_and_none_demo TeeErrorAndNone.cpp)
target_link_libraries(fxt_tap_error_and_none_demo PRIVATE fxt::fxt)

add_executable(fxt_failure_demo Failure.cpp)
target_link_libraries(fxt_failure_demo PRIVATE fxt::fxt)

add_executable(fxt_variant_index_demo VariantIndex.cpp)
target_link_libraries(fxt_variant_index_demo PRIVATE fxt::fxt)

add_executable(fxt_variant_visit_demo VariantVisit.cpp)
target_link_libraries(fxt_variant_visit_demo PRIVATE fxt::fxt)

add_executable(fxt_variant_when_demo VariantWhen.cpp)
target_link_libraries(fxt_variant_when_demo PRIVATE fxt::fxt)

add_executable(fxt_variant_transform_when_demo VariantTransformWhen.cpp)
target_link_libraries(fxt_variant_transform_when_demo PRIVATE fxt::fxt)

add_executable(fxt_immutable_demo Immutable.cpp)
target_link_libraries(fxt_immutable_demo PRIVATE fxt::fxt)

add_executable(fxt_append_demo Append.cpp)
target_link_libraries(fxt_append_demo PRIVATE fxt::fxt)

add_executable(fxt_apply_demo Apply.cpp)
target_link_libraries(fxt_apply_demo PRIVATE fxt::fxt)

add_executable(fxt_apply_append_demo ApplyAppend.cpp)
target_link_libraries(fxt_apply_append_demo PRIVATE fxt::fxt)

add_executable(fxt_apply_replace_demo ApplyReplace.cpp)
target_link_libraries(fxt_apply_replace_demo PRIVATE fxt::fxt)

add_executable(fxt_get_demo Get.cpp)
target_link_libraries(fxt_get_demo PRIVATE fxt::fxt)

add_executable(fxt_select_demo Select.cpp)
target_link_libraries(fxt_select_demo PRIVATE fxt::fxt)

add_executable(fxt_type_demo Type.cpp)
target_link_libraries(fxt_type_demo PRIVATE fxt::fxt)

add_executable(fxt_logical_or_demo LogicalOr.cpp)
target_link_libraries(fxt_logical_or_demo PRIVATE fxt::fxt)

add_executable(fxt_flat_tuple_demo FlatTuple.cpp)
target_link_libraries(fxt_flat_tuple_demo PRIVATE fxt::fxt)

add_executable(fxt_tuple_size_demo TupleSize.cpp)
target_link_libraries(fxt_tuple_size_demo PRIVATE fxt::fxt)

add_executable(fxt_tuple_append_demo TupleAppend.cpp)
target_link_libraries(fxt_tuple_append_demo PRIVATE fxt::fxt)

add_executable(fxt_tuple_prepend_demo TuplePrepend.cpp)
target_link_libraries(fxt_tuple_prepend_demo PRIVATE fxt::fxt)

add_executable(fxt_tuple_element_demo TupleElement.cpp)
target_link_libraries(fxt_tuple_element_demo PRIVATE fxt::fxt)

add_executable(fxt_tuple_transform_demo TupleTransform.cpp)
target_link_libraries(fxt_tuple_transform_demo PRIVATE fxt::fxt)

add_executable(fxt_tuple_cat_demo TupleCat.cpp)
target_link_libraries(fxt_tuple_cat_demo PRIVATE fxt::fxt)

add_executable(fxt_tuple_reverse_demo TupleReverse.cpp)
target_link_libraries(fxt_tuple_reverse_demo PRIVATE fxt::fxt)

add_executable(fxt_lazy_demo Lazy.cpp)
target_link_libraries(fxt_lazy_demo PRIVATE fxt::fxt)

add_executable(fxt_attempt_demo Attempt.cpp)
target_link_libraries(fxt_attempt_demo PRIVATE fxt::fxt)

add_executable(fxt_as_array_demo AsArrayDemo.cpp)
target_link_libraries(fxt_as_array_demo PRIVATE fxt::fxt)

add_executable(fxt_mas_array_demo MasArrayDemo.cpp)
target_link_libraries(fxt_mas_array_demo PRIVATE fxt::fxt)

add_executable(fxt_string_enum_demo StringEnum.cpp)
target_link_libraries(fxt_string_enum_demo PRIVATE fxt::fxt)

add_executable(fxt_type_enum_demo TypeEnum.cpp)
target_link_libraries(fxt_type_enum_demo PRIVATE fxt::fxt)

add_executable(fxt_tuple_conversion_demo TupleConversion.cpp)
target_link_libraries(fxt_tuple_conversion_demo PRIVATE fxt::fxt)

add_executable(fxt_mzip_demo MZip.cpp)
target_link_libraries(fxt_mzip_demo PRIVATE fxt::fxt)

add_executable(fxt_with_demo With.cpp)
target_link_libraries(fxt_with_demo PRIVATE fxt::fxt)

