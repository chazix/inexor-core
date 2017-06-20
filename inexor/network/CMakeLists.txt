
# generate source file list
declare_module(network .)

# exclude any template file from the source list (it wont compile)
filter_from_module_lists("template" network)
add_lib(network)

include(require_run_gluegen.cmake)


# @param BUILD_FLAGS see require_run_gluegen
function(require_network targ BUILD_FLAGS)
  require_grpc(${targ})
  require_util(${targ})
  require_enet(${targ})

  # Folder where generated files land: somewhere in the build/ dir.
  set(GLUEGEN_OUT_DIR "${CMAKE_BINARY_DIR}/inexor/network/${targ}")
  set(network_dir "${SOURCE_DIR}/network")
  require_run_gluegen(${targ} ${BUILD_FLAGS} ${network_dir} ${GLUEGEN_OUT_DIR})

  target_include_directories(${targ} PUBLIC ${GLUEGEN_OUT_DIR})
  target_link_libraries(${targ} module_network)
endfunction()

