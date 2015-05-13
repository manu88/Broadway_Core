//
//  GXPaintJS.h
//  Broadway_test
//
//  Created by Manuel Deneu on 08/10/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_test__GXPaintJS__
#define __Broadway_test__GXPaintJS__

/*!
 *  \brief     GXPaintJS
 *  \details   Display context to draw from JS code
 *  \author    Manuel Deneu
 *  \version   0
 *  \date      2014
 *  \pre       .
 *  \bug       
 *  \warning   .
 *  \copyright GNU Public License.
 */
#include <vector>
#include <pthread.h>

#include "GXElement.h"
#include "../Internal/Object.h"
#include "Painter.h"
#include "GXPath.h"

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
 
    This class provides asynchronious methods to draw path on screen. Since every paint operations
    have to be performed from the GUI thread,  its the only way to create shapes from an other thread.
    Simply use the usual methods ( addLine, addRect, ... , colors setters , fill/stroke , etc.) from
    any thread and a list of drawing operations will be created, and performed assync later when this 
    element is marked as 'in need of display'. 
 
    GXPaintJS's methods are basically the same than GXPath's, but you can call them from any thread :-)
    
 
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class GXAsyncPainter : public GXElement
{
public:
    GXAsyncPainter();
    ~GXAsyncPainter();
    
    void setFillColor( const GXColor &fillColor);
    void setStrokeColor( const GXColor &strokeColor);
    void setStrokeWidth( float width );
    
    void setCapStyle ( GXCapStyle style );
    void setJoinStyle( GXJoinStyle style );
    
    void addRect( const GXRect &rect );
    void addRoundedRect( const GXRect &rect, float arcW , float arcH );
    
    void addEllipse( const GXRect &rect );
    void addArc( const GXPoint &center ,  const GXSize &size , float startAngle /* rads*/ , float angleExtent /* rads*/ ,GXArcType type );
    void addLine( const GXLine &line );
    
    void moveToPoint         ( const GXPoint &point);
    void addLineToPoint      ( const GXPoint &point);
    void addCubicCurveToPoint( const GXPoint &controlPoint0 , const GXPoint &controlPoint1 , const GXPoint &endPoint );
    void addQuadCurveToPoint ( const GXPoint &controlPoint , const GXPoint &endPoint  );
    void close();

    void stroke();
    void fill();
    
    void clear();
    
protected:
    void addPainter(Painter *painter  );

    
    void prepareRessources();
    void deleteRessources();
    void paint( const GXRect &rect , GXAnimation* anim );
    void changed();
    
    
private:
    std::vector<Painter*> m_paintOperations;
    

    


    
};

#endif /* defined(__Broadway_test__GXPaintJS__) */
