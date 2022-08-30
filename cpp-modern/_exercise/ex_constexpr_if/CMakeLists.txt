##################
# Target
get_filename_component(DIRECTORY_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" TARGET_MAIN ${DIRECTORY_NAME})

####################
# Sources & headers
aux_source_directory(. SRC_LIST)
file(GLOB HEADERS_LIST "*.h" "*.hpp")

add_executable(${TARGET_MAIN} ${SRC_LIST} ${HEADERS_LIST})

#target_compile_features(${TARGET_MAIN} PRIVATE cxx_std_20)

if (MSVC)
    target_compile_options(${TARGET_MAIN} PUBLIC /std:c++latest)
else()
    target_compile_features(${TARGET_MAIN} PUBLIC cxx_std_20)
endif()

add_test(${TARGET_MAIN}_tests ${TARGET_MAIN})