add_library(pocketfft STATIC
    pocketfft.c
)

target_include_directories(pocketfft PUBLIC ${CMAKE_CURRENT_LIST_DIR})

# The hot path (FFT butterflies) is scalar float arithmetic with no
# library calls; -O2 lets GCC use the Cortex-M33's single-precision FPU
# directly instead of soft-float routines.
target_compile_options(pocketfft PRIVATE -O2)
