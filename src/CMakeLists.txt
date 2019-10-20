include(FetchContent)

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        1472048b974391905de2ec6779d3206be8753fe0
  GIT_SHALLOW    ON
)
FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(
  doctest
  GIT_REPOSITORY https://github.com/onqtam/doctest.git
  GIT_TAG        7ac22cc2190eb090ff66509015fb2d995bce957e
  GIT_SHALLOW    ON
)
FetchContent_MakeAvailable(doctest)

FetchContent_Declare(
  dxc
  GIT_REPOSITORY https://github.com/microsoft/DirectXShaderCompiler.git
  GIT_TAG        a6a28a34bc78e1a629ad79294ac1c16eda789dc9
  GIT_SHALLOW    ON
  )
FetchContent_GetProperties(dxc)
if(NOT dxc_POPULATED)
  FetchContent_Populate(dxc)
endif()

if(NOT LIB_TEST_MODE)
  add_library(dxcruntime)
  target_compile_definitions(dxcruntime PRIVATE DOCTEST_CONFIG_DISABLE)
else()
  add_executable(dxcruntime)
endif()

target_sources(dxcruntime PRIVATE
  "dxcruntime.cpp"
)
if(LIB_TEST_MODE)
  target_sources(dxcruntime PRIVATE
	"test.cpp"
  )
endif()

target_include_directories(dxcruntime
  PUBLIC
  "../include"
  PRIVATE
  "./"
  "${dxc_SOURCE_DIR}/include"
)

target_link_libraries(dxcruntime PRIVATE "spdlog" doctest::doctest)
target_compile_features(dxcruntime PUBLIC cxx_std_17)
#TODO copy dlls
