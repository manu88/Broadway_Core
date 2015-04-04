//
//  GXImage.cpp
//  Broadway_test
//
//  Created by Manuel Deneu on 23/09/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//
#include "../Internal/Utils.h"




extern "C"
{
    #include <stdio.h>
    #include <cstddef>

#ifdef HAVE_JPEG_LIB
    #include <jpeglib.h>
#endif
    
#ifdef HAVE_PNG_LIB

    
    #include <png.h>
#endif
    

}



#include <assert.h>

#include "GXImage.h"
#include "../Log/Log.h"
#include "../Data/StringOperations.h"
#include "../Internal/FileSystem.h"

GXImage::GXImage( const std::string &filename ):
m_filename     ( filename ),

/*
m_scaleX       ( 1.f ),
m_scaleY       ( 1.f ),
 */

m_imageSize    ( makeSizeNULL() ),
m_autoScale    ( true ),
m_image        ( 0 )
{
    className = "GXImage";
}


void GXImage::setImage( const std::string &filename )
{
    if ( m_filename != filename)
    {
        m_filename = filename;
        setUnprepared();
        changed();
    }
}

void GXImage::fitImageToBounds( bool autoScale )
{
    
}

void GXImage::paint( const GXRect &rect , GXAnimation* anim )
{

    if ( m_image == VG_INVALID_HANDLE )
        return ;
    
    if ( m_autoScale )
    {
        vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
        vgLoadIdentity();
        vgScale(0.5f, 0.3f);
        
    }

    vgSetPixels( rect.origin.x, // dest
                 rect.origin.y, // dest
                 m_image,
                 rect.origin.x,              // src
                 rect.origin.y,              // src
                 rect.size.width,
                 rect.size.height );

    if ( m_autoScale )
    {
        vgLoadIdentity();        
    }


}

