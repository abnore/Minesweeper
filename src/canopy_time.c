#include "canopy_time.h"

#include <mach/mach_time.h>
#include <inttypes.h>  // For PRIu64
#include <blackbox.h>

#define DEFAULT_FPS 60

static struct {
    mach_timebase_info_data_t timebase;
    double to_seconds;

    double target_frame_time;     // Time between frames (in seconds)
    double last_frame_time;       // Timestamp of last rendered frame
    double delta_time;
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

    canopy_timer.target_frame_time = 1.f / DEFAULT_FPS;  // Default to 60 FPS
    canopy_timer.last_frame_time = canopy_get_time();  // Initialize clock

    INFO("Timer initialized");
    DEBUG("Timer conversion factor: %.12f", canopy_timer.to_seconds);
    TRACE("Default target frame time: %.6f seconds, %d FPS", canopy_timer.target_frame_time, DEFAULT_FPS);
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

double canopy_get_delta_time(void)
{
    return canopy_timer.delta_time;
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
        canopy_timer.delta_time = elapsed;
        canopy_timer.last_frame_time = now;
       // TRACE("Render allowed (elapsed %.6f >= %.6f)", elapsed, canopy_timer.target_frame_time);
        return 1;
    }

    //TRACE("Skipping frame (elapsed %.6f < %.6f)", elapsed, canopy_timer.target_frame_time);
    return 0;
}


void canopy_sleep_until_next_frame(void)
{
    uint64_t now = mach_absolute_time();
    double elapsed = (now * canopy_timer.to_seconds) - canopy_timer.last_frame_time;
    double remaining = canopy_timer.target_frame_time - elapsed;

    if (remaining > 0) {
        uint64_t wait_until = now + (uint64_t)(remaining / canopy_timer.to_seconds);
        mach_wait_until(wait_until);
    }
}
