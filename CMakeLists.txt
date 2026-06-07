cmake_minimum_required(VERSION 3.10.0)
project(LunarLander VERSION 2.5.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 11)

configure_file(${PROJECT_SOURCE_DIR}/config.h.in config.h)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/src/main.cpp
                               ${PROJECT_SOURCE_DIR}/include/GameScripts/TextureManager/textureManager.cpp
                               ${PROJECT_SOURCE_DIR}/include/GameScripts/floor.h
                               ${PROJECT_SOURCE_DIR}/include/GameScripts/lander.h
                               ${PROJECT_SOURCE_DIR}/include/GameScripts/goal.h
                               ${PROJECT_SOURCE_DIR}/include/GameScripts/aiLander.h
                               ${PROJECT_SOURCE_DIR}/include/GameScripts/ai.h
                               ${PROJECT_SOURCE_DIR}/include/GameScripts/collisionLayers.h
)

target_compile_definitions(${PROJECT_NAME} PRIVATE GLM_ENABLE_EXPERIMENTAL)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/include/EngineIncludes ${PROJECT_SOURCE_DIR}/include/EngineIncludes/imgui)
target_link_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/lib)
target_link_libraries(${PROJECT_NAME} Engine soundio glfw3 pulse NEAT)   