cmake_minimum_required(VERSION 3.10)  # Specify the minimum CMake version required

# Set the project name and version
project(MyProject VERSION 1.0 LANGUAGES CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add an executable target
add_executable(my_program
    src/main.cpp   # Source files for the executable
    #src/other_file.cpp  # Add other source files here
)

# Optionally, add include directories
target_include_directories(my_program
    PRIVATE ${PROJECT_SOURCE_DIR}/include
)

# Optionally, link libraries
# target_link_libraries(my_program
#     some_library
# )
