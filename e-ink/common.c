/*
 * common.cpp
 *
 * 2024 FEB 22
 * 		The very first release
 * 2024 SEP 13
 *  	Implemented 3-color driver (R-B-W) through single memory buffer
 *  	active_layer variable used to store the current color layer index (Red or Black) the drawing routines are writing to.
 *  	The variable active_layer is initialized by EPD_Init() depending on layer number of the display type (B-W or R-B-W) and full parameter.
 *  	In case of single-memory-buffer mode (layers > 1 & !full) active_layer initialized to EPD_LAYERS.
 *  	Otherwise (B-W display or full parameter is true) the EPD_Init() tries to allocate separate memory buffer for each color layer.
 *  	If the memory amount is sufficient, the full-memory-buffer mode is initialized and active_layer variable initialized to 0.
 *  	To start display the image to the screen in single-memory-buffer mode, the EPD_FirstPage() should be called first.
 *  	As soon as first layer activated by the EPD_FirstPage(), it is possible to start draw the image using common display routines such as
 *  	EPD_DrawPixel(), EPD_DrawLine() and so on. The only black points will be written to the memory buffer according to active_layer variable.
 *  	When all the drawing finished, the EPD_TurnPage() should be called to send the data from the memory buffer to the screen. The active_layer
 *  	will be incremented by 1 to activate next layer (Red). Then whole drawing routines should be repeated one more time to write red points to the memory buffer. And the EPD_TurnPage() should be called
 *  	again to finish the screen output.
 */

#include <common.h>
#include <stdlib.h>
#include <stdbool.h>

// Static image data
static uint8_t		*image[EPD_LAYERS];						// Display layer data, allocated dynamically
static uint8_t		layers;									// Actual number of color layers. 1 for BW display, 2 for Black-White-Red display
static uint16_t		width;									// Display width (pixels). No display rotation.
static uint16_t		height;									// Display height (pixels). No display rotation.
static Rotation_t	rotate;									// Display rotation angle. Used to translate coordinates of pixel to hardware position
static uint8_t		active_layer = EPD_LAYERS;				// In case of consequently drawing layers used as current layer number [0;layers)

// Local forward function declarations
static void EPD_DrawAbsolutePixel(uint16_t x, uint16_t y, COLOR c);

#ifndef TINY_LIB
static void	EPD_DrawAbsoluteHLine(uint16_t x, uint16_t y, uint16_t line_width, COLOR c);
static void EPD_DrawAbsoluteVLine(uint16_t x, uint16_t y, uint16_t line_height, COLOR c);
#endif

bool EPD_Init(uint16_t w, uint16_t h, uint8_t lay, bool full) {
	if (lay > EPD_LAYERS)
		return false;

	uint16_t w1 = w % 8 ? w + 8 - (w % 8) : w;				// Align the width to whole byte
	rotate	= ROTATE_0;
	width	= w;
	height	= h;
	layers	= lay;
	bool ok = true;
	if (lay > 1 && !full) {									// Allocate single memory buffer for the display
		active_layer	= 0;								// Use single memory buffer for all color layers to save memory
		lay				= 1;
	} else {												// Allocate memory for each color layer separately
		active_layer	= EPD_LAYERS;						// No layered screen, draw display image in a single step
	}
	for (uint8_t l = 0; l < lay; ++l) {
		if (image[l]) {
			if (width == w && height == h)
				continue;
			free(image[l]);
			image[l] = 0;

		}
		image[l] = (uint8_t *)malloc(h*w1/8);
		if (!image[l]) {
			ok = false;
			break;
		}
	}
	if (!ok) {												// Failed to allocate all layers. Free allocated memory
	    for (uint8_t l = 0; l < lay; ++l) {
	    	if (image[l])
	    		free(image[l]);
	    	image[l] = 0;									// No memory allocated!
	    }
	    width	= 0;										// The display failed to initialize
	    height	= 0;
	    layers	= 0;
	}
	EPD_Clear(MODE_FULL);
	return ok;
}

