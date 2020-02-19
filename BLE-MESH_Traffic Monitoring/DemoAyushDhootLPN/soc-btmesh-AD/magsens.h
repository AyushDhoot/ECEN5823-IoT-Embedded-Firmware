/*
 * #define MAGsens.h
 *
 *  Created on: 18-Nov-2018
 *      Author: Ayush
 */

#ifndef MAGSENS_H_
#define  MAGSENS_H_


/*
int16_t data_receive=0;
int16_t Xaxis=0;
int16_t Yaxis=0;
int16_t Zaxis=0;
*/


#define SENSITIVITY (0.00048828125)



#define MAG_WHO_AM_I (0x0F);
#define MAG_CTRL_REG1  (0x20);
#define MAG_CTRL_REG2  (0x21);
#define MAG_CTRL_REG3  (0x22);
#define MAG_CTRL_REG4  (0x23);
#define MAG_CTRL_REG5  (0x24);
#define MAG_STATUS_REG (0x27);
#define MAG_OUTX_L     (0x28);
#define MAG_OUTX_H     (0x29);
#define MAG_OUTY_L     (0x2A);
#define MAG_OUTY_H     (0x2B);
#define MAG_OUTZ_L     (0x2C);
#define MAG_OUTZ_H     (0x2D);
#define MAG_TEMP_OUT_L (0x2E);
#define MAG_TEMP_OUT_H (0x2F);
#define MAG_INT_CFG    (0x30);
#define MAG_INT_SRC    (0x31);
#define MAG_INT_THS_L  (0x32);
#define MAG_INT_THS_H  (0x33);


#define MAG_TEMP_EN_DISABLE (0x00);
#define MAG_TEMP_EN_ENABLE  (0x80);


#define MAG_DO_0_625_Hz (0x00);
#define MAG_DO_1_25_Hz  (0x04);
#define MAG_DO_2_5_Hz   (0x08);
#define MAG_DO_5_Hz     (0x0C);
#define MAG_DO_10_Hz    (0x10);
#define MAG_DO_20_Hz    (0x14);
#define MAG_DO_40_Hz    (0x18);
#define MAG_DO_80_Hz    (0x1C);


  #define MAG_FS_4_Ga   (0x00);
  #define MAG_FS_8_Ga   (0x20);
  #define MAG_FS_12_Ga  (0x40);
  #define MAG_FS_16_Ga  (0x60);


  #define MAG_BDU_DISABLE (0x00);
  #define MAG_BDU_ENABLE  (0x40);


  #define MAG_OMXY_LOW_POWER              (0x00);
  #define MAG_OMXY_MEDIUM_PERFORMANCE     (0x20);
  #define MAG_OMXY_HIGH_PERFORMANCE       (0x40);
  #define MAG_OMXY_ULTRA_HIGH_PERFORMANCE (0x60);


  #define MAG_OMZ_LOW_PW                  (0x00);
  #define MAG_OMZ_MEDIUM_PERFORMANCE      (0x04);
  #define MAG_OMZ_HIGH_PERFORMANCE        (0x08);
  #define MAG_OMZ_ULTRA_HIGH_PERFORMANCE  (0x0C);


  #define MAG_MD_CONTINUOUS   (0x00);
  #define MAG_MD_SINGLE       (0x01);
  #define MAG_MD_POWER_DOWN_1 (0x02);
  #define MAG_MD_POWER_DOWN_2 (0x03);


#endif /* #define MAGSENS_H_ */
