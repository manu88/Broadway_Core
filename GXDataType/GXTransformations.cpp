

//
//  GXTransformations.c
//  Broadway_test
//
//  Created by Manuel Deneu on 12/10/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#include <math.h>
#include "GXTransformations.h"


GXTransformation makeTransformation( float *  mat , GXTransformationType type)
{
    return GXTransformation { type , mat[0] ,  mat[1] , mat[2] ,
                                     mat[3] ,  mat[4] , mat[5] ,
                                     mat[6] ,  mat[7] , mat[8]
                            };
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

GXTransformation makeTranslation( float dx , float dy)
{
    /*
     1 0 dx
     0 1 dy 
     0 0 1
     
     */
    return GXTransformation { GXTransformation_Translation ,
                              1.0f ,  0    , dx  ,
                              0    ,  1.0f , dy  ,
                              0    ,  0    , 1.0f
                            };
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

GXTransformation makeScale      ( float dx , float dy)
{
    /*
     dx 0  0
     0  dy 0
     0  0  1
     */
    return GXTransformation { GXTransformation_Scale ,
                                dx ,  0  , 0   ,
                                0  ,  dy , 0   ,
                                0  ,  0  , 1.0f
    };
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

GXTransformation makeRotation( float theta /* rads! */)
{
    /*
    cos t  -sin t  0
    sin t  cost t  0
    0      0       1
     */
    
    const float sin = sinf( theta );
    const float cos = cosf( theta );
    return GXTransformation { GXTransformation_Scale ,
                                cos ,  -sin , 0   ,
                                sin ,  cos  , 0   ,
                                0   ,  0    , 1.0f
                            };
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

GXTransformation makeIdentity()
{
    return GXTransformation { GXTransformation_Scale ,
                                1.0f ,  0    , 0   ,
                                0    ,  1.0f , 0   ,
                                0    ,  0    , 1.0f
                            };
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

GXTransformationType getType( const float *mat /* 9 floats ! */)
{
    if (     (mat[0] == 1) && (mat[1] == 0) && (mat[2] == 0)
         &&  (mat[3] == 0) && (mat[4] == 1) && (mat[5] == 1)
         &&  (mat[3] == 0) && (mat[4] == 0) && (mat[5] == 0)
        )
        return GXTransformation_Identity;
    
    
    else if (     (mat[0] == 1) && (mat[1] == 1) && (mat[2] == 1)
              &&  (mat[3] == 1) && (mat[4] == 1) && (mat[5] == 1)
              &&  (mat[3] == 1) && (mat[4] == 1) && (mat[5] == 1)
        )
        return GXTransformation_Identity;
    
    return GXTransformation_Unknown;
}