uint16_t EPD_Width(void) {
	if (rotate == ROTATE_0 || rotate == ROTATE_180)
		return width;
	else
		return height;
}

uint16_t EPD_Height(void) {
	if (rotate == ROTATE_0 || rotate == ROTATE_180)
		return height;
	else
		return width;
}

void EPD_Clear(COLOR c) {
	if (active_layer < EPD_LAYERS) {						// Use single memory buffer for all color layers
		for (uint16_t x = 0; x < height*width/8; ++x) {		// Clear the memory buffer anyway
			image[0][x] = 0;
		}
	} else {
		uint8_t lm = 1;
		for (uint8_t l = 0; l < layers; ++l) {
			if ((c & lm) && image[l]) {
				for (uint16_t x = 0; x < height*width/8; ++x) {
					image[l][x] = 0;
				}
			}
			lm <<= 1;
		}
	}
}

void EPD_DrawPixel(uint16_t x, uint16_t y, COLOR c) {
    if (rotate == ROTATE_0) {
        if (x >= width || y >= height) return;
        EPD_DrawAbsolutePixel(x, y, c);
    } else if (rotate == ROTATE_90) {
        if (x >= height || y >= width) return;
        EPD_DrawAbsolutePixel(width-y, x, c);
    } else if (rotate == ROTATE_180) {
        if (x >= width || y >= height) return;
        EPD_DrawAbsolutePixel(width-x, height-y, c);
    } else if (rotate == ROTATE_270) {
        if (x >= height || y >= width) return;
        EPD_DrawAbsolutePixel(y, height-x, c);
    }
}

