# PROJECT HORIZON
#
# HorizonOS CTR

cmake_minimum_required(VERSION 3.24)

project(CTRSDK C CXX ASM)


function(nn_library TARGET_NAME MODULE_DIR)
    file(GLOB_RECURSE MODULE_SOURCES
        "${MODULE_DIR}/*.cpp"
        "${MODULE_DIR}/*.c"
        "${MODULE_DIR}/*.s"
        "${MODULE_DIR}/*.asm"
    )

    if(NOT MODULE_SOURCES)
        message(STATUS "Skipping ${TARGET_NAME} (no sources)")
        return()
    endif()

    add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

    set_target_properties(${TARGET_NAME} PROPERTIES
        OUTPUT_NAME "lib${TARGET_NAME}"
        SUFFIX ".fast.a"
    )

    target_compile_options(${TARGET_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:
        --multibyte-chars --signed_chars --cpu=MPCore
        --dollar --littleend --wchar --wchar16
        --apcs=//interwork --data-reorder --split_sections
        --debug_info=line_inlining_extensions --force_new_nothrow --fpmode=fast --no_exceptions --rtti --rtti_data --remarks --no_vfe --no_allow_fpreg_for_nonfpdata
        --diag_suppress=3017,940,997,177,541,1,1134,550,83,427,2525,117,284,174,111,1441,161
        --cpp --arm -O3 -Otime
        -DNN_PROCESSOR_ARM11MPCORE -DNN_BUILD_RELEASE
        >
    )

    target_compile_options(${TARGET_NAME} PRIVATE $<$<COMPILE_LANGUAGE:C>:
        --multibyte-chars --signed_chars --cpu=MPCore
        --dollar --littleend --wchar --wchar16
        --apcs=//interwork --data-reorder --split_sections
        --debug_info=line_inlining_extensions --force_new_nothrow --fpmode=fast --no_exceptions --rtti --rtti_data --remarks --no_vfe --no_allow_fpreg_for_nonfpdata
        --diag_suppress=3017,940,997,177,541,1,1134,550,83,427,2525,117,284,174,111,1441,161,826
        --c99 --arm -O3 -Otime
        -DNN_PROCESSOR_ARM11MPCORE -DNN_BUILD_RELEASE
        >
    )

    target_include_directories(${TARGET_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

    target_compile_definitions(${TARGET_NAME} PUBLIC
        NON_MATCHING=
    )
endfunction()

nn_library(nn_applet  ${CMAKE_CURRENT_SOURCE_DIR}/src/nn/applet)
nn_library(nn_camera  ${CMAKE_CURRENT_SOURCE_DIR}/src/nn/camera)
nn_library(nn_cfg     ${CMAKE_CURRENT_SOURCE_DIR}/src/nn/cfg)
nn_library(nn_crt0    ${CMAKE_CURRENT_SOURCE_DIR}/src/nn/crt0)
nn_library(nn_crypto  ${CMAKE_CURRENT_SOURCE_DIR}/src/nn/crypto)
nn_library(nn_cx      ${CMAKE_CURRENT_SOURCE_DIR}/src/nn/cx)
nn_library(nn_dbg     ${CMAKE_CURRENT_SOURCE_DIR}/src/nn/dbg)
nn_library(nn_dbm     ${CMAKE_CURRENT_SOURCE_DIR}/src/nn/dbm)
nn_library(nn_dev     ${CMAKE_CURRENT_SOURCE_DIR}/src/nn/dev)
nn_library(nn_dsp     ${CMAKE_CURRENT_SOURCE_DIR}/src/nn/dsp)
nn_library(nn_err     ${CMAKE_CURRENT_SOURCE_DIR}/src/nn/err)
nn_library(nn_fnd     ${CMAKE_CURRENT_SOURCE_DIR}/src/nn/fnd)
nn_library(nn_font    ${CMAKE_CURRENT_SOURCE_DIR}/src/nn/font)
nn_library(nn_fs      ${CMAKE_CURRENT_SOURCE_DIR}/src/nn/fs)
nn_library(nn_fslow   ${CMAKE_CURRENT_SOURCE_DIR}/src/nn/fslow)
nn_library(nn_gr      ${CMAKE_CURRENT_SOURCE_DIR}/src/nn/gr)
nn_library(nn_gx      ${CMAKE_CURRENT_SOURCE_DIR}/src/nn/gx)
#nn_library(nn_gxlow   ${CMAKE_CURRENT_SOURCE_DIR}/src/nn/gxlow)
nn_library(nn_hid     ${CMAKE_CURRENT_SOURCE_DIR}/src/nn/hid)
nn_library(nn_hidlow  ${CMAKE_CURRENT_SOURCE_DIR}/src/nn/hidlow)
#nn_library(nn_hio     ${CMAKE_CURRENT_SOURCE_DIR}/src/nn/hio)
nn_library(nn_init    ${CMAKE_CURRENT_SOURCE_DIR}/src/nn/init)
nn_library(nn_ir      ${CMAKE_CURRENT_SOURCE_DIR}/src/nn/ir)
nn_library(nn_math    ${CMAKE_CURRENT_SOURCE_DIR}/src/nn/math)
#nn_library(nn_mic     ${CMAKE_CURRENT_SOURCE_DIR}/src/nn/mic)
nn_library(nn_ndm     ${CMAKE_CURRENT_SOURCE_DIR}/src/nn/ndm)
nn_library(nn_nstd    ${CMAKE_CURRENT_SOURCE_DIR}/src/nn/nstd)
nn_library(nn_os      ${CMAKE_CURRENT_SOURCE_DIR}/src/nn/os)
nn_library(nn_pl      ${CMAKE_CURRENT_SOURCE_DIR}/src/nn/pl)
nn_library(nn_ptm     ${CMAKE_CURRENT_SOURCE_DIR}/src/nn/ptm)
nn_library(nn_ro      ${CMAKE_CURRENT_SOURCE_DIR}/src/nn/ro)
nn_library(nn_rtport  ${CMAKE_CURRENT_SOURCE_DIR}/src/nn/rtport/rvct)
nn_library(nn_snd     ${CMAKE_CURRENT_SOURCE_DIR}/src/nn/snd)
nn_library(nn_srv     ${CMAKE_CURRENT_SOURCE_DIR}/src/nn/srv)
nn_library(nn_svc     ${CMAKE_CURRENT_SOURCE_DIR}/src/nn/svc)
nn_library(nn_ulcd    ${CMAKE_CURRENT_SOURCE_DIR}/src/nn/ulcd)
nn_library(nn_util    ${CMAKE_CURRENT_SOURCE_DIR}/src/nn/util)

add_library(CTRSDK INTERFACE)
add_library(gles2 STATIC IMPORTED GLOBAL)
add_library(gxlow STATIC IMPORTED GLOBAL)

set_target_properties(gles2 PROPERTIES
    IMPORTED_LOCATION
        "${CMAKE_CURRENT_SOURCE_DIR}/build/libgles2.fast.a"
)
set_target_properties(gxlow PROPERTIES
    IMPORTED_LOCATION
        "${CMAKE_CURRENT_SOURCE_DIR}/build/libnn_gxlow.fast.a"
)

target_link_libraries(CTRSDK INTERFACE
    nn_applet
    nn_camera
    nn_cfg
    nn_crt0
    nn_crypto
    nn_cx
    nn_dbg
    nn_dbm
    nn_dev
    nn_dsp
    nn_err
    nn_fnd
    nn_font
    nn_fs
    nn_fslow
#    nn_gx
#    nn_gxlow
    nn_hid
    nn_hidlow
#    nn_hio
    nn_init
    nn_ir
    nn_math
#    nn_mic
    nn_ndm
    nn_nstd
    nn_os
    nn_pl
    nn_ptm
    nn_ro
    nn_rtport
    nn_snd
    nn_srv
    nn_svc
    nn_ulcd
    nn_util

    gxlow
    gles2
)