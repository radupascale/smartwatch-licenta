#pragma once
#include <Arduino_GFX_Library.h>

#define SCK DF_GFX_SCK
#define MOSI DF_GFX_MOSI
#define MISO DF_GFX_MISO
#define TFT_SS DF_GFX_CS
#define SD_SS 3

/* TODO: Change spi freq */
#define SPI_SPEED SD_SCK_MHZ(4)

int sd_init();