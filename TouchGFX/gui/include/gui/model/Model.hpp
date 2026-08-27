#ifndef MODEL_HPP
#define MODEL_HPP

#include <gui/common/GuiTransTypes.hpp>

class ModelListener;

class Model
{
public:
    Model();

    void bind(ModelListener* listener)
    {
        modelListener = listener;
    }

    void tick();
    void ViewtoModelDat(const ViewToModelData& Data);
    int requestDcdcSafeStop();

    //void err_inf(const err_info& err);
protected:
    ModelListener* modelListener;
    uint8_t deviceStatusTick;
};

#endif // MODEL_HPP
