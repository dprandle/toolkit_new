cmake_minimum_required(VERSION 2.8.11)

project(bbtoolkit)

set(TARGET_NAME bbtoolkit)

# CMAKE Options
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set binary output folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# Set the source directories
set(BBTOOLKIT_SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(UI_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/ui)

# Set the include directories
set(BBTOOLKIT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

set(URHO3D_DIR ${CMAKE_SOURCE_DIR}/deps/Urho3D/build/${CMAKE_BUILD_TYPE})
set(URHO3D_INCLUDE_DIR ${URHO3D_DIR}/include)
set(THIRD_PARTY_INCLUDE_DIR ${URHO3D_INCLUDE_DIR}/Urho3D/ThirdParty)

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

find_package(Qt5Core)
find_package(Qt5Widgets)
find_package(Qt5Gui)

find_library(URHO3D_LIB
  NAMES Urho3D libUrho3D
  HINTS "${URHO3D_DIR}/lib"
)

add_definitions(-DURHO_HASH_DEBUG)
add_definitions(-DURHO3D_ANGELSCRIPT)
add_definitions(-DURHO3D_LUA)
add_definitions(-DURHO3D_NETWORK)
add_definitions(-DURHO3D_PHYSICS)
add_definitions(-DQT_NO_FOREACH)
add_definitions(-DQT_BUILD)

if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
    add_definitions(-DDEBUG_VERSION)
    add_definitions(-DURHO3D_LOGGING)
else()
    add_definitions(-DRELEASE_VERSION)
endif()

qt5_wrap_ui(UI_HEADERS ${UI_FILES})
qt5_wrap_cpp(SRC_MOC_FILES ${HEADER_FILES})
qt5_add_resources(SRC_RES_FILE ${CMAKE_SOURCE_DIR}/toolkit.qrc)

# Set project includes dir
include_directories(
  "${BBTOOLKIT_INCLUDE_DIR}"
  "${URHO3D_INCLUDE_DIR}"
  "${THIRD_PARTY_INCLUDE_DIR}"
  )

add_executable(${TARGET_NAME} ${SRC_FILES} ${SRC_MOC_FILES} ${UI_HEADERS} ${SRC_RES_FILE})

# copy all the files in the deploy folder to bin
add_custom_target(deploy_files ALL
  COMMAND cmake -E copy_directory ${CMAKE_SOURCE_DIR}/deploy ${CMAKE_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE}/bin
  DEPENDS ${TARGET_NAME}
  )

# Create a shortcut for the compile commands
add_custom_target(move_compile_commands ALL
  COMMAND cmake -E create_symlink ${CMAKE_BINARY_DIR}/compile_commands.json ${CMAKE_SOURCE_DIR}/build/compile_commands.json
  DEPENDS ${TARGET_NAME}
  )

target_link_libraries(${TARGET_NAME} GL)

# Link in the unix only stuff
if (UNIX)
  target_link_libraries(${TARGET_NAME} pthread)
  target_link_libraries(${TARGET_NAME} dl)
endif()

# link in the libraries for Urho3d
target_link_libraries(${TARGET_NAME}
  Qt5::Core
  Qt5::Widgets
  Qt5::Gui
  ${URHO3D_LIB}
  )