GXImage::~GXImage()
{

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXImage::prepareRessources()
{

    
    const std::string extension = StringOperations::toLowerCase( FileSystem::getFileExtension( m_filename) );
    
    if (   extension == "jpg"
        || extension == "jpeg"
        )
    {
        m_image = createImageFromJpeg( m_filename.c_str() , m_imageSize.width , m_imageSize.height );
    }
    
    else if ( extension == "png" )
    {
        m_image = createImageFromPng( m_filename.c_str() , m_imageSize.width , m_imageSize.height );
        
    }

    if (m_image == VG_INVALID_HANDLE )
    {

    }
    
#ifdef TARGET_RASPBERRY_PI
    const int width = vgGetParameteri( m_image, VG_IMAGE_WIDTH);
    const int height = vgGetParameteri( m_image, VG_IMAGE_HEIGHT);

    setBounds(0, 0, width, height);
#endif


    

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXImage::deleteRessources()
{
    if ( m_image == VG_INVALID_HANDLE )
        return ;
    
    
#ifdef TARGET_RASPBERRY_PI
	vgDestroyImage(m_image);
#endif
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXImage::changed()
{
    // notif if bounds changed.

    /*
    deleteRessources();
    prepareRessources();
     */
    
}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ VGImage GXImage::createImageFromJpeg(const char *filename , int &w , int &h)
{
    VGImage img = 0;
    
    unsigned int width = -1;
    unsigned int height = -1;

#ifdef HAVE_JPEG_LIB


    FILE       *infile = nullptr;
    struct     jpeg_decompress_struct jdc;
    struct     jpeg_error_mgr jerr;
    JSAMPARRAY buffer;

    unsigned int bstride;
    unsigned int bbpp;
    

    VGubyte *data;
    

    unsigned int dstride;
    unsigned int dbpp;
    
    VGubyte *brow;
    VGubyte *drow;
    unsigned int x;
    
    
    VGImageFormat rgbaFormat;
    
    
    if ( Utils::checkLittleEndian() )
        rgbaFormat = VG_sABGR_8888;
    else
        rgbaFormat = VG_sRGBA_8888;
    
    // Try to open image file
    infile = fopen(filename, "rb");
    
    if (infile == NULL)
    {
        Log::log("Failed opening '%s' for reading!\n", filename);
        return VG_INVALID_HANDLE;
    }
    
    // Setup default error handling
    jdc.err = jpeg_std_error( &jerr );
    jpeg_create_decompress( &jdc );
    
    // Set input file
    jpeg_stdio_src( &jdc , infile );
    
    // Read header and start
    jpeg_read_header( &jdc , TRUE );
    jpeg_start_decompress( &jdc );
    width = jdc.output_width;
    height = jdc.output_height;
    
    // Allocate buffer using jpeg allocator
    bbpp = jdc.output_components;
    bstride = width * bbpp;
    buffer = (*jdc.mem->alloc_sarray)
    ((j_common_ptr) & jdc, JPOOL_IMAGE, bstride, 1);
    
    // Allocate image data buffer
    dbpp = 4;
    dstride = width * dbpp;
    data = (VGubyte *) malloc(dstride * height);
    
    // Iterate until all scanlines processed
    while (jdc.output_scanline < height)
    {
        
        // Read scanline into buffer
        jpeg_read_scanlines(&jdc, buffer, 1);
        drow = data + (height - jdc.output_scanline) * dstride;
        brow = buffer[0];
        // Expand to RGBA
        for (x = 0; x < width; ++x, drow += dbpp, brow += bbpp)
        {
            switch (bbpp)
            {
                case 4:
                    drow[0] = brow[0];
                    drow[1] = brow[1];
                    drow[2] = brow[2];
                    drow[3] = brow[3];
                    break;
                case 3:
                    drow[0] = brow[0];
                    drow[1] = brow[1];
                    drow[2] = brow[2];
                    drow[3] = 255;
                    break;
            }
        }
    }
    
    // Create VG image
    img = vgCreateImage( rgbaFormat,
                         width,
                         height,
                         VG_IMAGE_QUALITY_BETTER
                        );

    vgImageSubData( img,
                    data,
                    dstride,
                    rgbaFormat,
                    0,
                    0,
                    width,
                    height
                   );
    

    
    // Cleanup
    jpeg_destroy_decompress( &jdc );
    
    fclose( infile );
    
    free( data );
#endif    
    w = width;
    h = height;
    

    
    return img;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ VGImage GXImage::createImageFromPng(const char *filename , int &w , int &h)
{
    VGImage img = 0;

    unsigned int width = -1;
    unsigned int height = -1;
    
    
    

#ifdef HAVE_PNG_LIB

    printf("\n create PNG  IMAGE");    
    VGImageFormat rgbaFormat;
    
    
    if ( Utils::checkLittleEndian() )
        rgbaFormat = VG_sABGR_8888;
    else
        rgbaFormat = VG_sRGBA_8888;
    
    png_byte color_type;
    png_byte bit_depth;
    png_bytep *row_pointers = nullptr;
    
    FILE       *infile = nullptr;
    
    infile = fopen(filename, "rb");
    
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    
    if( !png )
        return VG_INVALID_HANDLE;
    
    png_infop info = png_create_info_struct( png );
    
    if(!info)
        return VG_INVALID_HANDLE;
    
    if( setjmp( png_jmpbuf( png ) ) )
        return VG_INVALID_HANDLE;
    
    png_init_io( png, infile );
    
    png_read_info(png, info);
    
    width = png_get_image_width(png, info);
    
    height = png_get_image_height(png, info);
    
    color_type = png_get_color_type(png, info);
    
    bit_depth = png_get_bit_depth(png, info);
    
    // Read any color_type into 8bit depth, RGBA format.
    // See http://www.libpng.org/pub/png/libpng-manual.txt
    
    if(bit_depth == 16)
        png_set_strip_16(png);
    
    if(color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);
    
    // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
    if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png);
    
    if(png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);
    
    // These color_type don't have an alpha channel then fill it with 0xff.
    if(color_type == PNG_COLOR_TYPE_RGB ||
       color_type == PNG_COLOR_TYPE_GRAY ||
       color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
    
    if(color_type == PNG_COLOR_TYPE_GRAY ||
       color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);
    
    png_read_update_info(png, info);
    
    row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
    
    for(unsigned int y = 0; y < height; y++)
    {
        row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png,info));
    }
    
    png_read_image(png, row_pointers);
    
    fclose( infile );
    
    for(unsigned int y = 0; y < height; y++)
    {
        png_bytep row = row_pointers[y];
        for( unsigned int x = 0; x < width; x++)
        {
            png_bytep px = &(row[x * 4]);
            // Do something awesome for each pixel here...
          //  printf("%4d, %4d = RGBA(%3d, %3d, %3d, %3d)\n", x, y, px[0], px[1], px[2], px[3]);
        }
    }
    
    
    
#endif
    return img;
    
}



