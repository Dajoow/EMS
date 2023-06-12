#ifndef CUSTOMCONTAINER4_HPP
#define CUSTOMCONTAINER4_HPP

#include <gui_generated/containers/CustomContainer4Base.hpp>

class CustomContainer4 : public CustomContainer4Base
{
public:
    CustomContainer4();
    virtual ~CustomContainer4() {}

    virtual void initialize();

    void SetText(int val1, int val2);
protected:
};

#endif // CUSTOMCONTAINER4_HPP
