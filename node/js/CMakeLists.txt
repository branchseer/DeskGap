file(GLOB_RECURSE NODE_TS_FILES node/*.ts)
file(GLOB_RECURSE UI_TS_FILES ui/*.ts)
file(GLOB_RECURSE COMMON_TS_FILES common/*.ts)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/dg_node.js
    COMMAND ${NPM_WEBPACK}
        --config ${CMAKE_CURRENT_LIST_DIR}/node/webpack.config.js
        $<$<CONFIG:Release>:-p> --mode=$<IF:$<CONFIG:Release>,production,development>
        -o ${CMAKE_CURRENT_BINARY_DIR}/dg_node.js
    DEPENDS ${NODE_TS_FILES} ${COMMON_TS_FILES} node/tsconfig.json node/webpack.config.js ${CMAKE_CURRENT_LIST_DIR}/../VERSION
)
bin2code(${CMAKE_CURRENT_BINARY_DIR}/dg_node.js ${CMAKE_CURRENT_BINARY_DIR}/dg_node.c)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/dg_ui.js
    COMMAND ${NPM_WEBPACK}
        --config ${CMAKE_CURRENT_LIST_DIR}/ui/webpack.config.js
        $<$<CONFIG:Release>:-p> --mode=$<IF:$<CONFIG:Release>,production,development>
        -o ${CMAKE_CURRENT_BINARY_DIR}/dg_ui.js
    DEPENDS ${UI_TS_FILES} ${COMMON_TS_FILES} ui/tsconfig.json ui/webpack.config.js
)
bin2code(${CMAKE_CURRENT_BINARY_DIR}/dg_ui.js ${CMAKE_CURRENT_BINARY_DIR}/dg_ui.c)

add_library(DeskGapNodeScripts OBJECT ${CMAKE_CURRENT_BINARY_DIR}/dg_node.c ${CMAKE_CURRENT_BINARY_DIR}/dg_ui.c)

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    bin2code(${DG_NODE_MODULES_DIR}/es6-promise/dist/es6-promise.auto.min.js ${CMAKE_CURRENT_BINARY_DIR}/es6_promise_auto_min.c)
    target_sources(DeskGapNodeScripts PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/es6_promise_auto_min.c)
endif ()
