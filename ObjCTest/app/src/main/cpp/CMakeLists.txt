# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

include(AndroidNdkModules)
android_ndk_import_module_cpufeatures()

# setup common C flags
set(CMAKE_C_FLAGS "-std=gnu17 -Os ${CMAKE_C_FLAGS}")

# Define list of Objective-C files
set(OBJECTIVEC_SRCS objc_test.m)

# Set up Objective-C suppport via GNUstep according to the current CPU architecture
if(${ANDROID_ABI} STREQUAL "arm64-v8a")
    set(GNUSTEP_HOME "$ENV{HOME}/Library/Android/GNUstep-arm64")

elseif(${ANDROID_ABI} STREQUAL "armeabi-v7a")
    set(GNUSTEP_HOME "$ENV{HOME}/Library/Android/GNUstep-armv7")

elseif(${ANDROID_ABI} STREQUAL "x86_64")
    set(GNUSTEP_HOME "$ENV{HOME}/Library/Android/GNUstep-x86_64")

endif()

# Setup compile options

set(OBJECTIVEC_COMPILE_FLAGS "-I${GNUSTEP_HOME}/include -MP -DGNUSTEP -DGNUSTEP_BASE_LIBRARY=1 -DGNUSTEP_RUNTIME=1 -D_NONFRAGILE_ABI=1 -DGNUSTEP_BASE_LIBRARY=1 -fno-strict-aliasing -pthread -fPIC -Wall -DGSWARN -DGSDIAGNOSE -Wno-import -fobjc-runtime=gnustep-1.9 -fconstant-string-class=NSConstantString")

set(OBJECTIVEC_LINK_OPTIONS "-L${GNUSTEP_HOME}/lib -lgnustep-base -lobjc -lm")

# set compile flags only on Objective-C source files
set_property(SOURCE ${OBJECTIVEC_SRCS}
        APPEND_STRING PROPERTY COMPILE_FLAGS ${OBJECTIVEC_COMPILE_FLAGS})

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
        native-lib

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        native-lib.c

        ${OBJECTIVEC_SRCS}
        )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        native-lib
        cpufeatures

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib}

        # Add linker options for GNUstep
        ${OBJECTIVEC_LINK_OPTIONS}
        )


# Copy GNUstep libraries into output directory to be bundled with the app.
add_custom_command(TARGET native-lib POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${GNUSTEP_HOME}/lib/*.so
        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/)

