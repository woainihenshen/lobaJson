add_library(lobajson "")
target_sources(lobajson PRIVATE lobajson.cpp)
target_include_directories(lobajson PUBLIC ${PROJECT_SOURCE_DIR}/loba)




