#include <tk/tkernel.h>  /* T-Kernel ヘッダ */
#include <tm/tmonitor.h> /* tm_printf() など */
#include <tk/device.h> //でばいすのかくしゅせんげん、ていぎ

#define GPIOA_BASE (0x400C0000)
#define PADATA  (GPIOA_BASE + 0x00)
#define PACR    (GPIOA_BASE + 0x04)
#define PAIE    (GPIOA_BASE + 0x38)

#define GPIOE_BASE (0x400C0400)
#define PEDATA  (GPIOE_BASE + 0x00)
#define PECR    (GPIOE_BASE + 0x04)
#define PEIE    (GPIOE_BASE + 0x38)

#define GPIOH_BASE (0x400C0700)
#define PHCR    (GPIOH_BASE + 0x04)
#define PHFR3   (GPIOH_BASE + 0x10)
#define PHIE    (GPIOH_BASE + 0x38)


#define MT2_BASE (0x400C7200UL)
#define MT2EN   (MT2_BASE + 0x00)
#define MT2RUN (MT2_BASE + 0x04)
#define MT2IGCR (MT2_BASE + 0x30)
#define MT2IGOCR (MT2_BASE + 0x40)
#define MT2IGRG2 (MT2_BASE + 0x44)
#define MT2IGRG3 (MT2_BASE + 0x48)
#define MT2IGRG4 (MT2_BASE + 0x4C)

//RED right
#define PWM0 (0x02)
#define PWM1 (0x03)
#define PWM2 (0x04)
#define PWM3 (0x05)
//RED left
#define PWM4 (0x06)
#define PWM5 (0x07)
#define PWM6 (0x08)
#define PWM7 (0x09)

#define PAFR3 (0x010)
#define CGICRCG 0x400F3060
#define INT3      3


#define SENSOR_MAX 4
const UB sensor_addr[SENSOR_MAX] = {0x38, 0x39, 0x3C, 0x3D};

#define HEADLIGHT_ADR 0x18
#define I2C_ATR_EXEC -100

T_I2C_EXEC  exec;
UB snd_data;
UB rcv_data;
  
UH sens_data[4];
UB data[2];
  
SZ asz;
INT i;
ER err;
  
typedef struct{
    UH red;
    UH green;
    UH blue;
    UH clear;
  } T_COLOR_DATA;

 T_COLOR_DATA cdata[SENSOR_MAX];
 
ER read_sensor(ID dd, T_COLOR_DATA data[]){
  

  typedef struct {
    UW sadr;
    W snd_size;
    UB *snd_data;
    W rcv_size;
    UB *rcv_data;
 } T_I2C_EXEC;
  

 //while(1){
   for(int s=0; s<4; s++){
      exec.sadr = sensor_addr[i];
      snd_data = 0x42;
      exec.snd_size = 1;
      exec.snd_data = &snd_data; 
      exec.rcv_size = 1;
      exec.rcv_data = &rcv_data;

      //tm_printf("%d\n",exec.sadr);

      err = tk_swri_dev(dd, TDN_I2C_EXEC, &exec, sizeof(T_I2C_EXEC),&asz);
      tm_printf("s1:%d\n",s);
 
 
      if(0!=(rcv_data & (1<<7))){
        exec.sadr = sensor_addr[i];
        snd_data = 0x50;
        exec.snd_size = 1;
        exec.snd_data = &snd_data; 
        exec.rcv_size = sizeof(sens_data);
        exec.rcv_data = (UB*)(&sens_data);

        err = tk_swri_dev(dd, TDN_I2C_EXEC, &exec, sizeof(T_I2C_EXEC),&asz);

        //tm_printf("s2:%d\n",s);

        
      }
      data[s].clear = sens_data[3];
      
      tm_printf("s3:%d\n",s);
     //break;
     
     
     } 
      
   // }
    }


EXPORT INT usermain(void){
  
 
  
  INT unit ;

  ID dd;
  dd = tk_opn_dev("iicb",TD_UPDATE);

  for( i= 0; i<SENSOR_MAX; i++) {
  data[0] = 0x40;
  data[1] = 0x80;

  err = tk_swri_dev(dd, sensor_addr[i], &data, 2, &asz);

  data[0] = 0x40;
  data[1] = 0x00;
  err = tk_swri_dev(dd, sensor_addr[i], &data, 2, &asz);

  data[0] = 0x42; 
  data[1] = 0x10;
  err = tk_swri_dev (dd, sensor_addr[i], &data, 2, &asz);

 while(1){

  //tk_slp_tsk(1000);

  read_sensor(dd, cdata);
  for (INT i = 0; i<SENSOR_MAX; i++ )
    tm_printf("%d: %d\n",i, cdata[i].clear);

  tk_slp_tsk(1000);

  }
  
  tk_slp_tsk(TMO_FEVR);
  return 0;

  }
}