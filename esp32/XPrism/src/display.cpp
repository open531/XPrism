#include "display.h"
#include "system.h"
#include "lvgl.h"

static lv_disp_draw_buf_t disp_buf;
static lv_disp_drv_t disp_drv;
static lv_color_t buf[SCREEN_WIDTH * LV_HOR_RES_MAX_LEN];

void xprism_disp_flush(lv_disp_drv_t *disp_drv,
                       const lv_area_t *area,
                       lv_color_t *color_p)
{
    uint16_t c;
    m_tft->startWrite();
    m_tft->setAddrWindow(area->x1, area->y1,
                         area->x2 - area->x1 + 1,
                         area->y2 - area->y1 + 1);
    for (int y = area->y1; y <= area->y2; y++)
    {
        for (int x = area->x1; x <= area->x2; x++)
        {
            c = color_p->full;
            m_tft->writeColor(c, 1);
            color_p++;
        }
    }
    m_tft->endWrite();
    lv_disp_flush_ready(disp_drv);
}

void screenInit(uint8_t rotation, uint8_t backLight)
{
    ledcSetup(0, 5000, 8);
    ledcAttachPin(PIN_DISPLAY_BLK, 0);
    lv_init();
    screenSetBackLight(0.0);
    m_tft->begin();
    m_tft->fillScreen(TFT_BLACK);
    m_tft->writecommand(ST7789_DISPON);
    m_tft->setRotation(rotation);
    screenSetBackLight(backLight / 100.0);
    lv_disp_draw_buf_init(&disp_buf, buf, NULL,
                          SCREEN_WIDTH * LV_HOR_RES_MAX_LEN);
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = SCREEN_WIDTH;
    disp_drv.ver_res = SCREEN_HEIGHT;
    disp_drv.flush_cb = xprism_disp_flush;
    disp_drv.draw_buf = &disp_buf;
    disp_drv.user_data = m_tft;
    lv_disp_drv_register(&disp_drv);
}

void screenRoutine()
{
    LVGL_OPERATE_LOCK(lv_timer_handler());
}

void screenSetBackLight(float brightness)
{
    ledcWrite(0, brightness * 255);
}
