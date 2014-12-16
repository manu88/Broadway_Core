//
//  vg_mac.h
//  Broadway_test
//
//  Created by Manuel Deneu on 27/10/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//


/*
 
 Dummy OpenVG declarations that do nothing, in order to compile on macOS
 
 */

#ifndef Broadway_test_vg_mac_h
#define Broadway_test_vg_mac_h


typedef int VGint;
typedef int VGbitfield;
typedef float VGfloat;
typedef VGint VGHandle;
typedef VGHandle VGImage;
typedef VGHandle VGPaint;
typedef VGHandle VGPath;

#define VG_INVALID_HANDLE ((VGHandle)0)

typedef enum {
    /* Color paint parameters */
    VG_PAINT_TYPE                               = 0x1A00,
    VG_PAINT_COLOR                              = 0x1A01,
    VG_PAINT_COLOR_RAMP_SPREAD_MODE             = 0x1A02,
    VG_PAINT_COLOR_RAMP_PREMULTIPLIED           = 0x1A07,
    VG_PAINT_COLOR_RAMP_STOPS                   = 0x1A03,
    
    /* Linear gradient paint parameters */
    VG_PAINT_LINEAR_GRADIENT                    = 0x1A04,
    
    /* Radial gradient paint parameters */
    VG_PAINT_RADIAL_GRADIENT                    = 0x1A05,
    
    /* Pattern paint parameters */
    VG_PAINT_PATTERN_TILING_MODE                = 0x1A06
} VGPaintParamType;

typedef enum {
    VG_PAINT_TYPE_COLOR                         = 0x1B00,
    VG_PAINT_TYPE_LINEAR_GRADIENT               = 0x1B01,
    VG_PAINT_TYPE_RADIAL_GRADIENT               = 0x1B02,
    VG_PAINT_TYPE_PATTERN                       = 0x1B03
} VGPaintType;

typedef enum {
    VG_COLOR_RAMP_SPREAD_PAD                    = 0x1C00,
    VG_COLOR_RAMP_SPREAD_REPEAT                 = 0x1C01,
    VG_COLOR_RAMP_SPREAD_REFLECT                = 0x1C02
} VGColorRampSpreadMode;

typedef enum {
    VG_TILE_FILL                                = 0x1D00,
    VG_TILE_PAD                                 = 0x1D01,
    VG_TILE_REPEAT                              = 0x1D02,
    VG_TILE_REFLECT                             = 0x1D03
} VGTilingMode;


typedef enum {
    VG_NO_ERROR                                 = 0,
    VG_BAD_HANDLE_ERROR                         = 0x1000,
    VG_ILLEGAL_ARGUMENT_ERROR                   = 0x1001,
    VG_OUT_OF_MEMORY_ERROR                      = 0x1002,
    VG_PATH_CAPABILITY_ERROR                    = 0x1003,
    VG_UNSUPPORTED_IMAGE_FORMAT_ERROR           = 0x1004,
    VG_UNSUPPORTED_PATH_FORMAT_ERROR            = 0x1005,
    VG_IMAGE_IN_USE_ERROR                       = 0x1006,
    VG_NO_CONTEXT_ERROR                         = 0x1007
} VGErrorCode;

