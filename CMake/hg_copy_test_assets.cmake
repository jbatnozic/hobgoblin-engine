
function(hg_copy_test_assets A_TARGET A_ASSET_DIR)
    set(COPIED_DIR_NAME "_${A_TARGET}_TestAssets")

    add_custom_command(
    OUTPUT
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${COPIED_DIR_NAME}"

    COMMAND 
        ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/${A_ASSET_DIR}"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${COPIED_DIR_NAME}"

    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/${A_ASSET_DIR}"
    )

    set(COMMAND_NAME "hg-copy-test-assets-${A_TARGET}-${A_ASSET_DIR}")

    add_custom_target(${COMMAND_NAME}
        DEPENDS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${COPIED_DIR_NAME}"
    )

    add_dependencies(${A_TARGET} ${COMMAND_NAME})

    target_compile_definitions(${A_TARGET}
    PUBLIC
        HG_TEST_ASSET_DIR="${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${COPIED_DIR_NAME}"
    )
endfunction()
