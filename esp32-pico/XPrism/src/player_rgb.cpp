#include "player.h"
#include "common.h"

#define VIDEO_WIDTH 240L
#define VIDEO_HEIGHT 240L
#define MOVIE_BUFFER_SIZE 28800 // (57600)

#define TFT_MISO -1
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS -1 // Not connected
#define TFT_DC 2
#define TFT_RST 4 // Connect reset to ensure display initialises

RgbPlayDecoder::RgbPlayDecoder(File *file, bool isUseDMA)
{
    m_pFile = file;
    m_isUseDMA = isUseDMA;
    m_displayBuf = NULL;
    m_displayBufWithDma[0] = NULL;
    m_displayBufWithDma[1] = NULL;
    video_start();
}

RgbPlayDecoder::~RgbPlayDecoder(void)
{
    Serial.println(F("~RgbPlayDecoder"));
    // 释放资源
    video_end();
}

bool RgbPlayDecoder::video_start()
{
    if (m_isUseDMA)
    {
        m_displayBufWithDma[0] = (uint8_t *)heap_caps_malloc(MOVIE_BUFFER_SIZE, MALLOC_CAP_DMA);
        m_displayBufWithDma[1] = (uint8_t *)heap_caps_malloc(MOVIE_BUFFER_SIZE, MALLOC_CAP_DMA);
        m_tft->initDMA();
    }
    else
    {
        m_displayBuf = (uint8_t *)malloc(MOVIE_BUFFER_SIZE);
        m_tft->setAddrWindow((m_tft->width() - VIDEO_WIDTH) / 2,
                             (m_tft->height() - VIDEO_HEIGHT) / 2,
                             VIDEO_WIDTH, VIDEO_HEIGHT);
    }
    return true;
}

bool RgbPlayDecoder::video_play_screen(void)
{
    // Read video
    uint32_t l = 0;
    unsigned long Millis_1 = 0; // 更新的时间

    if (m_isUseDMA)
    {
        // 80M主频大概200ms一帧 240M大概150ms一帧
        uint8_t *dst = NULL;
        dst = m_displayBufWithDma[0];
        m_pFile->read(dst, MOVIE_BUFFER_SIZE);
        m_tft->pushImageDMA(0, 0, 240, 60, (uint16_t *)dst, nullptr);

        dst = m_displayBufWithDma[1];
        m_pFile->read(dst, MOVIE_BUFFER_SIZE);
        m_tft->pushImageDMA(0, 60, 240, 60, (uint16_t *)dst, nullptr);

        dst = m_displayBufWithDma[0];
        m_pFile->read(dst, MOVIE_BUFFER_SIZE);
        m_tft->pushImageDMA(0, 120, 240, 60, (uint16_t *)dst, nullptr);

        dst = m_displayBufWithDma[1];
        m_pFile->read(dst, MOVIE_BUFFER_SIZE);
        m_tft->pushImageDMA(0, 180, 240, 60, (uint16_t *)dst, nullptr);
    }
    else
    {

        m_tft->startWrite();
        Millis_1 = GET_SYS_MILLIS();
        l = m_pFile->read(m_displayBuf, MOVIE_BUFFER_SIZE);
        Serial.println(GET_SYS_MILLIS() - Millis_1);
        Millis_1 = GET_SYS_MILLIS();
        m_tft->pushColors(m_displayBuf, l);
        Serial.println(GET_SYS_MILLIS() - Millis_1);
        Millis_1 = GET_SYS_MILLIS();
        l = m_pFile->read(m_displayBuf, MOVIE_BUFFER_SIZE);
        Serial.println(GET_SYS_MILLIS() - Millis_1);
        Millis_1 = GET_SYS_MILLIS();
        m_tft->pushColors(m_displayBuf, l);
        Serial.println(GET_SYS_MILLIS() - Millis_1);

        l = m_pFile->read(m_displayBuf, MOVIE_BUFFER_SIZE);
        m_tft->pushColors(m_displayBuf, l);
        l = m_pFile->read(m_displayBuf, MOVIE_BUFFER_SIZE);
        m_tft->pushColors(m_displayBuf, l);

        m_tft->endWrite();
    }
    return true;
}

bool RgbPlayDecoder::video_end(void)
{
    m_pFile = NULL;
    // 结束播放 释放资源
    if (m_isUseDMA)
    {
        if (NULL != m_displayBufWithDma[0])
        {
            free(m_displayBufWithDma[0]);
            free(m_displayBufWithDma[1]);
            m_displayBufWithDma[0] = NULL;
            m_displayBufWithDma[1] = NULL;
        }
    }
    else
    {
        if (NULL != m_displayBuf)
        {
            free(m_displayBuf);
            m_displayBuf = NULL;
        }
    }

    return true;
}