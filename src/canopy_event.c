#include "canopy.h"

static canopy_event event_queue[CANOPY_MAX_EVENTS];
static int event_head = 0;
static int event_tail = 0;

void canopy_push_event(canopy_event ev) {
    int next = (event_tail + 1) % CANOPY_MAX_EVENTS;
    if (next != event_head) { // only add if queue not full
        event_queue[event_tail] = ev;
        event_tail = next;
    }
}

bool canopy_poll_event(canopy_event* out_event)
{
    if (event_head == event_tail) return false;
    *out_event = event_queue[event_head];
    event_head = (event_head + 1) % CANOPY_MAX_EVENTS;
    return true;
}
