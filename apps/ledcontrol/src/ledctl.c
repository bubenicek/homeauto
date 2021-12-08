
#include "system.h"
#include "ledctl.h"
#include "led_strip.h"

#define TRACE_TAG "ledctl"
#if !ENABLE_TRACE_MAIN
#undef TRACE
#define TRACE(...)
#endif

#define LED_STRIP_RMT_INTR_NUM 		        19


typedef struct
{
    osTimerId timer;
    uint32_t stop_time;

    struct
    {
        int r;
        int g;
        int b;

    } step;

} channel_ramp_timer_t;



// Locals:
static struct led_color_t led_strip0_buf_1[CFG_LEDCTL_CHANNEL0_LENGTH];
static struct led_color_t led_strip0_buf_2[CFG_LEDCTL_CHANNEL0_LENGTH];

static struct led_color_t led_strip1_buf_1[CFG_LEDCTL_CHANNEL1_LENGTH];
static struct led_color_t led_strip1_buf_2[CFG_LEDCTL_CHANNEL1_LENGTH];

static struct led_color_t led_strip2_buf_1[CFG_LEDCTL_CHANNEL2_LENGTH];
static struct led_color_t led_strip2_buf_2[CFG_LEDCTL_CHANNEL2_LENGTH];

static struct led_color_t led_strip3_buf_1[CFG_LEDCTL_CHANNEL3_LENGTH];
static struct led_color_t led_strip3_buf_2[CFG_LEDCTL_CHANNEL3_LENGTH];


static struct led_strip_t led_strip[CFG_LEDCTL_NUM_CHANNELS] =
{
    {
        .rgb_led_type = RGB_LED_TYPE_WS2812,
        .rmt_channel = RMT_CHANNEL_1,
        .rmt_interrupt_num = LED_STRIP_RMT_INTR_NUM,
        .gpio = CFG_LEDCTL_CHANNEL0_GPIO,
        .led_strip_buf_1 = led_strip0_buf_1,
        .led_strip_buf_2 = led_strip0_buf_2,
        .led_strip_length = CFG_LEDCTL_CHANNEL0_LENGTH
    },

    {
        .rgb_led_type = RGB_LED_TYPE_WS2812,
        .rmt_channel = RMT_CHANNEL_2,
        .rmt_interrupt_num = LED_STRIP_RMT_INTR_NUM,
        .gpio = CFG_LEDCTL_CHANNEL1_GPIO,
        .led_strip_buf_1 = led_strip1_buf_1,
        .led_strip_buf_2 = led_strip1_buf_2,
        .led_strip_length = CFG_LEDCTL_CHANNEL1_LENGTH
    },

    {
        .rgb_led_type = RGB_LED_TYPE_WS2812,
        .rmt_channel = RMT_CHANNEL_3,
        .rmt_interrupt_num = LED_STRIP_RMT_INTR_NUM,
        .gpio = CFG_LEDCTL_CHANNEL2_GPIO,
        .led_strip_buf_1 = led_strip2_buf_1,
        .led_strip_buf_2 = led_strip2_buf_2,
        .led_strip_length = CFG_LEDCTL_CHANNEL2_LENGTH
    },

    {
        .rgb_led_type = RGB_LED_TYPE_WS2812,
        .rmt_channel = RMT_CHANNEL_4,
        .rmt_interrupt_num = LED_STRIP_RMT_INTR_NUM,
        .gpio = CFG_LEDCTL_CHANNEL3_GPIO,
        .led_strip_buf_1 = led_strip3_buf_1,
        .led_strip_buf_2 = led_strip3_buf_2,
        .led_strip_length = CFG_LEDCTL_CHANNEL3_LENGTH
    },

};

/** Initialize LED controller */
int ledctl_init(void)
{
    int ix;

    for (ix = 0; ix < CFG_LEDCTL_NUM_CHANNELS; ix++)
    {
        led_strip[ix].access_semaphore = xSemaphoreCreateBinary();

        if (!led_strip_init(&led_strip[ix]))
        {
            TRACE_ERROR("LED strip channel: %d init failed", ix);
            return -1;
        }

        ledctl_clear(ix);
    }

	TRACE("LED strip init ok");

    return 0;
}

/** Set pixel */
int ledctl_set_pixel(ledctl_channel_t channel, uint32_t pixel_num, uint8_t red, uint8_t green, uint8_t blue)
{
    int ix;
    int res = 0;

    if (channel < CFG_LEDCTL_NUM_CHANNELS)
    {
        res = led_strip_set_pixel_rgb(&led_strip[channel], pixel_num, red, blue, green) ? 0 : -1;
    }
    else if (channel == LEDCTL_CHANNEL_ALL)
    {
        for (ix = 0; ix < CFG_LEDCTL_NUM_CHANNELS; ix++)
        {
            res += led_strip_set_pixel_rgb(&led_strip[ix], pixel_num, red, blue, green) ? 0 : -1;
        }
    }
    else
    {
        res = -1;
    }

    return res;
}

