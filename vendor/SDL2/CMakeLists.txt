# Define target name
set (TARGET_NAME SDL)

include_directories(include)

# Define source files
file (GLOB C_FILES
    src/*.c src/atomic/*.c src/audio/*.c src/audio/disk/*.c src/audio/dummy/*.c src/cpuinfo/*.c src/events/*.c src/file/*.c
    src/haptic/*.c src/joystick/*.c src/libm/*.c src/power/*.c src/stdlib/*.c 
    src/thread/*.c src/timer/*.c src/video/*.c src/video/dummy/*.c
    src/render/*.c src/render/opengl/*.c src/render/*.c src/render/opengl/*.c src/render/software/*.c 
)

if (WIN32)
    # New MinGW versions may evaluate whether to use A or W postfix on functions before SDL gets to define UNICODE on its own,
    # so make sure it is already defined
    if (MINGW)
        add_definitions (-DUNICODE=1)
    endif ()
    CHECK_INCLUDE_FILES (wbemcli.h HAVE_WBEMCLI_H)
    if (HAVE_WBEMCLI_H)
        add_definitions (-DSDL_JOYSTICK_DINPUT)
        add_definitions (-DSDL_HAPTIC_DINPUT)
        set (HAPTIC_DRIVER windows)
    else ()
        message (STATUS "Building SDL without DX joystick support due to missing wbemcli.h")
        message (STATUS "For MSVC, get it from Windows 7 SDK. For MinGW, get it from eg. Wine sources or from MinGW-w64")
        add_definitions (-DSDL_JOYSTICK_WINMM)
        add_definitions (-DSDL_HAPTIC_DUMMY)
        set (HAPTIC_DRIVER dummy)
    endif ()
    file (GLOB SYS_C_FILES
        src/audio/directsound/*.c src/haptic/${HAPTIC_DRIVER}/*.c src/joystick/windows/*.c src/core/windows/*.c src/loadso/windows/*.c
        src/power/windows/*.c src/thread/windows/*.c src/thread/generic/SDL_syscond.c src/timer/windows/*.c src/video/windows/*.c src/joystick/dummy/*.c
        src/filesystem/windows/*.c
        src/main/windows/*.c
        src/render/direct3d/*.c
    )
elseif (IOS)
    file (GLOB SYS_C_FILES
        src/audio/coreaudio/*.c src/file/cocoa/*.m src/joystick/iphoneos/*.m src/loadso/dlopen/*.c src/power/uikit/*.m
        src/thread/pthread/*.c src/timer/unix/*.c src/video/uikit/*.m src/video/uikit/*.c src/haptic/dummy/*.c
        src/filesystem/cocoa/*.m
    )
elseif (APPLE)
    file (GLOB SYS_C_FILES
        src/audio/coreaudio/*.c src/file/cocoa/*.m src/haptic/darwin/*.c src/joystick/darwin/*.c src/loadso/dlopen/*.c
        src/power/macosx/*.c src/thread/pthread/*.c src/timer/unix/*.c src/video/cocoa/*.m src/filesystem/cocoa/*.m
    )
elseif (ANDROID)
    file (GLOB SYS_C_FILES
        src/audio/android/*.c src/core/android/*.c src/joystick/android/*.c src/loadso/dlopen/*.c src/power/android/*.c
        src/thread/pthread/*.c src/timer/unix/*.c src/video/android/*.c src/haptic/dummy/*.c src/filesystem/dummy/*.c
    )
else ()
    # On Linux, make sure development libraries exist, and use their include file directories  
    if (RASPI)
        file (GLOB VIDEO_DRIVER_C_FILES src/video/raspberry/*.c src/core/linux/*.c src/input/evdev/*.c)
    else ()
        find_package (OpenGL REQUIRED)
        include_directories (${OpenGL_INCLUDE_DIRS})
        
        find_package (X11 REQUIRED)
        if (NOT X11_Xrandr_FOUND)
            message (FATAL_ERROR "Could not find XRandR (libxrandr-dev)")
        endif ()
        include_directories (${X11_INCLUDE_DIRS})  

        if (NOT DEFINED HAVE_CONST_XEXT_ADDDISPLAY)
            message (STATUS "Following tests check whether X11 library installed in this system uses _Xconst in below functions")
            message (STATUS "A failed test result simply means the installed X11 library does not use _Xconst")
            message (STATUS "It is OK to proceed to build Urho3D regardless of the test result")
        endif ()
        include (CheckCSourceCompiles)
        set  (CMAKE_REQUIRED_LIBRARIES ${X11_LIB} ${X11_LIB})
        check_c_source_compiles ("
            #include <X11/Xlib.h>
            #include <X11/Xproto.h>
            #include <X11/extensions/Xext.h>
            #include <X11/extensions/extutil.h>
            extern XExtDisplayInfo* XextAddDisplay(XExtensionInfo* a,Display* b,_Xconst char* c,XExtensionHooks* d,int e,XPointer f);
            int main(int argc, char **argv) {}" HAVE_CONST_XEXT_ADDDISPLAY)
        if (HAVE_CONST_XEXT_ADDDISPLAY)
            add_definitions (-DSDL_VIDEO_DRIVER_X11_CONST_PARAM_XEXTADDDISPLAY)
        endif ()
        check_c_source_compiles ("
            #include <X11/Xlibint.h>
            extern int _XData32(Display *dpy,register _Xconst long *data,unsigned len);
            int main(int argc, char **argv) {}" HAVE_CONST_XDATA32)
        if (HAVE_CONST_XDATA32)
            add_definitions (-DSDL_VIDEO_DRIVER_X11_CONST_PARAM_XDATA32)
        endif ()
        set (CMAKE_REQUIRED_LIBRARIES)
        
        file (GLOB VIDEO_DRIVER_C_FILES src/video/x11/*.c)
    endif ()
    find_package (ALSA REQUIRED)
    include_directories (${ALSA_INCLUDE_DIRS})

    file (GLOB SYS_C_FILES
        src/audio/alsa/*.c src/audio/dma/*.c src/audio/dsp/*.c src/haptic/linux/*.c src/joystick/linux/*.c src/loadso/dlopen/*.c
        src/power/linux/*.c src/thread/pthread/*.c src/timer/unix/*.c src/filesystem/unix/*.c
    )
    set (SYS_C_FILES ${SYS_C_FILES} ${VIDEO_DRIVER_C_FILES})
endif ()

file (GLOB H_FILES include/*.h)

# Define source files
set_source_files_properties (${C_FILES} PROPERTIES LANGUAGE C)
set_source_files_properties (${SYS_C_FILES} PROPERTIES LANGUAGE C)
set (SOURCE_FILES ${C_FILES} ${SYS_C_FILES} ${H_FILES})
if (ANDROID)
    # Install dependency for SDL_android_main.c
    install (DIRECTORY include/ DESTINATION ${DEST_INCLUDE_DIR}/SDL USE_SOURCE_PERMISSIONS FILES_MATCHING PATTERN *.h)     # Note: the trailing slash is significant
    install (FILES src/main/android/SDL_android_main.c DESTINATION ${DEST_RUNTIME_DIR}/../templates/android)
endif ()

add_library(SDL2 STATIC ${SOURCE_FILES})
