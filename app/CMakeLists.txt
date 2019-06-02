# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

set(OBOE_DIR "../../../projects/oboe")
get_filename_component(SAMPLE_ROOT_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../projects/oboe/samples ABSOLUTE)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             src/main/cpp/native-lib.cpp
        src/main/cpp/MicrophoneInput.cpp)

add_subdirectory(${OBOE_DIR} ./oboe-bin)

include_directories(${OBOE_DIR}/include)



include_directories(/home/student/workspace/MicrophoneInput/module/lib/x86_64/include/)
add_library(libessentia SHARED IMPORTED)

# TODO: Use ABI variable

set_target_properties(libessentia
        PROPERTIES IMPORTED_LOCATION
        /home/student/workspace/MicrophoneInput/module/lib/x86_64/libessentia.so)


# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

#target_include_directories( PRIVATE
#        ${SAMPLE_ROOT_DIR}/debug-utils)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       native-lib

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib}

        oboe
        libessentia)

