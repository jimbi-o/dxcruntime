get_target_property(target_type dxcruntime TYPE)
if(NOT target_type STREQUAL "EXECUTABLE")
  add_executable(launcher)
  target_sources(launcher PRIVATE
	"main.cpp"
  )
  target_link_libraries(launcher PRIVATE dxcruntime)
  target_compile_features(launcher PRIVATE cxx_std_17)
endif()
