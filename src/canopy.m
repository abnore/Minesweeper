#import <Cocoa/Cocoa.h>
#import "canopy.h"

//----------------------------------------
// Struct to hold macOS window internals
//----------------------------------------
struct canopy_window {
    id window;
    id view;
    id delegate;
    framebuffer fb;

    bool should_close;
    bool is_opaque;
    uint32_t pixel_ratio; // support of high spi/retina screen
};

//----------------------------------------
// Window Delegate
//----------------------------------------
@interface canopy_delegate : NSObject <NSWindowDelegate>
{
    canopy_window* window;
}

- (instancetype)init_with_canopy_window:(canopy_window*)init_window;

@end
//----------------------------------------
@implementation canopy_delegate

- (instancetype)init_with_canopy_window:(canopy_window*)init_window
{
    TRACE("Creating Canopy Delegate");
    self = [super init];
    if (self) {
        window = init_window;
        DEBUG("Window pointer assigned to delegate: %p", window);
    }
    return self;
}

- (BOOL)windowShouldClose:(id)sender
{
    INFO("Window close requested");
    window->should_close = true;
    return NO;
}

- (void)showCustomAboutPanel:(id)sender
{
    INFO("Displaying About panel");

    NSImage* icon = [[NSImage alloc] initWithContentsOfFile:@"img/icon.bmp"];
    if (!icon) {
        WARN("Could not load custom icon, falling back to default");
    }

    NSDictionary* options = @{
        @"ApplicationName": @"Minesweeper",
        @"ApplicationVersion": @"0.1.0",
        @"ApplicationIcon": icon ?: [NSImage imageNamed:NSImageNameApplicationIcon],
        @"Copyright": @"© 2025 Canopy",
        @"Credits": [[NSAttributedString alloc]
                    initWithString:@"Built by abnore using Cocoa and C"]
    };

    [NSApp orderFrontStandardAboutPanelWithOptions:options];
    TRACE("About panel shown");
}
@end
//----------------------------------------
// View
//----------------------------------------
@interface canopy_view : NSView
{
    canopy_window* window;
}

- (instancetype)init_with_frame:(NSRect)frame window:(canopy_window*)win;

@end
//----------------------------------------
@implementation canopy_view

- (instancetype)init_with_frame:(NSRect)frame window:(canopy_window*)win {
    TRACE("Initializing Canopy View with frame (%d, %d)", (int)frame.size.width, (int)frame.size.height);
    self = [super initWithFrame:frame];
    if (self) {
        window = win;
    }
    return self;
}

- (BOOL)isFlipped { return YES; }
- (BOOL)acceptsFirstResponder { return YES; }
- (BOOL)is_opaque { return window->is_opaque;}
- (void)updateTrackingAreas
{ // To receive mouse entered and exit we setup a tracking area
    NSTrackingAreaOptions opts =  NSTrackingMouseEnteredAndExited |
                                  NSTrackingActiveInKeyWindow |
                                  NSTrackingEnabledDuringMouseDrag |
                                  NSTrackingInVisibleRect;


    NSTrackingArea* area = [[NSTrackingArea alloc]
                            initWithRect:[self bounds]
                                 options:opts
                                   owner:self
                                userInfo:nil];

    [self addTrackingArea:area];
    [super updateTrackingAreas];
}

//----------------------------------------
// Standard mouse event handler
// so that i can add as many as i want later
//----------------------------------------
- (void)push_mouse_event_with_action:(canopy_mouse_action)action
                           event:(NSEvent *)event
                         scrollX:(float)sx
                         scrollY:(float)sy
{
    NSPoint pos = [self convertPoint:[event locationInWindow] fromView:nil];

    canopy_event e = {
        .type = CANOPY_EVENT_MOUSE,
        .mouse.action = action,
        .mouse.x = (int)pos.x,
        .mouse.y = (int)pos.y,
        .mouse.button = (int)[event buttonNumber],
        .mouse.modifiers = (int)[event modifierFlags],
        .mouse.scroll_x = sx,
        .mouse.scroll_y = sy,
    };

    // Safe call only for click-related events otherwise we trap
    switch(action){
        default: e.mouse.click_count = 0;
        break;
        case CANOPY_MOUSE_PRESS:
        case CANOPY_MOUSE_RELEASE:
        case CANOPY_MOUSE_DRAG:
                 e.mouse.click_count = (int)[event clickCount];
        break;
    }

    canopy_push_event(e);
}

