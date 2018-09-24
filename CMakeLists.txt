cmake_minimum_required(VERSION 2.8.11)

project(bbtoolkit)

set(EXEC_NAME bbtoolkit)

set(CUSTOM_LIB_DIR $ENV{HOME}/CustomLibs/${CMAKE_BUILD_TYPE})

find_package(Qt5Core)
find_package(Qt5Widgets)
find_package(Qt5Gui)

# Set the source directories
set(BBTOOLKIT_SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(UI_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/ui)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Set the include directory
set(BBTOOLKIT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(URHO3D_INCLUDE_DIR ${CUSTOM_LIB_DIR}/include)
set(THIRD_PARTY_INCLUDE_DIR ${URHO3D_INCLUDE_DIR}/Urho3D/ThirdParty)


# Postfix debug builds with d
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# Set the src files for the project
file(GLOB SRC_FILES 
  "${BBTOOLKIT_SRC_DIR}/*.cpp"
  )

file(GLOB UI_FILES
  "${UI_INCLUDE_DIR}/*.ui"
  )

file(GLOB HEADER_FILES
  "${BBTOOLKIT_INCLUDE_DIR}/*.h"
  )

qt5_wrap_ui(UI_HEADERS ${UI_FILES})
qt5_wrap_cpp(SRC_MOC_FILES ${HEADER_FILES})
qt5_add_resources(SRC_RES_FILE ${CMAKE_SOURCE_DIR}/toolkit.qrc)

# Set project includes dir
include_directories(
  "${BBTOOLKIT_INCLUDE_DIR}"
  "${URHO3D_INCLUDE_DIR}"
  "${THIRD_PARTY_INCLUDE_DIR}"
  )

if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
  add_definitions(-DDEBUG_VERSION)
else()
  add_definitions(-DRELEASE_VERSION)
endif()

add_definitions(-DQT_BUILD)

if (UNIX)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")
elseif (WIN32)
  set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} /WX /NODEFAULTLIB:MSVCRT /NODEFAULTLIB:LIBCMT" )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
endif()

add_executable(${EXEC_NAME} ${SRC_FILES} ${SRC_MOC_FILES} ${UI_HEADERS} ${SRC_RES_FILE})

# copy all the files in the deploy folder to bin
add_custom_target(deploy_files ALL
  COMMAND cmake -E copy_directory ${CMAKE_SOURCE_DIR}/deploy ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  DEPENDS ${TARGET_NAME}
  )

# Create a shortcut for the compile commands
add_custom_target(move_compile_commands ALL
  COMMAND cmake -E create_symlink ${CMAKE_BINARY_DIR}/compile_commands.json ${CMAKE_SOURCE_DIR}/compile_commands.json
  DEPENDS ${TARGET_NAME}
  )

find_library(URHO3D Urho3D PATHS ${CUSTOM_LIB_DIR}/lib/Urho3D)

# link with opengl (no debug version)
find_package(OpenGL REQUIRED)
if(NOT OPENGL_FOUND)
  message(ERROR "Error: OpenGL not found on your system - please make sure you have it downloaded.")
endif(NOT OPENGL_FOUND)
target_link_libraries(${EXEC_NAME} ${OPENGL_LIBRARIES})

# Link in the unix only stuff
if (UNIX)
  target_link_libraries(${EXEC_NAME} pthread)
  target_link_libraries(${EXEC_NAME} dl)
endif()

# link in the libraries for Urho3d
target_link_libraries(${EXEC_NAME}
  ${OPENGL_LIBRARIES}
  Qt5::Core
  Qt5::Widgets
  Qt5::Gui
  ${URHO3D}
  )