int ledctl_update_pixel(ledctl_channel_t channel, uint32_t pixel_num, uint8_t red, uint8_t green, uint8_t blue)
{
    int ix;
    int res = 0;

    if (channel < CFG_LEDCTL_NUM_CHANNELS)
    {
        res = led_strip_set_pixel_rgb(&led_strip[channel], pixel_num, red, blue, green) ? 0 : -1;
        led_strip_show(&led_strip[channel]);
    }
    else if (channel == LEDCTL_CHANNEL_ALL)
    {
        for (ix = 0; ix < CFG_LEDCTL_NUM_CHANNELS; ix++)
        {
            res += led_strip_set_pixel_rgb(&led_strip[ix], pixel_num, red, blue, green) ? 0 : -1;
        }

        for (ix = 0; ix < CFG_LEDCTL_NUM_CHANNELS; ix++)
        {
            led_strip_show(&led_strip[ix]);
        }
    }
    else
    {
        res = -1;
    }

    return res;
}


int ledctl_update_channel_pixels(ledctl_channel_t channel, uint8_t red, uint8_t green, uint8_t blue)
{
    int ix, iy;
    int res = 0;

    if (channel < CFG_LEDCTL_NUM_CHANNELS)
    {
        res = 0;
        for (ix = 0; ix < led_strip[channel].led_strip_length; ix++)
        {
            res += led_strip_set_pixel_rgb(&led_strip[channel], ix, red, blue, green) ? 0 : -1;
        }

        led_strip_show(&led_strip[channel]);
    }
    else if (channel == LEDCTL_CHANNEL_ALL)
    {
        for (ix = 0; ix < CFG_LEDCTL_NUM_CHANNELS; ix++)
        {
            for (iy = 0; iy < led_strip[ix].led_strip_length; iy++)
            {
                res += led_strip_set_pixel_rgb(&led_strip[ix], iy, red, blue, green) ? 0 : -1;
            }
        }

        for (ix = 0; ix < CFG_LEDCTL_NUM_CHANNELS; ix++)
        {
            led_strip_show(&led_strip[ix]);
        }
    }
    else
    {
        res = -1;
    }

    return res;
}

int ledctl_update_channel_pixels_ramp(ledctl_channel_t channel, uint32_t timelen, uint8_t red, uint8_t green, uint8_t blue)
{
    int ix, iy;
    int res = 0;

    if (channel < CFG_LEDCTL_NUM_CHANNELS)
    {
        res = 0;
        for (ix = 0; ix < led_strip[channel].led_strip_length; ix++)
        {
            res += led_strip_set_pixel_rgb(&led_strip[channel], ix, red, blue, green) ? 0 : -1;
        }

        led_strip_show(&led_strip[channel]);
    }
    else if (channel == LEDCTL_CHANNEL_ALL)
    {
        for (ix = 0; ix < CFG_LEDCTL_NUM_CHANNELS; ix++)
        {
            for (iy = 0; iy < led_strip[ix].led_strip_length; iy++)
            {
                res += led_strip_set_pixel_rgb(&led_strip[ix], iy, red, blue, green) ? 0 : -1;
            }
        }

        for (ix = 0; ix < CFG_LEDCTL_NUM_CHANNELS; ix++)
        {
            led_strip_show(&led_strip[ix]);
        }
    }
    else
    {
        res = -1;
    }

    return res;
}

/** Refresh pixels */
void ledctl_refresh(ledctl_channel_t channel)
{
    int ix;

    if (channel < CFG_LEDCTL_NUM_CHANNELS)
    {
        led_strip_show(&led_strip[channel]);
    }
    else if (channel == LEDCTL_CHANNEL_ALL)
    {
        for (ix = 0; ix < CFG_LEDCTL_NUM_CHANNELS; ix++)
        {
            led_strip_show(&led_strip[ix]);
        }
    }
}

/** Clear pixels */
void ledctl_clear(ledctl_channel_t channel)
{
    int ix;

    if (channel < CFG_LEDCTL_NUM_CHANNELS)
    {
        led_strip_clear(&led_strip[channel]);
    }
    else
    {
        for (ix = 0; ix < CFG_LEDCTL_NUM_CHANNELS; ix++)
        {
            led_strip_clear(&led_strip[ix]);
        }
    }
}

/** Run simple test */
void ledctl_run_test(void)
{
    int pixel;
    int chan;

	while(1)
	{
        for (chan = 0; chan < CFG_LEDCTL_NUM_CHANNELS; chan++)
        {
            for (pixel = 0; pixel < CFG_LEDCTL_CHANNEL0_LENGTH; pixel++)
            {
             ledctl_set_pixel(chan, pixel, 0, 0, 5);
            }
            ledctl_refresh(chan);
            hal_delay_ms(500);

            for (pixel = 0; pixel < CFG_LEDCTL_CHANNEL0_LENGTH; pixel++)
            {
             ledctl_set_pixel(chan, pixel, 0, 0, 0);
            }
            ledctl_refresh(chan);
            hal_delay_ms(500);
        }
    }
}
