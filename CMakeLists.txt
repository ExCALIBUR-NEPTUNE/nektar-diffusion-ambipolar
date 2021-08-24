CMAKE_MINIMUM_REQUIRED(VERSION 3.5.1)

PROJECT(DiffusionSolver)

# Nektar++ requires C++11.
SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)

# Default install location: build/dist
IF (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    SET(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/dist CACHE PATH "" FORCE)
ENDIF()

# Find Nektar++
FIND_PACKAGE(Nektar++ REQUIRED)
INCLUDE_DIRECTORIES(SYSTEM ${NEKTAR++_INCLUDE_DIRS} ${NEKTAR++_TP_INCLUDE_DIRS})
LINK_DIRECTORIES(${NEKTAR++_LIBRARY_DIRS} ${NEKTAR++_TP_LIBRARY_DIRS})
MESSAGE(STATUS "Found Nektar++: version ${NEKTAR++_VERSION}")

# Set RPATH for wherever Nektar++ is installed.
SET(CMAKE_INSTALL_RPATH "${NEKTAR++_LIBRARY_DIRS}")

# Components of the Nektar++ package to build
OPTION(NEKTAR_BUILD_DOCS    "Build the latex document." OFF)
IF (NEKTAR_BUILD_DOCS)
	INCLUDE_DIRECTORIES(docs)
	ADD_SUBDIRECTORY(docs)
ENDIF (NEKTAR_BUILD_DOCS)

# Add source files.
ADD_EXECUTABLE(DiffusionSolver
    src/DiffusionSolver.cpp
    src/UnsteadyDiffusion.cpp
    src/SteadyDiffusion.cpp
    src/SteadyDiffusionVaryingB.cpp
    )

# Add target link libraries.
TARGET_LINK_LIBRARIES(DiffusionSolver ${NEKTAR++_LIBRARIES} ${NEKTAR++_TP_LIBRARIES})
INSTALL(TARGETS DiffusionSolver DESTINATION ${CMAKE_INSTALL_PREFIX})

TARGET_COMPILE_DEFINITIONS(DiffusionSolver PUBLIC ${NEKTAR++_DEFINITIONS})

#SET_TARGET_PROPERTIES(TimingGeneralMatrixOp2D PROPERTIES COMPILE_FLAGS -DBASE_PATH="\\"${CMAKE_CURRENT_SOURCE_DIR}\\"")
#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/example/do_TimingGeneralMatrixOp2D       
#    ${CMAKE_BINARY_DIR}/dist/bin/do_TimingGeneralMatrixOp2D COPYONLY)


