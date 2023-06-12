#ifndef CUSTOMCONTAINER3_HPP
#define CUSTOMCONTAINER3_HPP

#include <gui_generated/containers/CustomContainer3Base.hpp>

class CustomContainer3 : public CustomContainer3Base
{
public:
    CustomContainer3();
    virtual ~CustomContainer3() {}

    virtual void initialize();

    void SetText(int val1, int val2);
protected:
};

#endif // CUSTOMCONTAINER3_HPP
