#include <tk/tkernel.h>
#include <tm/tmonitor.h>
#include <tk/device.h>

#define HEADLIGHT_ADR   0x18

UB data[2];
SZ asz;
ER err;

EXPORT  INT usermain(void){
    ID dd;

    dd = tk_opn_dev("iicb", TD_UPDATE);

    data[0] = 0x00;
    data[1] = 0x00;
    err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);

    data[0] = 0;
    data[1] = 0x00;
    err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);

    data[0] = 1;
    data[1] = 0x05;
    err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);

    data[0] = 0x0c;
    data[1] = 0x2a;
    err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);

    data[0] = 0x0d;
    data[1] = 0x2a;
    err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);

    UB red, green, blue;
    INT unit;
    
    data[0] = 0x02;
    data[1] = 0xff;
    err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
    data[0] = 0x06;
    data[1] = 0xff;
    err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
    tk_dly_tsk(1000);
    data[0] = 0x02;
    data[1] = 0x00;
    err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
    data[0] = 0x06;
    data[1] = 0x00;
    err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);

    data[0] = 0x03;
    data[1] = 0xff;
    err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
    data[0] = 0x07;
    data[1] = 0xff;
    err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
    tk_dly_tsk(1000);
    data[0] = 0x03;
    data[1] = 0x00;
    err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
    data[0] = 0x07;
    data[1] = 0x00;
    err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);

    data[0] = 0x04;
    data[1] = 0xff;
    err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
    data[0] = 0x08;
    data[1] = 0xff;
    err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
    tk_dly_tsk(1000);
    data[0] = 0x04;
    data[1] = 0x00;
    err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
    data[0] = 0x08;
    data[1] = 0x00;
    err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);

    data[0] = 0x02;
    data[1] = 0xff;
    err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
    data[0] = 0x03;
    data[1] = 0xff;
    err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
    data[0] = 0x04;
    data[1] = 0xff;
    err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
    data[0] = 0x06;
    data[1] = 0xff;
    err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
    data[0] = 0x07;
    data[1] = 0xff;
    err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
    data[0] = 0x08;
    data[1] = 0xff;
    err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);

    tk_dly_tsk(1000);

    data[0] = 0x02;
    data[1] = 0x00;
    err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
    data[0] = 0x03;
    data[1] = 0x00;
    err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
    data[0] = 0x04;
    data[1] = 0x00;
    err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
    data[0] = 0x06;
    data[1] = 0x00;
    err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
    data[0] = 0x07;
    data[1] = 0x00;
    err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
    data[0] = 0x08;
    data[1] = 0x00;
    err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
}