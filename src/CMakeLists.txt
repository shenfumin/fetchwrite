set(module_name fetchwrite)
aux_source_directory(. SRC_FILES)
add_library(${module_name} ${SRC_FILES})
target_include_directories(${module_name} PUBLIC .)