/* ------------  Press events ------------*/
- (void)mouseDown:(NSEvent *)event {
    [self push_mouse_event_with_action:CANOPY_MOUSE_PRESS event:event scrollX:0 scrollY:0];
}
- (void)rightMouseDown:(NSEvent *)event {
    [self push_mouse_event_with_action:CANOPY_MOUSE_PRESS event:event scrollX:0 scrollY:0];
}
- (void)otherMouseDown:(NSEvent *)event {
    [self push_mouse_event_with_action:CANOPY_MOUSE_PRESS event:event scrollX:0 scrollY:0];
}
/* ------------ Release events ------------*/
- (void)mouseUp:(NSEvent *)event {
    [self push_mouse_event_with_action:CANOPY_MOUSE_RELEASE event:event scrollX:0 scrollY:0];
}
- (void)rightMouseUp:(NSEvent *)event {
    [self push_mouse_event_with_action:CANOPY_MOUSE_RELEASE event:event scrollX:0 scrollY:0];
}
- (void)otherMouseUp:(NSEvent *)event {
    [self push_mouse_event_with_action:CANOPY_MOUSE_RELEASE event:event scrollX:0 scrollY:0];
}
/* ----------- Drag/move events -----------*/
- (void)mouseDragged:(NSEvent *)event {
    [self push_mouse_event_with_action:CANOPY_MOUSE_DRAG event:event scrollX:0 scrollY:0];
}
- (void)rightMouseDragged:(NSEvent *)event {
    [self push_mouse_event_with_action:CANOPY_MOUSE_DRAG event:event scrollX:0 scrollY:0];
}
- (void)otherMouseDragged:(NSEvent *)event {
    [self push_mouse_event_with_action:CANOPY_MOUSE_DRAG event:event scrollX:0 scrollY:0];
}
- (void)mouseMoved:(NSEvent *)event {
    [self push_mouse_event_with_action:CANOPY_MOUSE_MOVE event:event scrollX:0 scrollY:0];
}
/* ------------ Scroll events ------------ */
- (void)scrollWheel:(NSEvent *)event {
    [self push_mouse_event_with_action:CANOPY_MOUSE_SCROLL
                            event:event
                         scrollX:[event scrollingDeltaX]
                         scrollY:[event scrollingDeltaY]];
}
/* -------- Enter and exit events -------- */
- (void)mouseEntered:(NSEvent *)event {
    [self push_mouse_event_with_action:CANOPY_MOUSE_ENTER event:event scrollX:0 scrollY:0];
}

- (void)mouseExited:(NSEvent *)event {
    [self push_mouse_event_with_action:CANOPY_MOUSE_EXIT event:event scrollX:0 scrollY:0];
}

/* ------------- Key events -------------- */
- (void)push_key_event_with_action:(canopy_key_action)action event:(NSEvent *)event {
    canopy_event e = {
        .type = CANOPY_EVENT_KEY,
        .key.action = action,
        .key.keycode = (int)[event keyCode],
        .key.modifiers = (int)[event modifierFlags],
        .key.is_repeat = [event isARepeat] ? 1 : 0
    };

    canopy_push_event(e);
}

- (void)keyDown:(NSEvent *)event {
    [self push_key_event_with_action:CANOPY_KEY_PRESS event:event];
}

- (void)keyUp:(NSEvent *)event {
    [self push_key_event_with_action:CANOPY_KEY_RELEASE event:event];
}
@end

