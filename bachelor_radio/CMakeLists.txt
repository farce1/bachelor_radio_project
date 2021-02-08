cmake_minimum_required(VERSION 3.2)

project(Bachelor_Radio)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)

option(BUILD_BACHELOR_RADIO_APP    "Build Bachelor_Radio_APP"                        ON  )
option(WITH_APP_BUNDLE   "Enable Application Bundle for macOS"   OFF  )
option(KISS_FFT          "KISS FFT instead of FFTW"              OFF )
option(PROFILING         "Enable profiling (see README.md)"      OFF )
option(AIRSPY            "Compile with Airspy support"           OFF )
option(RTLSDR            "Compile with RTL-SDR support"          OFF )
option(SOAPYSDR          "Compile with SoapySDR support"         OFF )

add_definitions(-Wall)
add_definitions(-g)
add_definitions(-DDABLIN_AAC_FAAD2)

if(MINGW)
    add_definitions(-municode)
endif()

set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "")

#enable_testing()

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake/Modules)

if(APPLE AND WITH_APP_BUNDLE)
    if(NOT DEFINED BUNDLE_INSTALL_DIR)
        set(BUNDLE_INSTALL_DIR "/Applications")
    endif()

    set(GUI_INSTALL_DIR "${BUNDLE_INSTALL_DIR}")
else()
    include(GNUInstallDirs)

    set(GUI_INSTALL_DIR "${CMAKE_INSTALL_BINDIR}")
endif()

if(BUILD_BACHELOR_RADIO_APP)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)

    find_package(Qt5 COMPONENTS Widgets Quick QuickControls2 Multimedia Charts Qml REQUIRED)
endif()

if(PROFILING)
    add_definitions(-DWITH_PROFILING)
endif()

find_package(ALSA)
if(ALSA_FOUND)
    add_definitions(-DHAVE_ALSA)
endif()

if(KISS_FFT)
    add_definitions(-DKISSFFT)
    set(fft_sources src/libs/kiss_fft/kiss_fft.c)
    set(KISS_INCLUDE_DIRS src/libs/kiss_fft)
else()
    find_package(FFTW3f REQUIRED)
    set(fft_sources "")
    set(KISS_INCLUDE_DIRS "")
endif()

find_package(Threads REQUIRED)
find_package(Faad REQUIRED)
find_package(MPG123 REQUIRED)

if (RTLSDR)
    find_package(LibRTLSDR REQUIRED)
endif()

if (AIRSPY)
    find_package(LibAIRSPY REQUIRED)
endif()

if (SOAPYSDR)
  find_package(SoapySDR NO_MODULE REQUIRED)
  # Note: SoapySDRConfig.cmake sets C++11 standard so it needs to be reset to C++14
  set(CMAKE_CXX_STANDARD 14)
endif()

include_directories(
    src
    src/backend
    src/output
    src/various
    src/input
    src/gui
    src/libs/fec
    ${FFTW3F_INCLUDE_DIRS}
    ${KISS_INCLUDE_DIRS}
    ${FAAD_INCLUDE_DIRS}
    ${LIBRTLSDR_INCLUDE_DIRS}
    ${SoapySDR_INCLUDE_DIRS}
)

set(backend_sources
    src/backend/dab-audio.cpp
    src/backend/decoder_adapter.cpp
    src/backend/dab_decoder.cpp
    src/backend/dabplus_decoder.cpp
    src/backend/charsets.cpp
    src/backend/dab-constants.cpp
    src/backend/mot_manager.cpp
    src/backend/pad_decoder.cpp
    src/backend/eep-protection.cpp
    src/backend/fib-processor.cpp
    src/backend/fic-handler.cpp
    src/backend/msc-handler.cpp
    src/backend/freq-interleaver.cpp
    src/backend/ofdm-decoder.cpp
    src/backend/ofdm-processor.cpp
    src/backend/phasereference.cpp
    src/backend/phasetable.cpp
    src/backend/tii-decoder.cpp
    src/backend/protTables.cpp
    src/backend/radio-receiver.cpp
    src/backend/tools.cpp
    src/backend/uep-protection.cpp
    src/backend/viterbi.cpp
    src/various/Socket.cpp
    src/various/Xtan2.cpp
    src/various/channels.cpp
    src/various/fft.cpp
    src/various/profiling.cpp
    src/various/wavfile.c
    src/libs/fec/decode_rs_char.c
    src/libs/fec/encode_rs_char.c
    src/libs/fec/init_rs_char.c
)

set(gui_sources
    src/gui/main.cpp
    src/gui/audio_output.cpp
    src/gui/mot_image_provider.cpp
    src/gui/gui_helper.cpp
    src/gui/radio_controller.cpp
    src/gui/debug_output.cpp
    src/gui/waterfallitem.cpp
)

set(input_sources
    src/input/input_factory.cpp
    src/input/null_device.cpp
    src/input/raw_file.cpp
    src/input/rtl_tcp.cpp
)

if(LIBRTLSDR_FOUND)
    add_definitions (-DHAVE_RTLSDR)
    set(input_sources  ${input_sources} src/input/rtl_sdr.cpp)
endif()

if(LIBAIRSPY_FOUND)
    add_definitions (-DHAVE_AIRSPY)
    set(input_sources  ${input_sources} src/input/airspy_sdr.cpp)
endif()

if(SoapySDR_FOUND)
    add_definitions (-DHAVE_SOAPYSDR)
    set(input_sources  ${input_sources} src/input/soapy_sdr.cpp)
endif()

if(BUILD_BACHELOR_RADIO_APP)
    set(executableName gui)
    add_executable (${executableName} ${gui_sources} ${backend_sources} ${input_sources} ${fft_sources} ${EXTRA_MOCS} src/gui/resources.qrc)

    target_link_libraries (
      ${LIBRTLSDR_LIBRARIES}
      ${LIBAIRSPY_LIBRARIES}
      ${FFTW3F_LIBRARIES}
      ${FAAD_LIBRARIES}
      ${SoapySDR_LIBRARIES}
      ${MPG123_LIBRARIES}
      Threads::Threads
      Qt5::Core Qt5::Widgets Qt5::Multimedia Qt5::Charts Qt5::Qml Qt5::Quick Qt5::QuickControls2
    )
        INSTALL (TARGETS ${executableName} RUNTIME DESTINATION ${GUI_INSTALL_DIR})

            INSTALL (FILES ${PROJECT_SOURCE_DIR}/radio.desktop DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)
endif()


configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
