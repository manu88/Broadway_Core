//
//  AbstractDelegate.h
//  MediaCenter
//
//  Created by Manuel Deneu on 04/04/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#ifndef __MediaCenter__AbstractDelegate__
#define __MediaCenter__AbstractDelegate__

class AbstractController;

class AbstractDelegate
{
public:

    virtual ~AbstractDelegate();


protected:
    AbstractDelegate();
    
    
    //! Controllers will call this method to check if everything is ok.
    //! When false is return, the callback is ignored, and this incomming event is lost.
    virtual bool delegateReadyForController( const AbstractController* controller ) = 0;
    
    

    
    
};

#endif /* defined(__MediaCenter__AbstractDelegate__) */