//----------------------------------------
// Menubar Setup - internal
// 	Taken from GLFW as this
// 	is nasty stuff
//----------------------------------------
static void create_menubar(id delegate)
{
    NSString* appName = [[NSProcessInfo processInfo] processName];
    // Top-level menubar
    NSMenu* menubar = [[NSMenu alloc] init];
    [NSApp setMainMenu:menubar];

    // App menu item (blank title)
    NSMenuItem* appMenuItem = [menubar addItemWithTitle:@"" action:NULL keyEquivalent:@""];

    // Application menu
    NSMenu* appMenu = [[NSMenu alloc] init];

    // About
    NSMenuItem* aboutItem = [[NSMenuItem alloc]
	    initWithTitle:[NSString stringWithFormat:@"About %@", appName]
		   action:@selector(showCustomAboutPanel:)
	    keyEquivalent:@""];
    [aboutItem setTarget:delegate];
    [appMenu addItem:aboutItem];

    [appMenu addItem:[NSMenuItem separatorItem]];

    // Services
    NSMenu* servicesMenu = [[NSMenu alloc] init];
    [NSApp setServicesMenu:servicesMenu];
    [[appMenu addItemWithTitle:@"Services" action:NULL keyEquivalent:@""] setSubmenu:servicesMenu];

    [appMenu addItem:[NSMenuItem separatorItem]];

    // Hide, Hide Others, Show All
    [appMenu addItemWithTitle:[NSString stringWithFormat:@"Hide %@", appName]
                       action:@selector(hide:)
                keyEquivalent:@"h"];
    [[appMenu addItemWithTitle:@"Hide Others"
                        action:@selector(hideOtherApplications:)
                 keyEquivalent:@"h"]
        setKeyEquivalentModifierMask:(NSEventModifierFlagOption | NSEventModifierFlagCommand)];

    [appMenu addItemWithTitle:@"Show All"
                       action:@selector(unhideAllApplications:)
                keyEquivalent:@""];

    [appMenu addItem:[NSMenuItem separatorItem]];

    // Quit
    [appMenu addItemWithTitle:[NSString stringWithFormat:@"Quit %@", appName]
                       action:@selector(terminate:)
                keyEquivalent:@"q"];

    [appMenuItem setSubmenu:appMenu];

    // Cocoa semi-private fix to set this as the real app menu
    SEL setAppleMenuSelector = NSSelectorFromString(@"setAppleMenu:");
    if ([NSApp respondsToSelector:setAppleMenuSelector]) {
        [NSApp performSelector:setAppleMenuSelector withObject:appMenu];
    }

    // Add "Window" menu for things like Minimize/Zoom/Fullscreen
    NSMenuItem* windowMenuItem = [menubar addItemWithTitle:@"" action:NULL keyEquivalent:@""];
    NSMenu* windowMenu = [[NSMenu alloc] initWithTitle:@"Window"];
    [windowMenuItem setSubmenu:windowMenu];
    [NSApp setWindowsMenu:windowMenu];

    [windowMenu addItemWithTitle:@"Minimize"
                          action:@selector(performMiniaturize:)
                   keyEquivalent:@"m"];
    [windowMenu addItemWithTitle:@"Zoom"
                          action:@selector(performZoom:)
                   keyEquivalent:@""];
    [windowMenu addItem:[NSMenuItem separatorItem]];
    [windowMenu addItemWithTitle:@"Bring All to Front"
                          action:@selector(arrangeInFront:)
                   keyEquivalent:@""];
    [windowMenu addItem:[NSMenuItem separatorItem]];
    [[windowMenu addItemWithTitle:@"Enter Full Screen"
                           action:@selector(toggleFullScreen:)
                    keyEquivalent:@"f"]
     setKeyEquivalentModifierMask:NSEventModifierFlagControl | NSEventModifierFlagCommand];
}

//--------------------------------------------------------------------------------
// Public API Implementation - C Wrappers
//--------------------------------------------------------------------------------

