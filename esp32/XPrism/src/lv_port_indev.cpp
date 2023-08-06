#include "lv_port_indev.h"

static void encoder_init(void);
static bool encoder_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data);
static void encoder_handler(void);

lv_indev_t *indev_encoder;

int32_t encoder_diff;
lv_indev_state_t encoder_state;

void lv_port_indev_init(void)
{
	lv_indev_drv_t indev_drv;

	encoder_init();

	lv_indev_drv_init(&indev_drv);
	indev_drv.type = LV_INDEV_TYPE_ENCODER;
	indev_drv.read_cb = encoder_read;
	indev_encoder = lv_indev_drv_register(&indev_drv);
}

static void encoder_init(void)
{
}

static bool encoder_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
	data->enc_diff = encoder_diff;
	data->state = encoder_state;

	encoder_diff = 0;
	return false;
}

static void encoder_handler(void)
{
	encoder_diff += 0;
	encoder_state = LV_INDEV_STATE_REL;
}
