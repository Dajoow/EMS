#include <gui/containers/keyboard_modbus.hpp>
#include <touchgfx/Utils.hpp>

#include <gui/common/FrontendApplication.hpp>
#include <mvp/View.hpp>
#include <gui/modbus_threshold_setting_screen/Modbus_threshold_settingPresenter.hpp>
#include <touchgfx/widgets/Box.hpp>
#include <touchgfx/containers/ScrollableContainer.hpp>
#include <touchgfx/containers/Container.hpp>
#include <touchgfx/widgets/Image.hpp>
#include <touchgfx/widgets/TextArea.hpp>
#include <touchgfx/containers/buttons/Buttons.hpp>
#include <gui/containers/keyboard_modbus.hpp>
#include <touchgfx/widgets/ButtonWithLabel.hpp>

keyboard_modbus::keyboard_modbus()
{

}

void keyboard_modbus::initialize()
{
    keyboard_modbusBase::initialize();
    des = NULL;
    dessize = 0;
    refreshFlag = 0;
    buf_ptr = 0;
    dot_cnt = 0;
    memset(buff, '\0', sizeof(buff));
    Unicode::snprintf(shownumBuffer, SHOWNUM_SIZE, "%s", buff);
    shownum.invalidate();
}

void keyboard_modbus::SetBufPar(void* p, uint8_t size, void* w)
{
    des = p;
    dessize = size;
    widget = w;
}

void keyboard_modbus::fun_num_1()
{
    if(buf_ptr < int(sizeof(buff) - 1)){ //当初值为0，点击1后，数值为1
        buff[buf_ptr] = '1';
        buf_ptr++;
    }
    buff[buf_ptr] = '\0';

    Unicode::snprintf(shownumBuffer, SHOWNUM_SIZE, "%s", buff);
    shownum.invalidate();
}

void keyboard_modbus::fun_num_2()
{
    if(buf_ptr < int(sizeof(buff) - 1)){ //当初值为0，点击1后，数值为1
        buff[buf_ptr] = '2';
        buf_ptr++;
    }
    buff[buf_ptr] = '\0';

    Unicode::snprintf(shownumBuffer, SHOWNUM_SIZE, "%s", buff);
    shownum.invalidate();
}

void keyboard_modbus::fun_num_3()
{
    if(buf_ptr < int(sizeof(buff) - 1)){ //当初值为0，点击1后，数值为1
        buff[buf_ptr] = '3';
        buf_ptr++;
    }
    buff[buf_ptr] = '\0';

    Unicode::snprintf(shownumBuffer, SHOWNUM_SIZE, "%s", buff);
    shownum.invalidate();
}
void keyboard_modbus::fun_num_4()
{
    if(buf_ptr < int(sizeof(buff) - 1)){ //当初值为0，点击1后，数值为1
        buff[buf_ptr] = '4';
        buf_ptr++;
    }
    buff[buf_ptr] = '\0';

    Unicode::snprintf(shownumBuffer, SHOWNUM_SIZE, "%s", buff);
    shownum.invalidate();
}

void keyboard_modbus::fun_num_5()
{
    if(buf_ptr < int(sizeof(buff) - 1)){ //当初值为0，点击1后，数值为1
        buff[buf_ptr] = '5';
        buf_ptr++;
    }
    buff[buf_ptr] = '\0';

    Unicode::snprintf(shownumBuffer, SHOWNUM_SIZE, "%s", buff);
    shownum.invalidate();
}

void keyboard_modbus::fun_num_6()
{
    if(buf_ptr < int(sizeof(buff) - 1)){ //当初值为0，点击1后，数值为1
        buff[buf_ptr] = '6';
        buf_ptr++;
    }
    buff[buf_ptr] = '\0';

    Unicode::snprintf(shownumBuffer, SHOWNUM_SIZE, "%s", buff);
    shownum.invalidate();
}

