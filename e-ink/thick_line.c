/*
 * thick_line.cpp
 *
 */

#include <math.h>
#include "common.h"

// Local function forward declarations
static void EPD_xVarLine(uint16_t x0, uint16_t y0, int dx, int dy, int xstep, int ystep,
		int pxstep, int pystep, LineThickness thickness, COLOR color, uint8_t swapped);
static void EPD_yVarLine(uint16_t x0, uint16_t y0, int dx, int dy, int xstep, int ystep,
		int pxstep, int pystep, LineThickness thickness, COLOR color, uint8_t swapped);
static void EPD_xPerpendicular(int x0, int y0, int dx, int dy, int xstep, int ystep, int einit, int hthick, int winit, COLOR color);
static void EPD_yPerpendicular(int x0, int y0, int dx, int dy, int xstep, int ystep, int einit, int hthick, int winit, COLOR color);
static uint8_t line_thickness = 0;

void EPD_DrawThickLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t thickness, COLOR color) {
	line_thickness = thickness;
	EPD_DrawVarThickLine(x0, y0, x1, y1, 0, color);
}

void EPD_DrawVarThickLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, LineThickness thickness, COLOR color) {
	uint8_t swapped = 0;						// Flag indicating the line points were swapped
	if (x0 > x1) {								// switch line begin and end points
		uint16_t t 	= x1;
		x1			= x0;
		x0			= t;
		t			= y1;
		y1			= y0;
		y0			= t;
		swapped		= 1;
	}
    int dx 		= x1 - x0;
    int dy 		= y1 - y0;
	int xstep	= 1;
	int	ystep	= 1;

	if (dy < 0) { dy= -dy; ystep= -1; }

	if (dx == 0) xstep = 0;
	if (dy == 0) ystep = 0;

	int pxstep 	= 0;
	int pystep	= 0;
	switch(xstep + ystep*4) {
	    case  0 + -1*4 :  pystep =  0; pxstep = -1;	break;
	    case  0 +  0*4 :  pystep =  0; pxstep =  0;	break;
	    case  0 +  1*4 :  pystep =  0; pxstep =  1; break;
	    case  1 + -1*4 :  pystep = -1; pxstep = -1;	break;
	    case  1 +  0*4 :  pystep = -1; pxstep =  0;	break;
	    case  1 +  1*4 :  pystep =  1; pxstep = -1;	break;
	    default:
	    	break;
	}

	if (dx > dy)
		EPD_xVarLine(x0, y0, dx, dy, xstep, ystep, pxstep, pystep, thickness, color, swapped);
	else
		EPD_yVarLine(x0, y0, dx, dy, xstep, ystep, pxstep, pystep, thickness, color, swapped);
}

static void EPD_xVarLine(uint16_t x0, uint16_t y0, int dx, int dy, int xstep, int ystep,
		int pxstep, int pystep, LineThickness thickness, COLOR color, uint8_t swapped) {
    int p_error 	= 0;
    int error		= 0;
    int y			= y0;
    int x			= x0;
    int threshold 	= dx - 2*dy;
    int E_diag 		= -2*dx;
    int	E_square	= 2*dy;
    int	length 		= dx+1;
    double D		= sqrt(dx*dx + dy*dy);

    uint16_t hthick  = line_thickness * D;			// half of line thickness
    for(uint16_t p = 0; p < length; ++p) {
    	if (thickness) {
    		uint16_t pos = p;
    		if (swapped) pos = length-p-1;
    		hthick = (*thickness)(pos, length) * D;	// half of thickness
    	}
    	EPD_xPerpendicular(x, y, dx, dy, pxstep, pystep, p_error, hthick, error, color);
    	if (error >= threshold) {
    		y 		+= ystep;
    		error 	+= E_diag;
    		if (p_error >= threshold) {
    			EPD_xPerpendicular(x, y, dx, dy, pxstep, pystep, (p_error+E_diag+E_square), hthick, error, color);
    			p_error	+= E_diag;
    		}
    		p_error	+= E_square;
    	}
    	error	+= E_square;
    	x		+= xstep;
    }
}

