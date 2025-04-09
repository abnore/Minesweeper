/*******************************************************************************
 * Canopy — Minimal Windowing & Input Library for macOS
 *
 * Main API for the Canopy library
 *
 * Author: Andreas Nore (abnore)
 * License: MIT
 *******************************************************************************/

#ifndef CANOPY_H
#define CANOPY_H

#ifdef __cplusplus
extern "C" { // Prevents name mangling of functions
#endif

#include "canopy_time.h"
#include "common.h"

/// @brief Available window style options for Canopy windows.
typedef enum {
    CANOPY_WINDOW_STYLE_BORDERLESS              = 0,
    CANOPY_WINDOW_STYLE_TITLED                  = 1,
    CANOPY_WINDOW_STYLE_CLOSABLE                = 1 << 1,
    CANOPY_WINDOW_STYLE_MINIATURIZABLE          = 1 << 2,
    CANOPY_WINDOW_STYLE_RESIZABLE               = 1 << 3,
    CANOPY_WINDOW_STYLE_UTILITY                 = 1 << 4,
    CANOPY_WINDOW_STYLE_MODAL                   = 1 << 6,
    CANOPY_WINDOW_STYLE_NONACTIVATING_PANEL     = 1 << 7,
    CANOPY_WINDOW_STYLE_UNIFIED_TITLE_TOOLBAR   = 1 << 12,
    CANOPY_WINDOW_STYLE_HUD                     = 1 << 13,
    CANOPY_WINDOW_STYLE_FULLSCREEN              = 1 << 14,
    CANOPY_WINDOW_STYLE_FULLSIZE_CONTENT        = 1 << 15,

    /// @brief Default window style (Titled, Closable, Miniaturizable, Resizable).
    CANOPY_WINDOW_STYLE_DEFAULT = CANOPY_WINDOW_STYLE_TITLED |
                                  CANOPY_WINDOW_STYLE_CLOSABLE |
                                  CANOPY_WINDOW_STYLE_MINIATURIZABLE |
                                  CANOPY_WINDOW_STYLE_RESIZABLE
}canopy_window_style;

/*
    The canopy_window is the main actor in this library,
    acting as the bridge between C and Objective-C.

    This is a forward declaration — effectively saying:

        “Hey compiler, canopy_window is a struct that exists somewhere.
        I don’t need to know what it looks like yet — just that it exists.”

    This allows:
        * Declaring pointers to it (canopy_window*)
        * Passing it to and returning it from functions

    But you cannot:
        * Dereference it
        * Access its members
        * Allocate it directly (its size is unknown)

    In other words: it's opaque on purpose. Treat it with respect.
*/
/// @brief Opaque window structure. Don't access directly.
typedef struct canopy_window canopy_window;

/// @brief Bytes per pixel in the framebuffer (RGBA = 4 bytes)
#define CANOPY_BYTES_PER_PIXEL 4

/// @brief Framebuffer structure for storing pixel data.
typedef struct {
    uint32_t    *pixels;      ///< Pixel buffer (RGBA).
    int         width;        ///< Width in pixels.
    int         height;       ///< Height in pixels.
    int         pitch;        ///< Number of bytes per row.
} framebuffer;


/// @name Window Lifecycle
/// @{
///
/// @brief Creates and shows a new window with the given size and title.
/// @param[in] width Width of the window in screen pixels.
/// @param[in] height Height of the window in screen pixels.
/// @param[in] title Window title (UTF-8 encoded C string).
/// @return A pointer to the newly created window, or NULL on failure.
canopy_window* canopy_create_window(const char* title,
                                    int width,
                                    int height,
                                    canopy_window_style flags);

/// @brief Frees and closes a previously created window.
/// @param[in,out] w The window to destroy.
void canopy_free_window(canopy_window* w);

/// @brief Sets the dock icon of the application (macOS only).
/// @param[in] filepath Path to an image file to use as the application icon.
void canopy_set_icon(const char* filepath);
bool canopy_is_window_opaque(canopy_window *win);
void canopy_set_window_transparent(canopy_window *win, bool enable);

/// @brief Checks if the window should close (user clicked the close button).
/// @param[in] w The window to query.
/// @return true if the window should close, false otherwise.
bool canopy_window_should_close(canopy_window* w);
/// @}


/// @name Framebuffer Management
/// @{
///
/// @brief Initializes the framebuffer attached to the given window.
/// @param[in,out] w The window to initialize the framebuffer for.
/// @return true if successful, false on failure.
bool canopy_init_framebuffer(canopy_window* w);

/// @brief Returns the framebuffer associated with the window.
/// @param[in] w The target window.
/// @return Pointer to the framebuffer structure.
framebuffer* canopy_get_framebuffer(canopy_window* w);

/// @brief Presents the contents of the framebuffer to the screen.
/// @param[in] w The target window.
void canopy_present_buffer(canopy_window* w);

/// @brief Swaps a custom backbuffer into the window's framebuffer.
/// @param[in,out] w The target window.
/// @param[in,out] backbuffer The backbuffer to swap. Contents may be overwritten.
void canopy_swap_backbuffer(canopy_window* w, framebuffer* backbuffer);
/// @}


/// @name Event Handling
/// @brief Supports polling and pushing input events.
///
/// @{
/// @brief Maximum number of simultaneous events stored in the internal event queue.
#define CANOPY_MAX_EVENTS 64

/// @brief Type of high-level events.
typedef enum {
    CANOPY_EVENT_NONE,
    CANOPY_EVENT_MOUSE,
    CANOPY_EVENT_KEY,
} canopy_event_type;

/// @brief Mouse-specific event actions.
typedef enum {
    CANOPY_MOUSE_NONE,
    CANOPY_MOUSE_PRESS,
    CANOPY_MOUSE_RELEASE,
    CANOPY_MOUSE_MOVE,
    CANOPY_MOUSE_DRAG,
    CANOPY_MOUSE_SCROLL,
    CANOPY_MOUSE_ENTER,
    CANOPY_MOUSE_EXIT
} canopy_mouse_action;

/// @brief Key-specific event actions.
typedef enum {
    CANOPY_KEY_NONE,
    CANOPY_KEY_PRESS,
    CANOPY_KEY_RELEASE
} canopy_key_action;

/// @brief Mouse event structure.
typedef struct {
    canopy_mouse_action action;
    int x, y;
    mouse_buttons button;
    int modifiers;
    int click_count;
    float scroll_x;
    float scroll_y;
} canopy_mouse_event;

/// @brief Keyboard event structure.
typedef struct {
    canopy_key_action action;
    keys keycode;
    int modifiers;
    int is_repeat;
} canopy_key_event;

/// @brief Generic event union.
typedef struct {
    canopy_event_type type;
    union {
        canopy_mouse_event mouse;
        canopy_key_event key;
    };
} canopy_event;

/// @brief Poll the next event, if available.
/// @param[out] out_event Pointer to a canopy_event struct to fill.
/// @return true if an event was available and written to out_event, false otherwise.
bool canopy_poll_event(canopy_event* out_event);

/// @brief Post a fake (empty) event to the queue to wake up wait-based event loops.
///
/// This is useful to break out of canopy_wait_events() from another thread.
void canopy_post_empty_event(void);

/// @brief Process all pending events and dispatch them to the internal event system.
///
/// This should be called regularly if you're using a manual event loop.
void canopy_pump_events(void);

/// @brief Block until an event occurs and dispatch it.
///
/// Uses [NSDate distantFuture] to sleep until the next input event.
void canopy_wait_events(void);

/// @brief Block until an event occurs or the timeout is reached.
///
/// @param timeout_seconds Maximum number of seconds to wait before returning.
void canopy_wait_events_timeout(double timeout_seconds);

/// @brief Manually push an event into the queue.
void canopy_push_event(canopy_event event);





/*******************************************************************************/
/// @name (Optional) Window & Input Utilities
/// @{

/// @brief Get the current window position.
/// @param window The window to query.
/// @param[out] pos_x Pointer to store the X position.
/// @param[out] pos_y Pointer to store the Y position.
/// @todo Implement platform-specific retrieval of window position.
void canopy_get_window_pos(canopy_window* window, int* pos_x, int* pos_y);

/// @brief Set the position of the window.
/// @param window The window to move.
/// @param pos_x New X position.
/// @param pos_y New Y position.
/// @todo Implement platform-specific window positioning.
void canopy_set_window_pos(canopy_window* window, int pos_x, int pos_y);

/// @brief Get the name of a key given its key code and scancode.
/// @param key The internal key enum.
/// @param scancode The system-specific scancode.
/// @return A string representing the key name.
/// @todo Map key codes to human-readable names.
const char* canopy_get_key_name(int key, int scancode);

/// @brief Get the platform-specific scancode for a key.
/// @param key The internal key enum.
/// @return The system-specific scancode.
/// @todo Implement scancode resolution for keys.
int canopy_get_key_scancode(int key);

/// @brief Get the current state of a key (pressed or not).
/// @param window The window context.
/// @param key The internal key enum.
/// @return 1 if pressed, 0 otherwise.
/// @todo Implement key state tracking.
int canopy_get_key(canopy_window* window, int key);

/// @brief Get the state of a mouse button.
/// @param window The window context.
/// @param button The mouse button enum.
/// @return 1 if pressed, 0 otherwise.
/// @todo Implement mouse button state tracking.
int canopy_get_mouse_button(canopy_window* window, int button);

/// @brief Get the current position of the mouse cursor.
/// @param window The window context.
/// @param[out] pos_x Pointer to store the X position.
/// @param[out] pos_y Pointer to store the Y position.
/// @todo Track or query the cursor position from the platform.
void canopy_get_cursor_pos(canopy_window* window, double* pos_x, double* pos_y);

/// @}






#ifdef __cplusplus
}
#endif
#endif // CANOPY_H