void EPD_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, COLOR c) {
    /* Bresenham algorithm */
    int16_t dx = x1 - x0 >= 0 ? x1 - x0 : x0 - x1;
    int16_t sx = x0 < x1 ? 1 : -1;
    int16_t dy = y1 - y0 <= 0 ? y1 - y0 : y0 - y1;
    int16_t sy = y0 < y1 ? 1 : -1;
    int16_t err = dx + dy;

    while ((x0 != x1) && (y0 != y1)) {
        EPD_DrawPixel(x0, y0 , c);
        if (2 * err >= dy) {
            err += dy;
            x0 += sx;
        }
        if (2 * err <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

#ifdef TINY_LIB
void EPD_DrawHLine(uint16_t x, uint16_t y, uint16_t line_width, COLOR c) {
	for (uint16_t i = 0; i < line_width; ++i)
		EPD_DrawPixel(x+i, y, c);
}

void EPD_DrawVLine(uint16_t x, uint16_t y, uint16_t line_height, COLOR c) {
	for (uint16_t i = 0; i < line_height; ++i)
		EPD_DrawPixel(x, y+i, c);
}

#else

void EPD_DrawHLine(uint16_t x, uint16_t y, uint16_t line_width, COLOR c) {
    if (rotate == ROTATE_0) {
        EPD_DrawAbsoluteHLine(x, y, line_width, c);
    } else if (rotate == ROTATE_90) {
        EPD_DrawAbsoluteVLine(width-y, x, line_width, c);
    } else if (rotate == ROTATE_180) {
        EPD_DrawAbsoluteHLine(width-line_width-x, height-y, line_width, c);
    } else if (rotate == ROTATE_270) {
        EPD_DrawAbsoluteVLine(y, height-line_width-x, line_width, c);
    }
}

void EPD_DrawVLine(uint16_t x, uint16_t y, uint16_t line_height, COLOR c) {
    if (rotate == ROTATE_0) {
        EPD_DrawAbsoluteVLine(x, y, line_height, c);
    } else if (rotate == ROTATE_90) {
        EPD_DrawAbsoluteHLine(width-line_height-y, x, line_height, c);
    } else if (rotate == ROTATE_180) {
        EPD_DrawAbsoluteVLine(width-x, height-line_height-y, line_height, c);
    } else if (rotate == ROTATE_270) {
        EPD_DrawAbsoluteHLine(y, height-x, line_height, c);
    }
}

#endif

void EPD_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, COLOR c) {
    EPD_DrawHLine(x, y, w+1, c);
    EPD_DrawHLine(x, y+h, w+1, c);
    EPD_DrawVLine(x, y, h, c);
    EPD_DrawVLine(x+w+1, y, h, c);
}


void EPD_DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, COLOR c) {
    for (uint16_t i = x; i <= x+w; ++i) {
      EPD_DrawVLine(i, y, h+1, c);
    }
}

// Draw round corner
static void EPD_DrawCircleHelper(uint16_t x0, uint16_t y0, uint16_t r, uint8_t cornername, COLOR c) {
	int32_t f     = 1 - r;
	int32_t ddF_x = 1;
	int32_t ddF_y = -2 * r;
	int32_t x     = 0;

	while (x < r) {
		if (f >= 0) {
			r--;
			ddF_y += 2;
			f     += ddF_y;
		}
		x++;
		ddF_x += 2;
		f     += ddF_x;
		if (cornername & 0x4) {
			EPD_DrawPixel(x0 + x, y0 + r, c);
			EPD_DrawPixel(x0 + r, y0 + x, c);
		}
		if (cornername & 0x2) {
			EPD_DrawPixel(x0 + x, y0 - r, c);
			EPD_DrawPixel(x0 + r, y0 - x, c);
		}
		if (cornername & 0x8) {
			EPD_DrawPixel(x0 - r, y0 + x, c);
			EPD_DrawPixel(x0 - x, y0 + r, c);
		}
		if (cornername & 0x1) {
			EPD_DrawPixel(x0 - r, y0 - x, c);
			EPD_DrawPixel(x0 - x, y0 - r, c);
		}
	}
}

// Draw filled corner
static void EPD_DrawFilledCircleHelper(uint16_t x0, uint16_t y0, uint16_t r, uint8_t cornername, uint16_t delta, COLOR c) {
	int32_t f     = 1 - r;
	int32_t ddF_x = 1;
	int32_t ddF_y = -r - r;
	int32_t x     = 0;

	delta++;
	while (x < r) {
		if (f >= 0) {
			r--;
			ddF_y += 2;
			f     += ddF_y;
		}
		x++;
		ddF_x += 2;
		f     += ddF_x;

		if (cornername & 0x1) {
			EPD_DrawVLine(x0 + x, y0 - r, r + r + delta, c);
			EPD_DrawVLine(x0 + r, y0 - x, x + x + delta, c);
		}
		if (cornername & 0x2) {
			EPD_DrawVLine(x0 - x, y0 - r, r + r + delta, c);
			EPD_DrawVLine(x0 - r, y0 - x, x + x + delta, c);
		}
	}
}

void EPD_DrawRoundRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t r, COLOR c) {
	// Draw edges
	EPD_DrawHLine(x + r  , y    , w - r - r, c);				// Top
	EPD_DrawHLine(x + r  , y + h - 1, w - r - r, c);			// Bottom
	EPD_DrawVLine(x    , y + r  , h - r - r, c);				// Left
	EPD_DrawVLine(x + w - 1, y + r  , h - r - r, c);			// Right
	// Draw four corners
	EPD_DrawCircleHelper(x + r,					y + r, 	r, 1, c);
	EPD_DrawCircleHelper(x + w - r - 1,			y + r, 	r, 2, c);
	EPD_DrawCircleHelper(x + w - r - 1, y + h - r - 1,	r, 4, c);
	EPD_DrawCircleHelper(x + r, 		y + h - r - 1,	r, 8, c);

}

void EPD_DrawFilledRoundRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t r, COLOR c) {
	EPD_DrawFilledRectangle(x + r, y, w - r, h, c);
	// Draw right vertical bar with round corners
	EPD_DrawFilledCircleHelper(x + w - 1,	y + r, r, 1, h - r - r - 1, c);
	// Draw left vertical bar with round corners
	EPD_DrawFilledCircleHelper(x + r,		y + r, r, 2, h - r - r - 1, c);
}

