//
//  GXTransformations.h
//  Broadway_test
//
//  Created by Manuel Deneu on 12/10/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef Broadway_test_GXTransformations_h
#define Broadway_test_GXTransformations_h

typedef enum
{
    GXTransformation_Unknown        = -1,
    GXTransformation_Identity       = 0,
    GXTransformation_Translation    = 1,
    GXTransformation_Rotation       = 2,
    GXTransformation_Scale          = 3,
    GXTransformation_Multiplication = 4
    
} GXTransformationType;


typedef struct
{
    GXTransformationType type;
    
    /*
     sx   shy  w0
     shx  sy   w1
     tx   ty   w2
     */
    
    float   sx;
    float   shy;
    float   w0;
    float   shx;
    float   sy;
    float   w1;
    float   tx;
    float   ty;
    float   w2;

    
    
} GXTransformation;

GXTransformation makeTranslation   ( float dx , float dy);
GXTransformation makeScale         ( float dx , float dy);

GXTransformation makeRotation            ( float theta /* rads! */ );
//inline GXTransformation makeRotationAroundPoint ( float theta /* rads! */ , float x, float y  );

GXTransformation makeIdentity      ();
GXTransformation makeTransformation( float *  mat /* 9 floats ! */ , GXTransformationType type);

GXTransformationType getType( const float *mat /* 9 floats ! */);


#endif
