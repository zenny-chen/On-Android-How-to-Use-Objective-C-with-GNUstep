# On Android How to Use Objective-C with GNUstep
Android中使用Objective-C加GNUstep

<br />

当前最新的Android NDK（无论是Android Studio自带的还是独立的）都全面使用了LLVM-Clang编译工具链，这一工具链其实可以支持Objective-C。因此，GNUStep官方针对macOS系统以及Linux系统推出了能在Android上运行的GNUStep库的编译脚本。以下讲解将针对macOS系统，Linux上的操作步骤其实也差不多。

对于Android NDK的实际编译器应用程序，我们可以在NDK目录下的`toolchains/llvm/prebuilt/darwin-x86_64/bin`中找到。这里存放了针对各个处理器架构（arm 、armv7a、aarch64、i686、x86_64）以及各个不同Android SDK版本的Clang与Clang++程序。为了考虑尽可能地向前兼容性，我们可以选择API Level较低的版本。

GNUstep官方GitHub提供了针对Android系统的脚本编译工具，链接为：https://github.com/gnustep/tools-android

这里所用的Android cmake版本是当前最新版本，笔者下载时，其版本为：3.10.2.4988404。此外，该脚本工具当前设置的目标处理器架构为`armeabi-v7a`，针对Android 5.0 (Lollipop / API level 21) SDK 。如果我们要对其中任一项进行修改，可以查看该项目中的 `/env/sdkenv.sh` 脚本文件。