void EPD_DrawCircle(uint16_t x, uint16_t y, uint8_t radius, COLOR c) {
    /* Bresenham algorithm */
    int x_pos = -radius;
    int y_pos = 0;
    int err = 2 - 2 * radius;
    int e2;

    do {
        EPD_DrawPixel(x - x_pos, y + y_pos, c);
        EPD_DrawPixel(x + x_pos, y + y_pos, c);
        EPD_DrawPixel(x + x_pos, y - y_pos, c);
        EPD_DrawPixel(x - x_pos, y - y_pos, c);
        e2 = err;
        if (e2 <= y_pos) {
            err += ++y_pos * 2 + 1;
            if(-x_pos == y_pos && e2 <= x_pos) {
              e2 = 0;
            }
        }
        if (e2 > x_pos) {
            err += ++x_pos * 2 + 1;
        }
    } while (x_pos <= 0);
}

void EPD_DrawFilledCircle(uint16_t x, uint16_t y, uint8_t radius, COLOR c) {
    /* Bresenham algorithm */
    int x_pos = -radius;
    int y_pos = 0;
    int err = 2 - 2 * radius;
    int e2;

    do {
    	EPD_DrawPixel(x - x_pos, y + y_pos, c);
    	EPD_DrawPixel(x + x_pos, y + y_pos, c);
    	EPD_DrawPixel(x + x_pos, y - y_pos, c);
    	EPD_DrawPixel(x - x_pos, y - y_pos, c);
    	EPD_DrawHLine(x + x_pos, y + y_pos, 2 * (-x_pos) + 1, c);
    	EPD_DrawHLine(x + x_pos, y - y_pos, 2 * (-x_pos) + 1, c);
        e2 = err;
        if (e2 <= y_pos) {
            err += ++y_pos * 2 + 1;
            if(-x_pos == y_pos && e2 <= x_pos) {
                e2 = 0;
            }
        }
        if(e2 > x_pos) {
            err += ++x_pos * 2 + 1;
        }
    } while (x_pos <= 0);
}

void EPD_DrawBitmap(uint16_t x, uint16_t y, const uint8_t *bm, uint16_t bm_width, uint16_t bm_height, COLOR fg, COLOR bg) {
	if (bm == 0) return;
	uint16_t bytes_per_row = (bm_width + 7) >> 3;
    for (uint16_t row = 0; row < bm_height; ++row) {
    	for (uint16_t bit = 0; bit < bm_width; ++bit) {
			uint16_t in_byte = row * bytes_per_row + (bit >> 3);
			uint8_t  in_mask = 0x80 >> (bit & 0x7);
			COLOR color = (in_mask & bm[in_byte])?fg:bg;
			EPD_DrawPixel(x+bit, y+row, color);
    	}
    }
}

void EPD_SetRotation(Rotation_t r) {
	rotate = r;
}

uint8_t	*EPD_GetLayerData(COLOR c) {
	uint8_t l = EPD_Layer(c);
	return image[l];
}

uint8_t EPD_Layer(COLOR c) {
	uint8_t l = 0;
	switch (c) {
		case MODE_RED:
			l = 1;
			break;
		case MODE_BLACK:
		default:
			break;
	}
	return l;
}

uint8_t	EPD_GetLayerNumber(void) {
	return layers;
}

uint8_t	EPD_GetActiveLayer(void) {
	return active_layer;
}

bool EPD_IsFullMemory(void) {								// The memory buffers allocated for each display layer separately
	return active_layer >= EPD_LAYERS;
}

bool EPD_FirstPage(void) {
	if (active_layer >= EPD_LAYERS)							// Not layered display, do no draw it by pages
		return false;
	active_layer = 0;
	return true;
}

