#ifndef CANOPY_TIMER_H
#define CANOPY_TIMER_H

#include <stdint.h>

/// @file canopy_time.h
/// @brief High-precision timer and frame limiter for Canopy applications.


//----------------------------------------
// Initialization
//----------------------------------------

/// @brief Initializes the high-resolution timer system.
///
/// Sets up internal timebase conversion and initializes the frame timer.
/// Must be called before any other time-related functions.
void canopy_init_timer(void);


//----------------------------------------
// Time Queries
//----------------------------------------

/// @brief Returns the current time in seconds (high precision).
///
/// Time is returned as a `double` representing seconds since an arbitrary
/// fixed point (e.g., application launch).
///
/// @return Current time in seconds.
double canopy_get_time(void);

/// @brief Returns the current time in nanoseconds.
///
/// Useful for ultra-fine timing measurements (e.g., profiling).
///
/// @return Current time in nanoseconds.
uint64_t canopy_get_time_ns(void);


//----------------------------------------
// Frame Timing Control
//----------------------------------------

/// @brief Sets the target frame rate for rendering.
///
/// By default, this is set to 60 FPS. Setting FPS below 1 will clamp to 1.
///
/// @param fps Target frames per second (minimum 1).
void canopy_set_fps(int fps);

/// @brief Gets the currently configured target FPS.
///
/// @return The current FPS setting.
int canopy_get_fps(void);

/// @brief Determines whether enough time has passed to render the next frame.
///
/// Internally checks elapsed time since the last frame and updates the internal
/// frame timestamp if rendering is allowed.
///
/// @return `1` if rendering should occur, `0` if the frame should be skipped.
int canopy_should_render_frame(void);

#endif // CANOPY_TIMER_H
