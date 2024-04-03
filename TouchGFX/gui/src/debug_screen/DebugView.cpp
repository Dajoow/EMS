#include <gui/debug_screen/DebugView.hpp>

#ifndef SIMULATOR
extern "C"{
    extern void can_open_switch ();
    extern void can_close_switch ();
}
#endif

DebugView::DebugView () 
{ 
    exit_count = 10; 
}

void DebugView::setupScreen()
{
    DebugViewBase::setupScreen();
}

void DebugView::tearDownScreen()
{
    exit_count = 10; 
    DebugViewBase::tearDownScreen();
}

void DebugView::switch_open_cb()
{
    can_open_switch();
}

void DebugView::switch_close_cb()
{
    can_close_switch();
}

void DebugView::exit_count_cb()
{
    if (exit_count > 0) {
        exit_count--;
        Unicode::snprintf(exitBuffer, EXIT_SIZE, "%d", exit_count);
        exit.invalidate();
    } else {
        exit_count = 10;
        application().gotoMainScreenScreenNoTransition();
        Unicode::snprintf(exitBuffer, EXIT_SIZE, "%d", exit_count);
        exit.invalidate();
    }
}