bool EPD_TurnPage(void) {
	if (active_layer >= EPD_LAYERS)							// No layered display
		return true;
	if (++active_layer >= layers)
		active_layer = 0;
	for (uint16_t x = 0; x < height*width/8; ++x) {			// Clear the memory buffer
		image[0][x] = 0;
	}
	return (active_layer == 0);								// Last page
}

bool EPD_AbsRectangle(Rectangle *r) {
    if (rotate == ROTATE_0) {
        if (r->x_upl >= width || r->y_upl >= height) return false;
        if (r->x_lor >= width || r->y_lor >= height) return false;
    } else if (rotate == ROTATE_90) {
        if (r->x_upl >= height || r->y_upl >= width) return false;
        if (r->x_lor >= height || r->y_lor >= width) return false;
        uint16_t y = r->x_upl;
        r->x_upl = width - r->y_upl;
        r->y_upl = y;
        y = r->x_lor;
        r->x_lor = width - r->y_lor;
        r->y_lor = y;
    } else if (rotate == ROTATE_180) {
        if (r->x_upl >= width || r->y_upl >= height) return false;
        if (r->x_lor >= width || r->y_lor >= height) return false;
        r->x_upl = width  - r->x_upl;
        r->y_upl = height - r->y_upl;
        r->x_lor = width  - r->x_lor;
        r->y_lor = height - r->y_lor;
    } else if (rotate == ROTATE_270) {
        if (r->x_upl >= height || r->y_upl >= width) return false;
        if (r->x_lor >= height || r->y_lor >= width) return false;
        uint16_t x = r->y_upl;
        r->y_upl = height - r->x_upl;
        r->x_upl = x;
        x = r->y_lor;
        r->y_lor = height - r->x_lor;
        r->x_lor = x;
    }
    if (r->x_upl > r->x_lor) {
    	uint16_t t	= r->x_upl;
    	r->x_upl	= r->x_lor;
    	r->x_lor	= t;
    }
    if (r->y_upl > r->y_lor) {
    	uint16_t t	= r->y_upl;
    	r->y_upl	= r->y_lor;
    	r->y_lor	= t;
    }
    return true;
}

void EPD_Reset(void) {
	HAL_GPIO_WritePin(EPD_RST_GPIO_Port, EPD_RST_Pin, GPIO_PIN_RESET);
	HAL_Delay(200);
	HAL_GPIO_WritePin(EPD_RST_GPIO_Port, EPD_RST_Pin, GPIO_PIN_SET);
	HAL_Delay(200);
}

void EPD_WaitDisplayReady(GPIO_PinState busy_status) {
	while (HAL_GPIO_ReadPin(EPD_BUSY_GPIO_Port, EPD_BUSY_Pin) == busy_status) {
		HAL_Delay(10);
	}
}

