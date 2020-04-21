set(DOXYGEN_EXTRACT_ALL YES)
set(DOXYGEN_BUILTIN_STL_SUPPORT YES)

file(GLOB SOURCE_LIST CONFIGURE_DEPENDS
    "${FinalProject_SOURCE_DIR}/include/**/*.h"
    "${FinalProject_SOURCE_DIR}/include/**/*.hpp"
    "${FinalProject_SOURCE_DIR}/include/**/*.cc"
    "${FinalProject_SOURCE_DIR}/include/**/*.cpp")

doxygen_add_docs(docs
    ${SOURCE_LIST}
    "README.md"
    WORKING_DIRECTORY
    "../../../include"
)