在用 **tools-android** 项目的`build.sh`脚本编译之前，macOS上必须先安装好autoconf与pkg-config。autoconf、automake相关工具的安装请参考此博文：《[macOS下安装autoconf与automake以及其相关的库](https://github.com/zenny-chen/On-macOS-Install-autoconf-and-automake)》。pkg-config项目的安装可以参考此博文中的相关内容：《[Mac与macOS开发相关技术文集](https://github.com/zenny-chen/Mac-and-macOS-Development-Relevant-Resource-Collection)》。

随后，我们去上面的GitHub链接，将整个工程下载下来，放到“下载”目录或桌面都行。然后进入该目录，执行：`bash build.sh` 。如果顺利的话，将会在 `~/Library/Android` 目录下生成GNUstep文件夹，里面就有编译好的libobjc库以及基于GNUstep的Foundation库、libdispatch、libBlockRuntime等库。这些工具都十分强大且好用。

如果各位当前所用的NDK的clang版本在7.0以上，那么我们可以修改 **tools-android** 目录中的 `config/gnustep-make-user.config` 文件，将里面的 `RUNTIME_VERSION=gnustep-1.9` 修改为：`RUNTIME_VERSION=gnustep-2.0`。这样可让我们使用更先进的Objective-C的语法特性！如果修改了这里，那么我们在实际使用中也需要将编译选项 `-fobjc-runtime=gnustep-1.9` 修改为相应的 `-fobjc-runtime=gnustep-2.0`。    
然而，这里有趣的是，笔者自己试了一下，如果我们使用gnustep-2.0，那么在ARMv8环境下会出现运行时加载错误，有一个启动selector的初始化函数会找不到，因此我们这里还是乖乖使用默认的 **`gnustep-1.9`** 就行～尽管在x86_64架构下没有问题……

倘若我们不需要使用Grand Central Dispatch，也不想使用Blocks语法，并且也不想启用Objective-C自带的异常机制，那么以下提供了一个比较精简的 `--objc-flags` 编译选项：
```bash
-MP -DGNUSTEP -DGNUSTEP_BASE_LIBRARY=1 -DGNUSTEP_RUNTIME=1 -D_NONFRAGILE_ABI=1 -DGNUSTEP_BASE_LIBRARY=1 -fno-strict-aliasing -pthread -fPIC -Wall -DGSWARN -DGSDIAGNOSE -Wno-import -fobjc-runtime=gnustep-1.9 -fconstant-string-class=NSConstantString
```

对于GNUstep所要连接的库，至少需要：`-lgnustep-base -lobjc -lm`。当然，这些我们都可以根据“tools-android”这一GitHub项目里的说明列出详细的编译选项与连接库信息。

对于需要包含的头文件路径以及动态库路径，我们需要针对不同的处理器架构做不同的路径指定。所以，比较稳妥的方式就是针对不同的处理器架构编译不同版本的GNUstep库。

最后，该GitHub里还提供了一个Android项目工程的demo，我们可以去观察它自带的CMakeLists.txt里的内容。

下面，笔者将给出 **sdkenv.sh** 中针对不同处理器架构的不同设置选项：

以下为ARMv7的设置（默认）：
```bash
export ABI_NAME=${ABI_NAME:-armeabi-v7a}
export ANDROID_API_LEVEL=${ANDROID_API_LEVEL:-21}
export ANDROID_TARGET=${ANDROID_TARGET:-armv7a-linux-androideabi}
export ANDROID_TARGET_BINUTILS=${ANDROID_TARGET_BINUTILS:-arm-linux-androideabi}
export BUILD_TYPE=${BUILD_TYPE:-Release}
```

以下为ARM64的设置：
```bash
export ABI_NAME=${ABI_NAME:-arm64-v8a}
export ANDROID_API_LEVEL=${ANDROID_API_LEVEL:-21}
export ANDROID_TARGET=${ANDROID_TARGET:-aarch64-linux-android}
export ANDROID_TARGET_BINUTILS=${ANDROID_TARGET_BINUTILS:-aarch64-linux-android}
export BUILD_TYPE=${BUILD_TYPE:-Release}
```

以下为x86_64的设置：
```bash
export ABI_NAME=${ABI_NAME:-x86_64}
export ANDROID_API_LEVEL=${ANDROID_API_LEVEL:-21}
export ANDROID_TARGET=${ANDROID_TARGET:-x86_64-linux-android}
export ANDROID_TARGET_BINUTILS=${ANDROID_TARGET_BINUTILS:-x86_64-linux-android}
export BUILD_TYPE=${BUILD_TYPE:-Release}
```

最后，笔者将给出完整的运用于Android Studio项目工程中的CMakeLists.txt文件。这里假定当前C语言工程模块名为默认的native-lib，并且有一个名为“objc_test.m”的Objective-C源文件和一个“native-lib.c”的C源文件。

```cmake
# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

include(AndroidNdkModules)
android_ndk_import_module_cpufeatures()

# Define list of Objective-C files
set(OBJECTIVEC_SRCS objc_test.m)

# Set up Objective-C suppport via GNUstep according to the current CPU architecture
if(${ANDROID_ABI} STREQUAL "arm64-v8a")
    set(CMAKE_C_FLAGS "-march=armv8.4a ${CMAKE_C_FLAGS}")
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

```

以上的cmake脚本代码中，**`$ENV{HOME}`** 表示当前用户根目录，即： **`~`** 。
最后一段从Android Studio库里的GNUstep中将lib目录下的所有.so文件全都拷贝到输出的cmake目录下，这个cmake输出目录在Android Studio 3里是在当前工程目录下的`/app/build/intermediates/cmake`目录之中。

下面给出objc_test.m源文件的内容：
```objectivec
#import <Foundation/Foundation.h>
#include <syslog.h>


void ObjCTest(void)
{
    @autoreleasepool {
        NSArray<NSNumber*> *array = @[ @10, @20, @30 ];

        int sum = 0;
        for(NSNumber *num in array) {
            sum += num.intValue;
        }

        NSString *logStr = [NSString stringWithFormat:@"array size is: %tu, sum = %d", array.count, sum];

        // 输出：array size is: 3, sum = 60
        syslog(LOG_INFO, "%s", logStr.UTF8String);
    }
}

```

这里各位要注意，Android系统下由于有自己的日志打印系统，因此我们不能直接用`NSLog` 函数，而只能用Android自带的打印函数，直接用 `NSLog` 将会引发应用崩溃。

