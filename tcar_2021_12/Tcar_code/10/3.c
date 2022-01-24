#include <tk/tkernel.h>
#include <tm/tmonitor.h>
#include <tk/device.h>

#define SENSOR_MAX   4

const UB sensor_addr[SENSOR_MAX] = {0x38, 0x39, 0x3c, 0x3D};

#define I2C_ATR_EXEC  -100

typedef struct {
    UH red;
    UH green;
    UH blue;
    UH clear;
}T_COLOR_DATA;

T_COLOR_DATA  cdata[SENSOR_MAX];

ER read_sensor(ID dd, T_COLOR_DATA cdata[]){
    INT i;

    T_I2C_EXEC    exec;
    UB            snd_data;
    UB            rcv_data;
    SZ            asz;
    ER            err;
    
    UH sens_data[4];
    UB data[2];

    typedef struct {
          UW  sadr;
          W   snd_size;
          UB  *snd_data;
          W   rcv_size;
          UB  *rcv_data;
    }T_I2C_EXEC;

    for(i=0; i<4; i++){
      while(1){
        exec.sadr = sensor_addr[i];
        snd_data = 0x42;
        exec.snd_size = 1;
        exec.snd_data = &snd_data;
        exec.rcv_size = 1;
        exec.rcv_data = &rcv_data;
        
        err = tk_swri_dev(dd, TDN_I2C_EXEC, &exec, sizeof(T_I2C_EXEC), &asz);

        if((rcv_data & 0x80) != 0){
          exec.sadr = sensor_addr[i];
          snd_data = 0x50;
          exec.snd_size = 1;
          exec.snd_data = &snd_data;
          exec.rcv_size = sizeof(sens_data);
          exec.rcv_data = (UB*)(&sens_data);

          err = tk_swri_dev(dd, TDN_I2C_EXEC, &exec, sizeof(T_I2C_EXEC), &asz);
          cdata[i].clear = sens_data[3];
          //return sens_data[i];
          //tm_printf("%d\n",sens_data[i]);
          break;
        }
      }
    }
}

EXPORT  INT usermain(void){
    INT dd;

    dd = tk_opn_dev("iicb", TD_UPDATE);

    T_I2C_EXEC    exec;
    UB            snd_data;
    UB            rcv_data;
    SZ            asz;
    ER            err;
    
    UH sens_data[4];
    UB data[2];
    
    INT i;
    for(i = 0; i < SENSOR_MAX; i++){
      data[0] = 0x40;
      data[1] = 0x80;
      err = tk_swri_dev(dd, sensor_addr[i], &data, 2, &asz);

      data[0] = 0x40;
      data[1] = 0x00;
      err = tk_swri_dev(dd, sensor_addr[i], &data, 2, &asz);

      data[0] = 0x42;
      data[1] = 0x10;
      err = tk_swri_dev(dd, sensor_addr[i], &data, 2, &asz);
    }

    while(1){
       read_sensor(dd, cdata);
       for (INT i = 0; i<SENSOR_MAX; i++ ){
          tm_printf("%d:%d\n",i, cdata[i].clear);
          tk_dly_tsk(1000);
       }
     }
     tk_slp_tsk(TMO_FEVR);
     return 0;
}
   