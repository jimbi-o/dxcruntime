include(FetchContent)
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        aa65dd89053bea0b7e0ffc8d9be4f152d5c01f44
  GIT_SHALLOW    ON
)
FetchContent_MakeAvailable(spdlog)
FetchContent_Declare(
  doctest
  GIT_REPOSITORY https://github.com/onqtam/doctest.git
  GIT_TAG        f13a00cc27ed3c1ec4f755572ab7556c4cb01716
  GIT_SHALLOW    ON
)
FetchContent_MakeAvailable(doctest)
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
)
target_link_libraries(dxcruntime PRIVATE "spdlog" doctest::doctest)
target_compile_features(dxcruntime PUBLIC cxx_std_17)
