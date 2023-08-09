#include "system.h"

Buttons m_button;
// Display m_display;
FlashFS m_flashFS;
Network m_network;
SDCard m_sdCard;
TFT_eSPI *m_tft = new TFT_eSPI(240, 240);
