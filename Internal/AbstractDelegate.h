//
//  AbstractDelegate.h
//  MediaCenter
//
//  Created by Manuel Deneu on 04/04/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#ifndef __MediaCenter__AbstractDelegate__
#define __MediaCenter__AbstractDelegate__

class AbstractDelegate
{
public:

    virtual ~AbstractDelegate();
    
    bool isReady() const
    {
        return _ready;
    }

protected:
    AbstractDelegate();
    
    void setReady( bool ready );
    
    
private:
    
    bool _ready;
    
};

#endif /* defined(__MediaCenter__AbstractDelegate__) */