static void EPD_yVarLine(uint16_t x0, uint16_t y0, int dx, int dy, int xstep, int ystep,
		int pxstep, int pystep, LineThickness thickness, COLOR color, uint8_t swapped) {

    int p_error		= 0;
    int	error		= 0;
    int y			= y0;
    int x			= x0;
    int threshold	= dy - 2*dx;
    int	E_diag		= -2*dy;
    int	E_square	= 2*dx;
    int	length		= dy+1;
    double D		= sqrt(dx*dx + dy*dy);

    uint16_t hthick  = line_thickness * D;

    for(int p=0;p<length;p++) {
    	if (thickness) {
    		uint16_t pos = p;
    		if (swapped) pos = length-p-1;
    		hthick = (*thickness)(pos, length) * D;	// half of thickness
    	}
    	EPD_yPerpendicular(x, y, dx, dy, pxstep, pystep, (p_error+E_diag+E_square), hthick, error, color);
    	if (error >= threshold) {
    		x		+= xstep;
    		error 	+= E_diag;
    		if (p_error >= threshold) {
    			EPD_yPerpendicular(x, y, dx, dy, pxstep, pystep, (p_error+E_diag+E_square), hthick, error, color);
    			p_error	+= E_diag;
    		}
    		p_error	+= E_square;
    	}
    	error	+= E_square;
    	y		+= ystep;
    }
}

static void EPD_xPerpendicular(int x0, int y0, int dx, int dy, int xstep, int ystep, int einit, int hthick, int winit, COLOR color) {
	int x			= x0;
	int y			= y0;
	int threshold	= dx - 2*dy;
	int E_diag		= -2*dx;
	int E_square	= 2*dy;
	int p 			= 0;
	int q			= 0;
	int error		= einit;
	int tk			= dx+dy-winit;

	while (tk <= hthick) {
		EPD_DrawPixel((uint16_t)x, (uint16_t)y, color);
		if (error >= threshold) {
			x		+= xstep;
			error	+= E_diag;
			tk		+= 2*dy;
		}
		error	+= E_square;
		y		+= ystep;
		tk		+= 2*dx;
		q++;
	}

	y		= y0;
	x		= x0;
	error	= -einit;
	tk		= dx+dy+winit;

	while(tk <= hthick) {
		if (p) EPD_DrawPixel((uint16_t)x, (uint16_t)y, color);
		if (error > threshold) {
			x		-= xstep;
			error 	+= E_diag;
			tk		+= 2*dy;
		}
		error 	+= E_square;
		y		-= ystep;
		tk		+= 2*dx;
		p++;
	}

	if (q==0 && p<2)							// we need this for very thin lines
		EPD_DrawPixel((uint16_t)x0, (uint16_t)y0, color);
}


static void EPD_yPerpendicular(int x0, int y0, int dx, int dy, int xstep, int ystep, int einit, int hthick, int winit, COLOR color) {
    int p			= 0;
    int q			= 0;
    int threshold	= dy - 2*dx;
    int	E_diag		= -2*dy;
    int E_square	= 2*dx;
    int y			= y0;
    int	x			= x0;
    int error		= -einit;
    int	tk			= dx+dy+winit;

    while(tk <= hthick) {
    	EPD_DrawPixel((uint16_t)x, (uint16_t)y, color);
    	if (error > threshold) {
    		y		+= ystep;
    		error	+= E_diag;
    		tk		+= 2*dx;
    	}
    	error	+= E_square;
    	x		+= xstep;
    	tk		+= 2*dy;
    	q++;
    }

    y		= y0;
    x		= x0;
    error	= einit;
    tk		= dx+dy-winit;

    while(tk <= hthick) {
    	if (p != 0)
    		EPD_DrawPixel((uint16_t)x, (uint16_t)y, color);
    	if (error >= threshold) {
    		y		-= ystep;
    		error	+= E_diag;
    		tk		+= 2*dx;
    	}
    	error	+= E_square;
    	x		-= xstep;
    	tk		+= 2*dy;
    	p++;
    }

    if (q == 0 && p < 2)						 // we need this for very thin lines
    	EPD_DrawPixel((uint16_t)x0, (uint16_t)y0, color);
}

