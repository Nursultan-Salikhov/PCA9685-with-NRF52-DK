# PCA9685-with-NRF52-DK
Driver for controlling a PWM module PCA9685 using NRF52-DK
The code was written based on this repository 
[https://github.com/henriheimann/stm32-hal-pca9685](url)

# Usage
First you need to set up the configuration file and enable TWI(i2c)
You can also find an example in SDK (\\\\nRF5_SDK_17.0.2_d674dde\examples\peripheral\twi_sensor)
``` 
#define TWI_INSTANCE_ID     0
const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);
volatile bool m_xfer_done = false;

pca9685_handle_t m_pcah = {
    .address = PCA9685_ADDRESS,
    .twi_handler = &m_twi,
    .m_xfer_done = &m_xfer_done};
```

A prerequisite for the driver to work is setting the flag to true in the interrupt handler
```
void twi_handler(nrf_drv_twi_evt_t const * p_event, void * p_context)
{
    switch (p_event->type)
    {
        case NRF_DRV_TWI_EVT_DONE:
            m_xfer_done = true;
            break;
        default:
            break;
    }
}
```
In the main loop, after initializing TWI, we call the following functions
```
    pca9685_init(&m_pcah);
    pca9685_set_pwm_frequency(&m_pcah, 100.0);
    pca9685_set_channel_pwm(&m_pcah,0,0,3000);
```
 If you did everything correctly, then the LED should light up at half brightness. The functions do not have any internal checks on the incoming data.  
