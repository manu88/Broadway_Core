
#include <string>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <errno.h>
#include "IPC.h"



IPC::Reader::Reader( const std::string &file):
_connected ( false ),
_msqid     ( -1 ),
_key       ( -1 )
{
    if ((_key = ftok(file.c_str(), 'B')) == -1) {  /* same key as kirk.c */
        perror("ftok");
        _connected = false;
    }
    
    if (( _msqid = msgget(_key, 0644)) == -1)
    {
        perror("msgget");
        _connected = false;
    }
    
    _connected = true;
}

IPC::Reader::~Reader()
{
    
}


bool IPC::Reader::read(void* buffer , long size) const
{
    return msgrcv( _msqid, buffer, size, 0, 0) != -1;
}

std::string IPC::Reader::read() const
{
    MsgBuffer in;
    
    if (read( &in, MaxMessageSize) )
        return std::string( in.mtext );
    
    return std::string();

}

/* ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** */
/* ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** */

IPC::Writer::Writer( const std::string &file):
_connected ( false ),
_msqid     ( -1 ),
_key       ( -1 )
{
    if (( _key = ftok(file.c_str(), 'B')) == -1)
    {
        perror("ftok");
        _connected = false;

    }
    
    if (( _msqid = msgget( _key, 0644 | IPC_CREAT)) == -1)
    {
        perror("msgget");
        _connected = false;

    }
    
    _connected = true;
}

IPC::Writer::~Writer()
{
    
}


bool IPC::Writer::write(const void* buffer , long size)
{
    if (!_connected)
        return false;
    
    return msgsnd( _msqid, buffer, size, 0) != -1;
}

bool IPC::Writer::removeId()
{
    return msgctl( _msqid, IPC_RMID, NULL) != -1;
}

/* ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** */
/* ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** */

IPC::SharedMemory::SharedMemory(key_t key):
_memid  ( -1  ),
_key    ( key ),
_handle ( nullptr )
{
    
}

IPC::SharedMemory::~SharedMemory()
{
    if (  _handle != nullptr)
        shmdt( _handle );
    
}

long IPC::SharedMemory::alloc( long size)
{
    _memid = shmget( _key , size, 0700 | IPC_CREAT);


    if( errno == 22)
    {
        printf("\n ERROR Size");
        
        return -1;
    }
    
    if( _memid !=-1)
    {
        _handle = shmat( _memid, NULL, 0);
        
        struct shmid_ds stat;
        memset(&stat, 0, sizeof(struct shmid_ds));
        
        
        if ( shmctl( _memid , IPC_STAT , &stat) != -1)
        {
            return stat.shm_segsz;
        }
        else
            printf("\n ERROR WHILE STATING SHARED MEM");
    }
    
    return  -1;
}

bool IPC::SharedMemory::dealloc()
{
    const int ret = shmctl( _memid,IPC_RMID,0);
    
    _handle = nullptr;
    
    return ret !=-1 ;
}


