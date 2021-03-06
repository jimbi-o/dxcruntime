include(FetchContent)

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        15ac7b08f7c36e475956d7e34b985e3d93116c9e
)
FetchContent_MakeAvailable(spdlog)
target_compile_definitions(spdlog PUBLIC SPDLOG_WCHAR_TO_UTF8_SUPPORT)

FetchContent_Declare(
  doctest
  GIT_REPOSITORY https://github.com/onqtam/doctest.git
  GIT_TAG        f1e1c1b8d2d73e64359c45e0eec67eff4fdcc099
)
FetchContent_MakeAvailable(doctest)

FetchContent_Declare(
  dxc
  GIT_REPOSITORY https://github.com/microsoft/DirectXShaderCompiler.git
  GIT_TAG        5c4e942e8692bf1cf467c3719f3d93f77ea6c540
  )
FetchContent_GetProperties(dxc)
if(NOT dxc_POPULATED)
  FetchContent_Populate(dxc)
endif()

if(NOT DXCRUNTIME_STANDALONE)
  add_library(dxcruntime)
  target_compile_definitions(dxcruntime PRIVATE DOCTEST_CONFIG_DISABLE)
else()
  add_executable(dxcruntime)
endif()

target_sources(dxcruntime PRIVATE
  "dxcruntime.cpp"
)
if(DXCRUNTIME_STANDALONE)
  target_sources(dxcruntime PRIVATE
	"test.cpp"
  )
endif()

target_include_directories(dxcruntime
  PUBLIC
  "../include"
  "${dxc_SOURCE_DIR}/include"
  PRIVATE
  "./"
)

target_link_libraries(dxcruntime PRIVATE "spdlog" doctest::doctest)
target_compile_features(dxcruntime PUBLIC cxx_std_17)

if(DXCRUNTIME_STANDALONE)
  set(DXCOMPILERDLL
	"C:\\Program\ Files\ \(x86\)\\Windows\ Kits\\10\\bin\\${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}\\${CMAKE_VS_PLATFORM_NAME}\\dxcompiler.dll"
	)
  set(DXILDLL
	"C:\\Program\ Files\ \(x86\)\\Windows\ Kits\\10\\bin\\${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}\\${CMAKE_VS_PLATFORM_NAME}\\dxil.dll"
	)
  set(DLL_TARGET_DIR "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}")
  add_custom_command(
	TARGET dxcruntime POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	${DXCOMPILERDLL}
	${DLL_TARGET_DIR}
	)
  add_custom_command(
	TARGET dxcruntime POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	${DXILDLL}
	${DLL_TARGET_DIR}
	)
  add_custom_command(
	TARGET dxcruntime POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_CURRENT_SOURCE_DIR}/shader
	${CMAKE_CURRENT_BINARY_DIR}/shader
	)
endif()
