cmake_minimum_required(VERSION 3.10)
project(ClaudeAgentGtk VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)

# Include directories
include_directories(${GTKMM_INCLUDE_DIRS})

# Source files
set(SOURCES
    src/main.cpp
    src/claude_agent.cpp
    src/claude_agent_gui.cpp
    src/config_dialog.cpp
    src/config_library_dialog.cpp
    src/json_utils.cpp
    src/logger.cpp
)

# Headers
set(HEADERS
    include/claude_agent.h
    include/claude_agent_gui.h
    include/config_dialog.h
    include/config_library_dialog.h
    include/json_utils.h
    include/logger.h
)

# Create include directory
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(${PROJECT_NAME} ${GTKMM_LIBRARIES})

# Compiler flags
target_compile_options(${PROJECT_NAME} PRIVATE ${GTKMM_CFLAGS_OTHER})

# Install target
install(TARGETS ${PROJECT_NAME} DESTINATION bin)