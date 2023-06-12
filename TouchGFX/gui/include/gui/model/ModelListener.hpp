#ifndef MODELLISTENER_HPP
#define MODELLISTENER_HPP

#include <gui/model/Model.hpp>

#ifndef SIMULATOR
#include "station_ctl.h"
#endif

class ModelListener
{
public:
    ModelListener() : model(0) {}
    
    virtual ~ModelListener() {}

    void bind(Model* m)
    {
        model = m;
    }

#ifndef SIMULATOR
    virtual void NotifyViewMsg(ModelToViewData modelToViewData){};
#endif

protected:
    Model* model;
};

#endif // MODELLISTENER_HPP
