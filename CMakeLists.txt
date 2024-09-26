# CMakeLists.txt for smdt package
cmake_minimum_required(VERSION 2.8.10)
project(calvision)


#
# Include the ROOT library
#

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

find_package(ROOT REQUIRED COMPONENTS MathCore RIO Hist Tree Net)
find_package(HDF5 REQUIRED COMPONENTS C CXX)

message(${ROOT_INCLUDE_DIRS})
include_directories(${ROOT_INCLUDE_DIRS})
include_directories(${HDF5_INCLUDE_DIRS})



#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
include(${ROOT_USE_FILE})



#
# Define which files are headers and source
#

file(GLOB CalVision_HEADERS ${PROJECT_SOURCE_DIR}/inc/*.h)
file(GLOB CalVision_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)


#
# Link this library to ROOT
#

include_directories(${CMAKE_CURRENT_SOURCE_DIR})


ROOT_GENERATE_DICTIONARY(G__calvision_CINT ${CalVision_HEADERS} LINKDEF src/LinkDef.h)

add_library(calvision_CINT SHARED ${CalVision_SOURCES} ${CalVision_HEADERS} G__calvision_CINT.cxx)

add_library(calvision SHARED ${CalVision_SOURCES} G__calvision_CINT.cxx)



#
# Create executables for muon reconstruction
#

add_executable(hdf2root run/hdf2root.cpp)
target_link_libraries(hdf2root calvision ${ROOT_LIBRARIES} ${HDF5_LIBRARIES})

add_executable(rootdraw run/rootdraw.cpp)
target_link_libraries(rootdraw calvision ${ROOT_LIBRARIES} ${HDF5_LIBRARIES})

add_executable(doFFT run/doFFT.cpp)
target_link_libraries(doFFT calvision ${ROOT_LIBRARIES} ${HDF5_LIBRARIES})


set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-shadow")

target_link_libraries(calvision_CINT PUBLIC ROOT::Core)
