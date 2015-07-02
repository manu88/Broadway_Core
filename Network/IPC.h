//
//  IPC.h
//  testIPC2
//
//  Created by Manuel Deneu on 29/06/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#ifndef __testIPC2__IPC__
#define __testIPC2__IPC__

#include <sys/types.h>

namespace IPC
{
    enum { MaxMessageSize = 256 };
    
    typedef struct
    {
        long mtype;
        char mtext[MaxMessageSize];
    } MsgBuffer;


    class Writer
    {
    public:
        Writer( const std::string &file);
        ~Writer();
        
        bool write(const void* buffer , long size);
        
        bool removeId();
        
    private:
        
        bool  _connected;
        int   _msqid;
        key_t _key;
    };

    /* *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** */

    class Reader
    {
    public:
        Reader( const std::string &file);
        ~Reader();
        
        bool read(void* buffer , long size) const ;
        
        std::string read() const;
    private:
        
        bool  _connected;
        int   _msqid;
        key_t _key;
    };
    
    /* *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** */
    
    class SharedMemory
    {
    public:
        SharedMemory( key_t key );
        ~SharedMemory();
        
        long alloc( long size);
        bool dealloc();

        
        template <typename T>  T getHandle() const
        {
            return static_cast<T>( _handle );
        }
        
        template <typename T>  T* operator->() const
        {
            return static_cast<T>( _handle );
        }
        
    private:
        

        int _memid;
        key_t _key;
        void* _handle;
        

        
    };
}

#endif /* defined(__testIPC2__IPC__) */
