#include <gui/containers/CustomContainer1.hpp>

CustomContainer1::CustomContainer1()
{

}

void CustomContainer1::initialize()
{
    CustomContainer1Base::initialize();
}

void CustomContainer1::SetText(int val1)
{
    Unicode::snprintf(textArea1Buffer, TEXTAREA1_SIZE, "%d", val1);
    textArea1.invalidate();
}