/* Window functions */
canopy_window* canopy_create_window(const char* title,
                                    int width,
                                    int height,
                                    canopy_window_style flags)
{
    @autoreleasepool {
        [NSApplication sharedApplication];
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
        [NSApp activateIgnoringOtherApps:YES];

        TRACE("Creating window: %dx%d \"%s\"", width, height, title);

        canopy_window* win = canopy_malloc(sizeof(canopy_window));

        if(!win) {
            FATAL("Failed to allocate canopy_window");
            return NULL;

        }

        win->delegate = [[canopy_delegate alloc] init_with_canopy_window:win];

        win->view = [[canopy_view alloc]
                init_with_frame: NSMakeRect(0, 0, width, height)
                         window: win];

        win->window = [[NSWindow alloc]
             initWithContentRect: NSMakeRect(0, 0, width, height)
                       styleMask: (NSWindowStyleMask)flags
                         backing: NSBackingStoreBuffered
                           defer: NO];

        create_menubar(win->delegate);

        [(NSWindow*)win->window center];
        [win->window setTitle: [NSString stringWithUTF8String:title]];
        [win->window setDelegate: win->delegate];
        [win->window setContentView: win->view];
        [win->window makeKeyAndOrderFront:nil];
        [win->window setAcceptsMouseMovedEvents: YES];
        [win->window makeFirstResponder: win->view];

        [win->view setWantsLayer: YES];
        // default opaque, can be set manually
        //[win->view setOpaque: YES];
        win->is_opaque = true;
        //[[win->view layer] setOpaque:YES]; // ensures the layer also is opaque - not needed
        // Properly handle content scaling for fidelity display (i.e. retina display)
        // INFO: Not supported yet, need to port this to every graphical section
        NSView *view = (NSView *)win->view;
        [[view layer] setContentsScale: view.window.backingScaleFactor];
        win->pixel_ratio = view.window.backingScaleFactor;
        INFO("Content Scale is : %i, not supported yet", win->pixel_ratio);

        canopy_init_framebuffer(win);

        win->should_close = false;

        INFO("Created window: \"%s\" (%dx%d)", title, width, height);

        canopy_post_empty_event();

        return win;
    }
}

void canopy_set_icon(const char* filepath)
{
    if (!filepath) {
        WARN("No icon filepath provided");
        return;
    };


    NSString* path = [NSString stringWithUTF8String:filepath];
    NSImage* icon = [[NSImage alloc] initWithContentsOfFile:path];

    if (icon) {
        [NSApp setApplicationIconImage:icon];
        INFO("Set application icon from path: %s", filepath);
    } else {
        ERROR("Failed to load icon from path: %s", filepath);
    }
}

bool canopy_is_window_opaque(canopy_window *win)
{
    return [win->view is_opaque];
}
void canopy_set_window_transparent(canopy_window *win, bool enable)
{
    if(enable) {
        win->is_opaque = false;
        [win->window setOpaque:NO];
        [win->window setHasShadow:NO];
        [win->window setBackgroundColor:[NSColor clearColor]];
        TRACE("Window transparent");
    } else {
        win->is_opaque = true;
        [win->window setOpaque:YES];
        [win->window setHasShadow:YES];
        TRACE("Window opaque");
    }
}
void canopy_free_window(canopy_window* win)
{
    if (!win) {
        WARN("Tried to free a NULL window");
        return;
    }

    @autoreleasepool {
        TRACE("Freeing canopy window");
        // Hide the window
        [win->window orderOut:nil];

        // Disconnect delegate and release
        [win->window setDelegate:nil];
        [win->delegate release];
        win->delegate = nil;

        // Release the view
        [win->view release];
        win->view = nil;

        // Close and clear the window object
        [win->window close];
        win->window = nil;

        // Free framebuffer
        if (win->fb.pixels) {
            canopy_free(win->fb.pixels);
            win->fb.pixels= NULL;
        }

        // (Optional) Let Cocoa flush pending events
        canopy_pump_events();

        DEBUG("Window closed and resources cleaned up");
    }

    canopy_free(win);
}


bool canopy_window_should_close(canopy_window *window)
{
    canopy_pump_events();  // Keep the UI alive
    return window->should_close;
}

