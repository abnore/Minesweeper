#include "canopy_time.h"
#include "logger/logger.h"  // Assuming your logger setup

#include <mach/mach_time.h>
#include <inttypes.h>  // For PRIu64

static struct {
    mach_timebase_info_data_t timebase;
    double to_seconds;

    double target_frame_time;     // Time between frames (in seconds)
    double last_frame_time;       // Timestamp of last rendered frame
} canopy_timer;

void canopy_init_timer(void)
{
    kern_return_t result = mach_timebase_info(&canopy_timer.timebase);

    if (result != KERN_SUCCESS) {
        FATAL("Failed to initialize mach timebase");
        return;
    }

    canopy_timer.to_seconds = ((double)canopy_timer.timebase.numer /
                               (double)canopy_timer.timebase.denom) / 1e9;

    canopy_timer.target_frame_time = 1.0 / 60.0;  // Default to 60 FPS
    canopy_timer.last_frame_time = canopy_get_time();  // Initialize clock

    TRACE("Timer initialized");
    DEBUG("Timer conversion factor: %.12f", canopy_timer.to_seconds);
    DEBUG("Default target frame time: %.6f seconds", canopy_timer.target_frame_time);
}

double canopy_get_time(void)
{
    uint64_t ticks = mach_absolute_time();
    double seconds = (double)ticks * canopy_timer.to_seconds;

    //TRACE("Get time: %.6f seconds", seconds);
    return seconds;
}

uint64_t canopy_get_time_ns(void)
{
    uint64_t ticks = mach_absolute_time();
    uint64_t ns = ticks * canopy_timer.timebase.numer / canopy_timer.timebase.denom;

   // TRACE("Get time: %" PRIu64 " nanoseconds", ns);
    return ns;
}

void canopy_set_fps(int fps)
{
    if (fps < 1) {
        WARN("FPS below 1 specified, clamping to 1");
        fps = 1;
    }

    canopy_timer.target_frame_time = 1.0 / (double)fps;
    INFO("Target FPS set to %d (%.6f s per frame)", fps, canopy_timer.target_frame_time);
}

int canopy_get_fps(void)
{
    int fps = (int)(1.0 / canopy_timer.target_frame_time);
    TRACE("Current FPS setting: %d", fps);
    return fps;
}

int canopy_should_render_frame(void)
{
    double now = canopy_get_time();
    double elapsed = now - canopy_timer.last_frame_time;

    if (elapsed >= canopy_timer.target_frame_time) {
        canopy_timer.last_frame_time = now;
       // TRACE("Render allowed (elapsed %.6f >= %.6f)", elapsed, canopy_timer.target_frame_time);
        return 1;
    }

    //TRACE("Skipping frame (elapsed %.6f < %.6f)", elapsed, canopy_timer.target_frame_time);
    return 0;
}

