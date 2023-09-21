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

    void get_err_info(unsigned char *err,int len);
protected:
    ModelListener* modelListener;
};

#endif // MODEL_HPP
