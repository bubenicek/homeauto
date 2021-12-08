
#ifndef __LEDCTL_H
#define __LEDCTL_H

typedef enum
{
    LEDCTL_CHANNEL0,
    LEDCTL_CHANNEL1,
    LEDCTL_CHANNEL2,
    LEDCTL_CHANNEL3,
    LEDCTL_CHANNEL_ALL = 0xFF

} ledctl_channel_t;


int ledctl_init(void);

int ledctl_set_pixel(ledctl_channel_t channel, uint32_t pixel_num, uint8_t red, uint8_t green, uint8_t blue);

int ledctl_update_pixel(ledctl_channel_t channel, uint32_t pixel_num, uint8_t red, uint8_t green, uint8_t blue);

int ledctl_update_channel_pixels(ledctl_channel_t channel, uint8_t red, uint8_t green, uint8_t blue);
int ledctl_update_channel_pixels_ramp(ledctl_channel_t channel, uint32_t timelen, uint8_t red, uint8_t green, uint8_t blue);

void ledctl_clear(ledctl_channel_t channel);

void ledctl_refresh(ledctl_channel_t channel);

void ledctl_run_test(void);


#endif  // __LEDCTL_H