void EPD_SendCommand(EPD_CMD command) {
	while (HAL_SPI_STATE_READY != HAL_SPI_GetState(&EPD_SPI_PORT)) { }
	HAL_GPIO_WritePin(EPD_DC_GPIO_Port, EPD_DC_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(EPD_CS_GPIO_Port, EPD_CS_Pin, GPIO_PIN_RESET);
  	HAL_SPI_Transmit(&EPD_SPI_PORT, (uint8_t*)&command, 1, 1000);
  	HAL_GPIO_WritePin(EPD_CS_GPIO_Port, EPD_CS_Pin, GPIO_PIN_SET);
}

void EPD_SendData(uint8_t data) {
	while (HAL_SPI_STATE_READY != HAL_SPI_GetState(&EPD_SPI_PORT)) { }
	HAL_GPIO_WritePin(EPD_DC_GPIO_Port, EPD_DC_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(EPD_CS_GPIO_Port, EPD_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&EPD_SPI_PORT, &data, 1, 1000);
	HAL_GPIO_WritePin(EPD_CS_GPIO_Port, EPD_CS_Pin, GPIO_PIN_SET);
}

void EPD_SendDataBuff(const uint8_t *data, uint32_t len) {
	while (HAL_SPI_STATE_READY != HAL_SPI_GetState(&EPD_SPI_PORT)) { }
	HAL_GPIO_WritePin(EPD_DC_GPIO_Port, EPD_DC_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(EPD_CS_GPIO_Port, EPD_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&EPD_SPI_PORT, (uint8_t *)data, len, 1000);
	HAL_GPIO_WritePin(EPD_CS_GPIO_Port, EPD_CS_Pin, GPIO_PIN_SET);
}

void EPD_SetPartialRectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
	EPD_SendCommand(PARTIAL_PTIN);							// This command makes the display enter partial mode
	EPD_SendCommand(PAARTIAL_PTL);							// resolution setting
	EPD_SendData(x0 >> 8);
	EPD_SendData(x0 & 0xff);
	EPD_SendData(x1 >> 8);
	EPD_SendData(x1 & 0xff);
	EPD_SendData(y0 >> 8);
	EPD_SendData(y0 & 0xff);
	EPD_SendData(y1 >> 8);
	EPD_SendData(y1 & 0xff);
	EPD_SendData(0x28);
}

void EPD_SetLut(const LUT *pLut) {
	const uint8_t *lut_ptr = pLut->lut_data;

	EPD_SendCommand(LUT_FOR_VCOM);
	EPD_SendDataBuff(lut_ptr, pLut->vcom_size);
	lut_ptr += pLut->vcom_size;

	EPD_SendCommand(LUT_WHITE_TO_WHITE);
	EPD_SendDataBuff(lut_ptr, pLut->ww_size);
	lut_ptr += pLut->ww_size;

	EPD_SendCommand(LUT_BLACK_TO_WHITE);
	EPD_SendDataBuff(lut_ptr, pLut->bw_size);
	lut_ptr += pLut->bw_size;

	EPD_SendCommand(LUT_WHITE_TO_BLACK);
	EPD_SendDataBuff(lut_ptr, pLut->wb_size);
	lut_ptr += pLut->wb_size;

	EPD_SendCommand(LUT_BLACK_TO_BLACK);
	EPD_SendDataBuff(lut_ptr, pLut->bb_size);
}

void EPD_Sleep(void) {
	EPD_SendCommand(DEEP_SLEEP);
	EPD_SendData(0xa5);
}

static void EPD_DrawAbsolutePixel(uint16_t x, uint16_t y, COLOR c) {
	uint8_t active = EPD_Layer(c);							// The memory buffer index to draw the pixel to
	uint8_t memory_layers = layers;
	if (active_layer < EPD_LAYERS) {						// Use single memory buffer for all color layers, draw display by pages
		uint8_t layer_color = 1 << active_layer;			// 1 - MODE_BLACK, 2 - MODE_RED, see common.h
		if ((c & layer_color) == 0 && c != MODE_CLEAR)
			return;											// Not active color, skip pixel
		active 			= 0;								// Draw to single memory buffer
		memory_layers	= 1;
	}
    if (!image[active] || x >= width || y >= height) return;

    if (c == MODE_CLEAR) {									// Clear all memory layers
    	for (uint8_t l = 0; l < memory_layers; ++l)
    		image[l][(x + y * width) / 8] &= ~(0x80 >> (x % 8));
    } else {
        for (uint8_t l = 0; l < memory_layers; ++l) {
        	if (l == active) {
        		image[l][(x + y * width) / 8] |= 0x80 >> (x % 8); // set pixel
        	} else {
        		image[l][(x + y * width) / 8] &= ~(0x80 >> (x % 8)); // clear pixel
        	}
        }
    }
}

#ifndef TINY_LIB
static void EPD_DrawAbsoluteHLine(uint16_t x, uint16_t y, uint16_t line_width, COLOR c) {
	if (x >= width || y >= height) return;
	if (line_width + x >= width) {							// End of line is out of screen
		line_width = width - x - 1;
	}
	if (line_width == 0) return;
	uint8_t active = EPD_Layer(c);							// Layer to draw the line
	uint8_t memory_layers = layers;
	if (active_layer < EPD_LAYERS) {						// Use single memory buffer for all color layers, draw display by pages
		uint8_t layer_color = 1 << active_layer;			// 1 - MODE_BLACK, 2 - MODE_RED, see common.h
		if ((c & layer_color) == 0 && c != MODE_CLEAR)
			return;											// Not active color, skip pixel
		active 			= 0;								// Draw to single memory buffer
		memory_layers	= 1;
	}
	uint8_t b_pos = x & 0x7;
	uint16_t  bytes_per_row = (width + 7) >> 3;
	if (b_pos) {											// Draw the beginning of line, align to the byte border
		bool single_byte = false;
		uint16_t in_byte = y * bytes_per_row + (x >> 3);
		uint8_t begin = 0xff >> b_pos;
		if (line_width <= 8 - b_pos) {						// Whole horizontal line fits the byte
			uint8_t tail = 0xff << (8 - b_pos - line_width);
			begin &= tail;
			single_byte = true;
		}
		if (c != MODE_CLEAR) {								// Add new pixels
			image[active][in_byte] |= begin;
		} else {											// clear pixels
			for (uint8_t l = 0; l < memory_layers; ++l) {
				if (image[l]) {
					image[l][in_byte] &= ~begin;
				}
			}
		}
		if (single_byte) return;
		x += 8 - b_pos;
		line_width -= 8 - b_pos;
	}

	uint16_t first_byte = y * bytes_per_row + (x >> 3);
	if (c != MODE_CLEAR) {
		for (uint8_t i = 0; i < (line_width >> 3); ++i) {	// Draw full-byte patterns
			image[active][first_byte + i] = 0xff;
		}
	} else {
		for (uint8_t l = 0; l < memory_layers; ++l) {
			if (image[l]) {
				for (uint8_t i = 0; i < (line_width >> 3); ++i) {
					image[l][first_byte + i] = 0;
				}
			}
		}
	}
	uint8_t tail_len = line_width & 0x7;
	if (tail_len) {											// Draw the tail of line
		uint16_t in_byte = first_byte + (line_width >> 3);
		uint8_t tail = 0xff << (8 - tail_len);
		if (c != MODE_CLEAR) {
			image[active][in_byte] |= tail;
		} else {
			for (uint8_t l = 0; l < memory_layers; ++l) {
				if (image[l]) {
					image[l][in_byte] &= ~tail;
				}
			}
		}
	}
}

static void EPD_DrawAbsoluteVLine(uint16_t x, uint16_t y, uint16_t line_height, COLOR c) {
	if (x >= width || y >= height) return;
	if (line_height + y >= height) {
		line_height = height - y - 1;
	}
	if (line_height == 0) return;
	uint8_t active = EPD_Layer(c);							// Layer to draw the line
	uint8_t memory_layers = layers;
	if (active_layer < EPD_LAYERS) {						// Use single memory buffer for all color layers, draw display by pages
		uint8_t layer_color = 1 << active_layer;			// 1 - MODE_BLACK, 2 - MODE_RED, see common.h
		if ((c & layer_color) == 0 && c != MODE_CLEAR)
			return;											// Not active color, skip pixel
		active 			= 0;								// Draw to single memory buffer
		memory_layers	= 1;
	}
	uint8_t bit_mask = 0x80 >> (x & 0x7);
	uint16_t  bytes_per_row = (width + 7) >> 3;
	uint16_t in_byte = y * bytes_per_row + (x >> 3);
	if (c != MODE_CLEAR) {
		for (uint16_t h = 0; h < line_height; ++h) {
			image[active][in_byte] |= bit_mask;
			in_byte += bytes_per_row;
		}
	} else {
		for (uint8_t l = 0; l < memory_layers; ++l) {
			if (image[l]) {
				for (uint16_t h = 0; h < line_height; ++h) {
					image[l][in_byte] &= ~bit_mask;
					in_byte += bytes_per_row;
				}
			}
		}
	}
}
#endif
