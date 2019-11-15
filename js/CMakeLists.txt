file(GLOB_RECURSE NODE_TS_FILES node/*.ts)
file(GLOB_RECURSE UI_TS_FILES ui/*.ts)
file(GLOB_RECURSE COMMON_TS_FILES common/*.ts)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/dg_node.c
    COMMAND npm run --prefix ${CMAKE_CURRENT_LIST_DIR} build-lib-node --  $<$<CONFIG:Release>:-p> --mode=$<IF:$<CONFIG:Release>,production,development>  -o ${CMAKE_CURRENT_BINARY_DIR}/dg_node.js
    COMMAND node ${CMAKE_SOURCE_DIR}/scripts/bin2code.js ${CMAKE_CURRENT_BINARY_DIR}/dg_node.js ${CMAKE_CURRENT_BINARY_DIR}/dg_node.c
    DEPENDS package.json ${NODE_TS_FILES} ${COMMON_TS_FILES} node/tsconfig.json node/webpack.config.js
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/dg_ui.c
    COMMAND npm run --prefix ${CMAKE_CURRENT_LIST_DIR} build-lib-ui --  $<$<CONFIG:Release>:-p> --mode=$<IF:$<CONFIG:Release>,production,development>  -o ${CMAKE_CURRENT_BINARY_DIR}/dg_ui.js
    COMMAND node ${CMAKE_SOURCE_DIR}/scripts/bin2code.js ${CMAKE_CURRENT_BINARY_DIR}/dg_ui.js ${CMAKE_CURRENT_BINARY_DIR}/dg_ui.c
    DEPENDS package.json ${UI_TS_FILES} ${COMMON_TS_FILES} ui/tsconfig.json ui/webpack.config.js
)

add_library(DeskGapNodeScripts OBJECT ${CMAKE_CURRENT_BINARY_DIR}/dg_node.c ${CMAKE_CURRENT_BINARY_DIR}/dg_ui.c)
