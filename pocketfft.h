#ifndef POCKETFFT_H_
#define POCKETFFT_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

///
/// Locally patched (see git log in this submodule) from the upstream
/// syoyo/standalone-pocketfft wrapper:
///  - all math is `float`, not `double` -- the RP2350/Cortex-M33 target has a
///    single-precision-only hardware FPU, so `double` here would silently
///    fall back to (much slower) software emulation.
///  - the plan-based real-FFT API (below) is exposed instead of the original
///    one-shot `rfft_forward_1d_array`/`cfft_backward_1d_array` wrappers,
///    which allocated and threw away a fresh plan (recomputing twiddle
///    tables) on every single call. Callers here are expected to create one
///    plan per fixed FFT length at startup and reuse it for the life of the
///    program.
///

/// Opaque real-FFT plan for a fixed transform length.
typedef struct rfft_plan_i * rfft_plan;

/// Create a real-FFT plan for the given length. Returns NULL on failure
/// (invalid length or allocation failure). Longer-lived: create once per
/// fixed size and reuse across calls, then destroy_rfft_plan() at shutdown.
rfft_plan make_rfft_plan(size_t length);

/// Free a plan created by make_rfft_plan().
void destroy_rfft_plan(rfft_plan plan);

///
/// Forward real FFT, in place, using FFTPACK's packed real-spectrum layout:
/// c[0]                  = Re(bin 0)      (DC, purely real)
/// c[2*k-1], c[2*k]      = Re(bin k), Im(bin k)   for k = 1 .. length/2 - 1
/// c[length-1]           = Re(bin length/2) (Nyquist, purely real, length even)
///
/// @param[in,out] c   length-element buffer: real signal in, packed spectrum out.
/// @param[in] fct      Scale factor applied to the result (1.0 for unnormalized).
/// @return 0 on success, nonzero on failure.
///
int rfft_forward(rfft_plan plan, float c[], float fct);

///
/// Inverse of rfft_forward: takes the same packed real-spectrum layout and
/// produces the real signal, in place.
///
/// @param[in,out] c   length-element buffer: packed spectrum in, real signal out.
/// @param[in] fct      Scale factor applied to the result (1.0/length to invert
///                     an unnormalized rfft_forward exactly).
/// @return 0 on success, nonzero on failure.
///
int rfft_backward(rfft_plan plan, float c[], float fct);

#ifdef __cplusplus
}
#endif

#endif // POCKETFFT_H_
