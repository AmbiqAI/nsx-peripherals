if(NOT DEFINED NSX_BOARD_FLAGS_TARGET)
    message(FATAL_ERROR "Board must define NSX_BOARD_FLAGS_TARGET before ns-peripherals is added.")
endif()
if(NOT DEFINED NSX_SOC_FAMILY)
    message(FATAL_ERROR "NSX_SOC_FAMILY must be defined before ns-peripherals is added.")
endif()

set(NSX_PERIPHERALS_ARCH_DIR "")
if(NSX_SOC_FAMILY STREQUAL "apollo3" OR NSX_SOC_FAMILY STREQUAL "apollo3p")
    set(NSX_PERIPHERALS_ARCH_DIR "apollo3")
elseif(NSX_SOC_FAMILY STREQUAL "apollo4l" OR NSX_SOC_FAMILY STREQUAL "apollo4p")
    set(NSX_PERIPHERALS_ARCH_DIR "apollo4")
elseif(NSX_SOC_FAMILY STREQUAL "apollo5b" OR NSX_SOC_FAMILY STREQUAL "apollo510" OR NSX_SOC_FAMILY STREQUAL "apollo510b")
    set(NSX_PERIPHERALS_ARCH_DIR "apollo5")
elseif(NSX_SOC_FAMILY STREQUAL "apollo330P")
    set(NSX_PERIPHERALS_ARCH_DIR "apollo330")
else()
    message(FATAL_ERROR "Unsupported NSX_SOC_FAMILY for ns-peripherals: ${NSX_SOC_FAMILY}")
endif()

file(GLOB nsx_peripherals_common_sources CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_LIST_DIR}/src/*.c
    ${CMAKE_CURRENT_LIST_DIR}/src/*.cc
    ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp
)
file(GLOB nsx_peripherals_arch_sources CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_LIST_DIR}/src/${NSX_PERIPHERALS_ARCH_DIR}/*.c
    ${CMAKE_CURRENT_LIST_DIR}/src/${NSX_PERIPHERALS_ARCH_DIR}/*.cc
    ${CMAKE_CURRENT_LIST_DIR}/src/${NSX_PERIPHERALS_ARCH_DIR}/*.cpp
)

list(FILTER nsx_peripherals_common_sources EXCLUDE REGEX "/ns_power\\.(c|cc|cpp)$")
list(FILTER nsx_peripherals_arch_sources EXCLUDE REGEX "/ns_power\\.(c|cc|cpp)$")
list(FILTER nsx_peripherals_arch_sources EXCLUDE REGEX "/ns_tempco\\.(c|cc|cpp)$")

add_library(nsx_peripherals STATIC
    ${nsx_peripherals_common_sources}
    ${nsx_peripherals_arch_sources}
)
set_target_properties(nsx_peripherals PROPERTIES EXPORT_NAME peripherals)
add_library(nsx::peripherals ALIAS nsx_peripherals)

target_link_libraries(nsx_peripherals PUBLIC ${NSX_BOARD_FLAGS_TARGET} nsx_soc_hal nsx_runtime_core)

target_include_directories(nsx_peripherals PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/includes-api>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src/${NSX_PERIPHERALS_ARCH_DIR}>
    $<BUILD_INTERFACE:${NSX_ROOT}/modules/nsx-core/includes-api>
    $<BUILD_INTERFACE:${NSX_ROOT}/modules/nsx-harness/includes-api>
    $<BUILD_INTERFACE:${NSX_ROOT}/modules/nsx-utils/includes-api>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/nsx/modules/nsx-peripherals/includes-api>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/nsx/modules/nsx-peripherals/src/${NSX_PERIPHERALS_ARCH_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/nsx/modules/nsx-core/includes-api>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/nsx/modules/nsx-harness/includes-api>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/nsx/modules/nsx-utils/includes-api>
)

install(DIRECTORY
    ${CMAKE_CURRENT_LIST_DIR}/includes-api
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nsx/modules/nsx-peripherals
)
install(DIRECTORY
    ${CMAKE_CURRENT_LIST_DIR}/src/${NSX_PERIPHERALS_ARCH_DIR}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nsx/modules/nsx-peripherals/src
)

install(TARGETS
    nsx_peripherals
    EXPORT nsxTargets
)
