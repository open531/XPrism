#ifndef LV_PORT_INDEV_TEMPL_H
#define LV_PORT_INDEV_TEMPL_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "lvgl.h"

	extern lv_indev_t *indev_encoder;

	void lv_port_indev_init(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_PORT_INDEV_TEMPL_H*/
