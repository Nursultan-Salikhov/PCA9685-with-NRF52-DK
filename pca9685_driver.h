
#ifndef PCA9685_DRIVER_H__
#define PCA9685_DRIVER_H__

#include <stdint.h>
#include <string.h>
#include "nrf_drv_twi.h"
#include "nrf_delay.h"
#include "app_error.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define PCA9685_ADDRESS	      0x40
#define PCA9685_SET_BIT_MASK(BYTE, MASK)      ((BYTE) |= (uint8_t)(MASK))
#define PCA9685_CLEAR_BIT_MASK(BYTE, MASK)    ((BYTE) &= (uint8_t)(~(uint8_t)(MASK)))
#define PCA9685_READ_BIT_MASK(BYTE, MASK)     ((BYTE) & (uint8_t)(MASK))


typedef enum
{
	PCA9685_REGISTER_MODE1_SLEEP = (1u << 4u),
	PCA9685_REGISTER_MODE1_RESTART = (1u << 7u)
} pca9685_register_mode1_t;

      typedef enum
{
	PCA9685_REGISTER_MODE1 = 0x00,
	PCA9685_REGISTER_MODE2 = 0x01,
	PCA9685_REGISTER_LED0_ON_L = 0x06,
	PCA9685_REGISTER_ALL_LED_ON_L = 0xFA,
	PCA9685_REGISTER_ALL_LED_ON_H = 0xFB,
	PCA9685_REGISTER_ALL_LED_OFF_L = 0xFC,
	PCA9685_REGISTER_ALL_LED_OFF_H = 0xFD,
	PCA9685_REGISTER_PRESCALER = 0xFE
} pca9685_register_t;

      
typedef struct{
  
  uint8_t address;
  
  const nrf_drv_twi_t *twi_handler;
  
  volatile bool *m_xfer_done;
  
}pca9685_handle_t;

//--------------------------------------------------------------------------

void pca9685_set_pwm_frequency(pca9685_handle_t *pcah, float freq_in);

void pca9685_init (pca9685_handle_t *pcah);

void pca9685_set_channel_pwm (pca9685_handle_t *pcah, uint8_t channel, uint16_t on_time, uint16_t off_time);

#endif /* -PCA9685_DRIVER_H__-*/