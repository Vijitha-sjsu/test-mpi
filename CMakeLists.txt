cmake_minimum_required(VERSION 3.10)

# Set some basic project attributes
project(mini2
    VERSION 0.1
    DESCRIPTION "A Hello World Project")
    
# Find the OpenMP package
find_package(OpenMP REQUIRED)
find_package(MPI REQUIRED)

add_subdirectory(matplotplusplus)
# Include the configuration header in the build and add CSVProcessor.cpp to the executable
#add_executable(${PROJECT_NAME} mini2.cpp CSVProcessor.cpp)
add_executable(${PROJECT_NAME} 
    #src/main.cpp
    src/mini2.cpp 
    src/CSVProcessor.cpp
    src/Analyzer.cpp
    src/MPIParser.cpp
    #src/CompleteProcessor.cpp
)

# target_link_libraries(${PROJECT_NAME} PUBLIC matplot)
target_link_libraries(${PROJECT_NAME} PUBLIC matplot OpenMP::OpenMP_CXX ${MPI_CXX_LIBRARIES}
)

# If OpenMP was found, link it to the target
# if(OpenMP_CXX_FOUND OR OpenMP_FOUND)
#     target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX)
# endif()

# Include directories for MPI and Matplot++
target_include_directories(${PROJECT_NAME} 
    PRIVATE ${MPI_INCLUDE_PATH}
    PUBLIC "${PROJECT_BINARY_DIR}"
)

if(MPI_COMPILE_FLAGS)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()


# Create a simple configuration header
configure_file(config.h.in config.h)

# Include the configuration header in the build
target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_BINARY_DIR}")

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Explicitly setting OpenMP flags and libraries might not be necessary 
# if find_package(OpenMP REQUIRED) successfully finds and configures OpenMP
