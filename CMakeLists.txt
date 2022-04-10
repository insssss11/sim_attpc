#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.8...3.18)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()
project(sim_attpc)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

# check Geant4 version
if(${Geant4_VERSION} VERSION_LESS 11.0.0)
  message(FATAL_ERROR "Geant4 version must be higher than 11.0.0")
endif()

# to enable to complie with ROOT lib.
find_package(ROOT REQUIRED)
# check ROOT version
if(${ROOT_VERSION} VERSION_LESS 6.24/00)
  message(FATAL_ERROR "ROOT version must be higher than 6.24/00")
endif()

# Garfield++
if(NOT TARGET Garfield::Garfield)
  find_package(Garfield REQUIRED)
endif()

set(CMAKE_CXX_STANDARD 17)

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE} ${ROOT_USE_FILE})
#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include
                    ${Geant4_INCLUDE_DIR} ${ROOT_INCLUDE_DIRS})

file(GLOB sources
  ${PROJECT_SOURCE_DIR}/src/*.cc
  ${PROJECT_SOURCE_DIR}/src/custom_processes/*.cc
  ${PROJECT_SOURCE_DIR}/src/custom_processes/carbon_alpha/*.cc
  ${PROJECT_SOURCE_DIR}/src/custom_processes/mean_free_path/*.cc
  ${PROJECT_SOURCE_DIR}/src/gas_chamber/*.cc
  ${PROJECT_SOURCE_DIR}/src/gas/*.cc
  ${PROJECT_SOURCE_DIR}/src/detector_construction/*.cc
  ${PROJECT_SOURCE_DIR}/src/tuple/*.cc
  ${PROJECT_SOURCE_DIR}/src/config/*.cc
  ${PROJECT_SOURCE_DIR}/src/readout/*.cc
  ${PROJECT_SOURCE_DIR}/src/training_data/*.cc
  ${PROJECT_SOURCE_DIR}/src/exception/*.cc
  ${PROJECT_SOURCE_DIR}/src/tree_merger/*.cc
  )
file(GLOB headers
  ${PROJECT_SOURCE_DIR}/include/*.hh
  ${PROJECT_SOURCE_DIR}/include/custom_processes/*.hh
  ${PROJECT_SOURCE_DIR}/include/custom_processes/carbon_alpha/*.hh
  ${PROJECT_SOURCE_DIR}/include/custom_processes/mean_free_path/*.hh
  ${PROJECT_SOURCE_DIR}/include/gas_chamber/*.hh
  ${PROJECT_SOURCE_DIR}/include/gas/*.hh
  ${PROJECT_SOURCE_DIR}/include/detector_construction/*.hh
  ${PROJECT_SOURCE_DIR}/include/tuple/*.hh
  ${PROJECT_SOURCE_DIR}/include/config/*.hh
  ${PROJECT_SOURCE_DIR}/include/readout/*.hh
  ${PROJECT_SOURCE_DIR}/include/training_data/*.hh
  ${PROJECT_SOURCE_DIR}/include/exception/*.hh
  ${PROJECT_SOURCE_DIR}/include/tree_merger/*.hh
  )

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(sim_attpc sim_attpc.cc ${sources} ${headers})
target_link_libraries(sim_attpc ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} Garfield::Garfield)
target_compile_options(sim_attpc PRIVATE -Wno-shadow)
add_executable(gen_training_data gen_training_data.cc ${sources} ${headers})
target_link_libraries(gen_training_data ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} Garfield::Garfield)
target_compile_options(gen_training_data PRIVATE -Wno-shadow)
#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build B5. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(SCRIPTS
  init.mac
  vis.mac
  ${gps_macros}
  gmacros/bragg_peak.mac
  rmacros/DrawBraggPeak.cc

  gmacros/trajectories_overlap_vis.mac
  
  gmacros/pad_charges_vis.mac
  gmacros/pad_charges.mac  
  rmacros/DrawPadCharges.cc
  rmacros/EventDrawingAction.hh
  
  rmacros/AddTrees.cc
  rmacros/BranchSummation/BranchSummation.hh
  rmacros/BranchSummation/EntrySummation.hh
  rmacros/BranchSummation/BranchAppender.hh

  parameters/config.txt
  parameters/gas_chamber.txt
  )

  foreach(_script ${SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()