void keyboard_modbus::fun_num_7()
{
    if(buf_ptr < int(sizeof(buff) - 1)){ //当初值为0，点击1后，数值为1
        buff[buf_ptr] = '7';
        buf_ptr++;
    }
    buff[buf_ptr] = '\0';

    Unicode::snprintf(shownumBuffer, SHOWNUM_SIZE, "%s", buff);
    shownum.invalidate();
}

void keyboard_modbus::fun_num_8()
{
    if(buf_ptr < int(sizeof(buff) - 1)){ //当初值为0，点击1后，数值为1
        buff[buf_ptr] = '8';
        buf_ptr++;
    }
    buff[buf_ptr] = '\0';

    Unicode::snprintf(shownumBuffer, SHOWNUM_SIZE, "%s", buff);
    shownum.invalidate();
}

void keyboard_modbus::fun_num_9()
{
    if(buf_ptr < int(sizeof(buff) - 1)){ //当初值为0，点击1后，数值为1
        buff[buf_ptr] = '9';
        buf_ptr++;
    }
    buff[buf_ptr] = '\0';

    Unicode::snprintf(shownumBuffer, SHOWNUM_SIZE, "%s", buff);
    shownum.invalidate();
}

void keyboard_modbus::fun_num_0()
{
    if(buf_ptr < int(sizeof(buff) - 1)){ //当初值为0，点击1后，数值为1
        buff[buf_ptr] = '0';
        buf_ptr++;
    }
    buff[buf_ptr] = '\0';

    Unicode::snprintf(shownumBuffer, SHOWNUM_SIZE, "%s", buff);
    shownum.invalidate();
}

//清零
void keyboard_modbus::fun_ce()
{
  buf_ptr = 0;
  dot_cnt = 0;
  memset(buff, 0, sizeof(buff));
  buff[0] = '\0';

  Unicode::snprintf(shownumBuffer, SHOWNUM_SIZE, "%s", buff);
  shownum.invalidate();
}

//删除末位
void keyboard_modbus::fun_de()
{
    if(buf_ptr > 0){ //当初值为0，点击1后，数值为1
        buf_ptr--;
        if(buff[buf_ptr] == '.')
          dot_cnt--;
        buff[buf_ptr] = '\0';
    }

    Unicode::snprintf(shownumBuffer, SHOWNUM_SIZE, "%s", buff);
    shownum.invalidate();
}

void keyboard_modbus::fun_ok()
{
    Unicode::snprintf((touchgfx::Unicode::UnicodeChar*)des, dessize, "%s", buff);
    ((touchgfx::WildcardTextButtonStyle< touchgfx::ImageButtonStyle < touchgfx::ClickButtonTrigger > > *) (widget))->invalidate();

    setVisible(false);
    invalidate();
}

void keyboard_modbus::fun_sub()
{
    if(buf_ptr == 0){ //当初值为0，点击1后，数值为1
        buff[buf_ptr] = 45;
        buf_ptr++;
    }
    buff[buf_ptr] = '\0';

    Unicode::snprintf(shownumBuffer, SHOWNUM_SIZE, "%s", buff);
    shownum.invalidate();
}

void keyboard_modbus::fun_plus()
{
    // if(buf_ptr == 0){ //当初值为0，点击1后，数值为1
    //     buff[buf_ptr] = '+';
    //     buf_ptr++;
    // }
    // buff[buf_ptr] = '\0';

    // Unicode::snprintf(shownumBuffer, SHOWNUM_SIZE, "%s", buff);
    // shownum.invalidate();
}

void keyboard_modbus::fun_dot()
{
    if(buf_ptr < int(sizeof(buff) - 1) && dot_cnt == 0){ //当初值为0，点击1后，数值为1
        buff[buf_ptr] = '.';
        buf_ptr++;
        dot_cnt++;
    }
    buff[buf_ptr] = '\0';

    Unicode::snprintf(shownumBuffer, SHOWNUM_SIZE, "%s", buff);
    shownum.invalidate();
}
