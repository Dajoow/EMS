#ifndef KEYBOARD_MODBUS_HPP
#define KEYBOARD_MODBUS_HPP

#include <gui_generated/containers/keyboard_modbusBase.hpp>

class keyboard_modbus : public keyboard_modbusBase
{
public:
    keyboard_modbus();
    virtual ~keyboard_modbus() {}

    virtual void initialize();

    virtual void fun_num_1();
    virtual void fun_num_2();
    virtual void fun_num_3();
    virtual void fun_num_4();
    virtual void fun_num_5();
    virtual void fun_num_6();
    virtual void fun_num_7();
    virtual void fun_num_8();
    virtual void fun_num_9();
    virtual void fun_num_0();
    virtual void fun_ce();
    virtual void fun_de();
    virtual void fun_ok();
    virtual void fun_sub();
    virtual void fun_plus();
    virtual void fun_dot();

    void SetBufPar(void* p, uint8_t size, void* w);

    Unicode::UnicodeChar buff[16];
    uint8_t refreshFlag;
protected:
  int dot_cnt;
  int buf_ptr;
  void *widget;
  void *des;
  uint8_t dessize;
};

#endif // KEYBOARD_MODBUS_HPP
