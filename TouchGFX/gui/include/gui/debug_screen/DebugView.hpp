#ifndef DEBUGVIEW_HPP
#define DEBUGVIEW_HPP

#include <gui_generated/debug_screen/DebugViewBase.hpp>
#include <gui/debug_screen/DebugPresenter.hpp>

class DebugView : public DebugViewBase
{
public:
    DebugView();
    virtual ~DebugView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void switch_close_cb();
    virtual void switch_open_cb();
    virtual void exit_count_cb();

  protected:
    uint32_t exit_count;
};

#endif // DEBUGVIEW_HPP
