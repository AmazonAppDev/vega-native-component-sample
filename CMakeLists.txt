cmake_minimum_required(VERSION 3.25)

project(NativeComponentExample
        VERSION 1.0
        LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(LIB_NAME nativeuicomponents)
set(IDL_NAME ${LIB_NAME}-idl)

find_package(Apmf REQUIRED)
find_package(Kepler REQUIRED)
kepler_add_idl_import_library(
  ${IDL_NAME}
  PACKAGES
  com.amazon.kepler.uitoolkit.graphics
  com.amazon.kepler.uitoolkit.view
  com.amazon.kepler.uitoolkit.react
  com.amazon.kepler.input.events
)

set(SOURCES
    src/kepler/ViewManagerFactory.cpp
    src/kepler/ButtonContainer/ButtonContainerViewManager.cpp
    src/kepler/ImageCard/ImageCardViewManager.cpp
)

kepler_add_library(${LIB_NAME} SHARED ${SOURCES})
set_target_properties(${LIB_NAME} PROPERTIES
  PREFIX ""
  OUTPUT_NAME "com.amazondeveloper.nativecomponentexample"
)
target_link_libraries(${LIB_NAME} PRIVATE ${IDL_NAME})

kepler_install_components(${LIB_NAME} IN_PROCESS
  "/com.amazondeveloper.nativecomponentexample")
