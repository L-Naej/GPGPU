file(GLOB_RECURSE SRC_FILES *.cpp *.hpp)
file(GLOB SHADERS_FILES  ../shaders/*)#pour les voirs dans son IDE

add_executable(TD1 main.cpp ${SRC_FILES} ${SHADERS_FILES})
target_link_libraries(TD1 ${ALL_LIBRARIES})

install(TARGETS TD1 DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../)
