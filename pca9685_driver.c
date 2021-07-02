#include "pca9685_driver.h"


void pca9685_write_u8 (pca9685_handle_t *pcah, uint8_t reg, uint8_t data)
{
  *pcah->m_xfer_done = false;
   uint8_t buff[2] = {reg, data};
   uint32_t err_code = nrf_drv_twi_tx(pcah->twi_handler,pcah->address, buff, 2, false);
   APP_ERROR_CHECK(err_code);
   while(*pcah->m_xfer_done == false);
   NRF_LOG_INFO("pca9685_write_u8: reg = %d: data = %d: err_code = %d .",reg,data, err_code);
   NRF_LOG_FLUSH();
}
 
void pca9685_write_data (pca9685_handle_t *pcah, uint8_t reg, uint8_t *data, uint8_t size)
{
   //m_xfer_done = false;
   *pcah->m_xfer_done = false;
   uint8_t buff[5] = {reg};
   memcpy(&buff[1],data,size);
   uint32_t err_code = nrf_drv_twi_tx(pcah->twi_handler,pcah->address,buff,size+1,false);
   APP_ERROR_CHECK(err_code);
   while(*pcah->m_xfer_done == false);
}
  
uint8_t pca9685_read_reg (pca9685_handle_t *pcah, uint8_t reg)
{
   uint8_t data = reg;
   *pcah->m_xfer_done = false;
   uint32_t err_code = nrf_drv_twi_tx(pcah->twi_handler,pcah->address, &data, 1, false);
   printf("pca_read reg = %d: err_code = %d",reg,err_code);
   NRF_LOG_FLUSH();
   APP_ERROR_CHECK(err_code);
   while(*pcah->m_xfer_done == false);
 

  *pcah->m_xfer_done = false;
   err_code = nrf_drv_twi_rx(pcah->twi_handler,pcah->address, &data, 1);
   APP_ERROR_CHECK(err_code);
   while (*pcah->m_xfer_done == false);
   NRF_LOG_INFO("pca_read data = %d: err_code = %d " ,data,err_code );
   NRF_LOG_FLUSH();
   return data;
}

_Bool pca9685_is_sleeping (pca9685_handle_t *pcah)
{
   uint8_t mode1_reg_data = pca9685_read_reg(pcah, PCA9685_REGISTER_MODE1);
   _Bool res = PCA9685_READ_BIT_MASK(mode1_reg_data,PCA9685_REGISTER_MODE1_SLEEP);
   return res;   
}

void pca9685_sleep (pca9685_handle_t *pcah)
{
  NRF_LOG_INFO("pca_sleep START");
  NRF_LOG_FLUSH();
  uint8_t mode1_data = pca9685_read_reg(pcah, PCA9685_REGISTER_MODE1);
  PCA9685_CLEAR_BIT_MASK(mode1_data, PCA9685_REGISTER_MODE1_RESTART);
  PCA9685_SET_BIT_MASK( mode1_data,PCA9685_REGISTER_MODE1_SLEEP);
  pca9685_write_u8(pcah, PCA9685_REGISTER_MODE1, mode1_data);
  NRF_LOG_INFO("pca_sleep END");
}

void pca9685_wakeup(pca9685_handle_t *pcah)
{
  NRF_LOG_INFO("pca_wakeup START");
  NRF_LOG_FLUSH();
  uint8_t mode1_data = pca9685_read_reg(pcah, PCA9685_REGISTER_MODE1);
  _Bool restart_req = PCA9685_READ_BIT_MASK(mode1_data, PCA9685_REGISTER_MODE1_RESTART);
  PCA9685_CLEAR_BIT_MASK(mode1_data, PCA9685_REGISTER_MODE1_RESTART);
  PCA9685_CLEAR_BIT_MASK(mode1_data, PCA9685_REGISTER_MODE1_SLEEP);
  pca9685_write_u8(pcah,PCA9685_REGISTER_MODE1,mode1_data);
  if(restart_req)
  {
    nrf_delay_us(10000);
    PCA9685_SET_BIT_MASK(mode1_data,PCA9685_REGISTER_MODE1_RESTART);
    pca9685_write_u8(pcah, PCA9685_REGISTER_MODE1,mode1_data);
  }
  NRF_LOG_INFO("pca_wakeup END");
  NRF_LOG_FLUSH();
}

void pca9685_set_pwm_frequency(pca9685_handle_t *pcah, float freq_in)
{
  NRF_LOG_INFO("set PWM frequency START")
  NRF_LOG_FLUSH();
  uint8_t prescaler = (uint8_t) roundf(25000000.0f/(4096 *freq_in))-1;
  _Bool already_sleeping = pca9685_is_sleeping(pcah);
  if (!already_sleeping)
  {
    pca9685_sleep(pcah);
  }
  pca9685_write_u8(pcah, PCA9685_REGISTER_PRESCALER, prescaler);
  
  if (!already_sleeping) pca9685_wakeup(pcah);
  NRF_LOG_INFO("set PWM frequincy END")
  NRF_LOG_FLUSH();
}


void pca9685_init (pca9685_handle_t *pcah)
{
   NRF_LOG_INFO("pca_init START");
   NRF_LOG_FLUSH();
   uint8_t mode1_data = 0b00110000;
   uint8_t mode2_data = 0b00000100;
   pca9685_write_u8(pcah, PCA9685_REGISTER_MODE1,mode1_data);
   pca9685_write_u8(pcah, PCA9685_REGISTER_MODE2, mode2_data);
   
   uint8_t m_data[4] = {0x00,0x00,0x00,0x10};
   pca9685_write_data(pcah, PCA9685_REGISTER_ALL_LED_ON_L,&m_data,4);
   pca9685_set_pwm_frequency(pcah, 100.0);
   pca9685_wakeup(pcah);
    pca9685_write_u8(pcah, PCA9685_REGISTER_MODE2, mode2_data);
   NRF_LOG_INFO("pca_init END");
   NRF_LOG_FLUSH();
}
  
void pca9685_set_channel_pwm (pca9685_handle_t *pcah, uint8_t channel, uint16_t on_time, uint16_t off_time)
{
   uint8_t buff[4] = {on_time, on_time >> 8, off_time, off_time >> 8};
   pca9685_write_data(pcah, PCA9685_REGISTER_LED0_ON_L + channel * 4, &buff, 4);
 //  NRF_LOG_INFO("setPWM: channel = %d; on = %d; off = %d;" ,channel, on_time,off_time);
 //  NRF_LOG_FLUSH();
}
  
  

  

  

  
 /* 
  void read_reg (uint8_t reg)
  {
       	      nrf_delay_us(20000);
	     uint8_t ff  =reg;
      nrf_drv_twi_tx(&m_twi,PCA9685_ADDRESS,&ff, sizeof(ff), false);
       nrf_delay_us(20000);
       
          m_xfer_done = false;
    nrf_drv_twi_rx(&m_twi, PCA9685_ADDRESS, &ff, 1);
    while (m_xfer_done == false); 
    
                NRF_LOG_INFO("check: %d == %d .",reg,ff);
	 NRF_LOG_FLUSH();
  }
  */