bool canopy_init_framebuffer(canopy_window *win)
{
    if(win->fb.pixels == NULL)
    {
        NSView* view = (NSView*)win->view;
        NSRect bounds = [view bounds];

        win->fb.width = (int)bounds.size.width;
        win->fb.height = (int)bounds.size.height;
        win->fb.pitch = win->fb.width * CANOPY_BYTES_PER_PIXEL;

        if (win->fb.width <= 0 || win->fb.height <= 0)
        {
            ERROR("Invalid framebuffer size: %dx%d\n",
                        win->fb.width, win->fb.height);
            return false;
        }
        // Allocate buffer to match window size
        win->fb.pixels = canopy_malloc(win->fb.pitch * win->fb.height);
        if (!win->fb.pixels) {
            FATAL("Failed to allocate framebuffer");
            return false;
        }
        TRACE("Initialized framebuffer: %dx%d (pitch %d)",
              win->fb.width, win->fb.height, win->fb.pitch);

    }

    return true;
}

void canopy_present_buffer(canopy_window *window)
{
    @autoreleasepool {
        if (!window->fb.pixels) {
            ERROR("Tried to present a NULL framebuffer");
            return;
        }

        NSBitmapImageRep *rep = [[[NSBitmapImageRep alloc]
                initWithBitmapDataPlanes: (uint8_t**)&window->fb.pixels
                              pixelsWide: window->fb.width
                              pixelsHigh: window->fb.height
                           bitsPerSample: 8
                         samplesPerPixel: 4
                                hasAlpha: YES
                                isPlanar: NO
                          colorSpaceName: NSDeviceRGBColorSpace
                             bytesPerRow: window->fb.pitch
                            bitsPerPixel: 32]
                            autorelease];

        NSImage *image = [[[NSImage alloc]
                            initWithSize: NSMakeSize(window->fb.width,
                                                    window->fb.height)]
                            autorelease];

        [image addRepresentation: rep];
        [(NSView*)window->view layer].contents = image;
        //TRACE("Framebuffer presented to screen");
    }
}


framebuffer *canopy_get_framebuffer(canopy_window *window)
{
    return &window->fb;
}

void canopy_swap_backbuffer(canopy_window *w, framebuffer *backbuffer)
{
    if (!backbuffer || !backbuffer->pixels) {
        ERROR("Backbuffer is NULL");
        return;
    }

    if (!w->fb.pixels) {
        ERROR("Framebuffer in window is NULL");
        return;
    }

    // Old way of copying byte for byte is too expensive when i scale
    // It is much more effecient to just swap pointers since the buffers
    // are equal
    //memcpy(w->fb.pixels, backbuffer->pixels, w->fb.height*w->fb.pitch);
    uint32_t *temp = w->fb.pixels;
    w->fb.pixels = backbuffer->pixels;
    backbuffer->pixels = temp;
}

/* Pump messages so that the window is shown to be responsive
 * Also needed for event handling
 * */

void canopy_pump_events(void)
{
    @autoreleasepool {
        NSEvent* event;

        while ((event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                           untilDate:nil
                                              inMode:NSDefaultRunLoopMode
                                             dequeue:YES]))
        {
            [NSApp sendEvent:event];
            [NSApp updateWindows];
        }
    } // autoreleasepool
}
void canopy_post_empty_event(void)
{
    @autoreleasepool {

    NSEvent* event = [NSEvent otherEventWithType:NSEventTypeApplicationDefined
                                        location:NSMakePoint(0, 0)
                                   modifierFlags:0
                                       timestamp:0
                                    windowNumber:0
                                         context:nil
                                         subtype:0
                                           data1:0
                                           data2:0];
    [NSApp postEvent:event atStart:YES];

    } // autoreleasepool
}

void canopy_wait_events(void)
{
    @autoreleasepool {
        NSEvent* event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                            untilDate:[NSDate distantFuture]
                                               inMode:NSDefaultRunLoopMode
                                              dequeue:YES];
        if (event) {
            [NSApp sendEvent:event];
            [NSApp updateWindows];
        }
    }
}
void canopy_wait_events_timeout(double timeout_seconds)
{
    @autoreleasepool {
        NSDate* timeout_date = [NSDate dateWithTimeIntervalSinceNow:timeout_seconds];

        NSEvent* event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                            untilDate:timeout_date
                                               inMode:NSDefaultRunLoopMode
                                              dequeue:YES];
        if (event) {
            [NSApp sendEvent:event];
            [NSApp updateWindows];
        }
    }
}