typedef enum {
    /* Mode settings */
    VG_MATRIX_MODE                              = 0x1100,
    VG_FILL_RULE                                = 0x1101,
    VG_IMAGE_QUALITY                            = 0x1102,
    VG_RENDERING_QUALITY                        = 0x1103,
    VG_BLEND_MODE                               = 0x1104,
    VG_IMAGE_MODE                               = 0x1105,
    
    /* Scissoring rectangles */
    VG_SCISSOR_RECTS                            = 0x1106,
    
    /* Stroke parameters */
    VG_STROKE_LINE_WIDTH                        = 0x1110,
    VG_STROKE_CAP_STYLE                         = 0x1111,
    VG_STROKE_JOIN_STYLE                        = 0x1112,
    VG_STROKE_MITER_LIMIT                       = 0x1113,
    VG_STROKE_DASH_PATTERN                      = 0x1114,
    VG_STROKE_DASH_PHASE                        = 0x1115,
    VG_STROKE_DASH_PHASE_RESET                  = 0x1116,
    
    /* Edge fill color for VG_TILE_FILL tiling mode */
    VG_TILE_FILL_COLOR                          = 0x1120,
    
    /* Color for vgClear */
    VG_CLEAR_COLOR                              = 0x1121,
    
    /* Enable/disable alpha masking and scissoring */
    VG_MASKING                                  = 0x1130,
    VG_SCISSORING                               = 0x1131,
    
    /* Pixel layout information */
    VG_PIXEL_LAYOUT                             = 0x1140,
    VG_SCREEN_LAYOUT                            = 0x1141,
    
    /* Source format selection for image filters */
    VG_FILTER_FORMAT_LINEAR                     = 0x1150,
    VG_FILTER_FORMAT_PREMULTIPLIED              = 0x1151,
    
    /* Destination write enable mask for image filters */
    VG_FILTER_CHANNEL_MASK                      = 0x1152,
    
    /* Implementation limits (read-only) */
    VG_MAX_SCISSOR_RECTS                        = 0x1160,
    VG_MAX_DASH_COUNT                           = 0x1161,
    VG_MAX_KERNEL_SIZE                          = 0x1162,
    VG_MAX_SEPARABLE_KERNEL_SIZE                = 0x1163,
    VG_MAX_COLOR_RAMP_STOPS                     = 0x1164,
    VG_MAX_IMAGE_WIDTH                          = 0x1165,
    VG_MAX_IMAGE_HEIGHT                         = 0x1166,
    VG_MAX_IMAGE_PIXELS                         = 0x1167,
    VG_MAX_IMAGE_BYTES                          = 0x1168,
    VG_MAX_FLOAT                                = 0x1169,
    VG_MAX_GAUSSIAN_STD_DEVIATION               = 0x116A
} VGParamType;

typedef enum {
    VG_CAP_BUTT                                 = 0x1700,
    VG_CAP_ROUND                                = 0x1701,
    VG_CAP_SQUARE                               = 0x1702
} VGCapStyle;

typedef enum {
    VG_JOIN_MITER                               = 0x1800,
    VG_JOIN_ROUND                               = 0x1801,
    VG_JOIN_BEVEL                               = 0x1802
} VGJoinStyle;

typedef enum {
    VG_EVEN_ODD                                 = 0x1900,
    VG_NON_ZERO                                 = 0x1901
} VGFillRule;

typedef enum {
    VG_STROKE_PATH                              = (1 << 0),
    VG_FILL_PATH                                = (1 << 1)
} VGPaintMode;


typedef enum {
    VG_MATRIX_PATH_USER_TO_SURFACE              = 0x1400,
    VG_MATRIX_IMAGE_USER_TO_SURFACE             = 0x1401,
    VG_MATRIX_FILL_PAINT_TO_USER                = 0x1402,
    VG_MATRIX_STROKE_PAINT_TO_USER              = 0x1403
} VGMatrixMode;

static void vgSeti (VGParamType type, VGint value)
{
    
}

static void vgSetPixels(VGint dx, VGint dy, VGImage src, VGint sx, VGint sy, VGint width, VGint height)
{
    
}

/* Matrix Manipulation */
static void vgLoadIdentity(void){}
static void vgLoadMatrix(const VGfloat * m){}
static void vgGetMatrix(VGfloat * m){}
static void vgMultMatrix(const VGfloat * m){}
static void vgTranslate(VGfloat tx, VGfloat ty){}
static void vgScale(VGfloat sx, VGfloat sy){}
static void vgShear(VGfloat shx, VGfloat shy){}
static void vgRotate(VGfloat angle){}

static VGPaint vgCreatePaint(void) { return 1; }
static void vgDestroyPaint(VGPaint paint){}
static void vgSetPaint(VGPaint paint, VGbitfield paintModes){}

static void vgSetParameteri(VGHandle object, VGint paramType, VGint value) {}
static void vgSetParameterfv(VGHandle object, VGint paramType, VGint count, const VGfloat * values) {}



#endif
