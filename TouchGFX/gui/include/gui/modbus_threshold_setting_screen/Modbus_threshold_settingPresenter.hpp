#ifndef MODBUS_THRESHOLD_SETTINGPRESENTER_HPP
#define MODBUS_THRESHOLD_SETTINGPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class Modbus_threshold_settingView;

class Modbus_threshold_settingPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    Modbus_threshold_settingPresenter(Modbus_threshold_settingView& v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    virtual ~Modbus_threshold_settingPresenter() {};

private:
    Modbus_threshold_settingPresenter();

    Modbus_threshold_settingView& view;
};

#endif // MODBUS_THRESHOLD_SETTINGPRESENTER_HPP
