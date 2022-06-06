
cmake_minimum_required (VERSION 3.4)
project (WinFontInstaller)



set(LINK_LIBS
	kernel32.lib
	user32.lib
	gdi32.lib
	advapi32.lib
	shell32.lib
)

set(DEBUG_LINK_LIBS
	vcruntimed.lib
	ucrtd.lib
)

set(SOURCE_FILES
	ReadFontName.c
	ReadFontName.h
	ReadFontNameInternal.h
	WinFontInstall.c
)

set(LIB_SOURCE_FILES
	libs/GetArgv.c
	libs/GetArgv.h
	libs/SimpleString.c
	libs/SimpleString.h
	libs/StringIntrinsics.h
	libs/SwapIntrinsics.h
)

set(RESOURCE_FILES
	WinFontInstall.rc
)


# set(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
# set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /MANIFEST:NO")
# set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /MANIFEST:NO")
set(CMAKE_C_FLAGS_DEBUG "/nologo /c /MDd /Zi /Ob0 /Od /RTC1 /W3 /DWIN32 /D_UNICODE /DUNICODE")
set(CMAKE_C_FLAGS "/nologo /c /MD /W3 /GF /GR- /EHs-c- /O1 /DWIN32 /DNDEBUG /D_UNICODE /DUNICODE")

add_executable (${PROJECT_NAME} ${SOURCE_FILES} ${LIB_SOURCE_FILES} ${RESOURCE_FILES})
target_link_libraries (${PROJECT_NAME} debug ${LINK_LIBS} ${DEBUG_LINK_LIBS})
target_link_libraries (${PROJECT_NAME} optimized ${LINK_LIBS})
target_include_directories(${PROJECT_NAME} PRIVATE libs)
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS -nodefaultlibs)
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "WinFontInstaller-x$(PlatformArchitecture)")
