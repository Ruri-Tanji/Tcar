#include <tk/tkernel.h>
#include <tm/tmonitor.h>
#include <tk/device.h>

#define SENSOR_MAX   4

const UB sensor_addr[SENSOR_MAX] = {0x38, 0x39, 0x3c, 0x3D};

#define I2C_ATR_EXEC  -100


EXPORT  INT usermain(void){
    ID dd;

    dd = tk_opn_dev("iicb", TD_UPDATE);

    typedef struct {
        UW  sadr;
        W   snd_size;
        UB  *snd_data;
        W   rcv_size;
        UB  *rcv_data;
    }T_I2C_EXEC;

    UB data[2];
    SZ asz;
    INT i;
    ER err;

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

    for(i=0; i<SENSOR_MAX; i++){
      while(1){
        T_I2C_EXEC    exec;
        UB            snd_data;
        UB            rcv_data;
        SZ            asz;
        ER            err;
        
        exec.sadr = sensor_addr[i];
        snd_data = 0x42;
        exec.snd_size = 1;
        exec.snd_data = &snd_data;
        exec.rcv_size = 1;
        exec.rcv_data = &rcv_data;
        
        err = tk_swri_dev(dd, TDN_I2C_EXEC, &exec, sizeof(T_I2C_EXEC), &asz);

        if((exec.rcv_data & 0x80) != 0){
          T_I2C_EXEC    exec;
          UB            snd_data;
          UH            sens_data[4];
          SZ            asz;
          ER            err;

          exec.sadr = sensor_addr[i];
          snd_data = 0x50;
          exec.snd_size = 1;
          exec.snd_data = &snd_data;
          exec.rcv_size = sizeof(sens_data);
          exec.rcv_data = (UB*)(&sens_data);

          err = tk_swri_dev(dd, TDN_I2C_EXEC, &exec, sizeof(T_I2C_EXEC), &asz);

          tm_printf("%d\n",exec.rcv_data);
          break;
        }
      }
    }
}