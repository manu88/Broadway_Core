//
//  AudioGraph.cpp
//  
//
//  Created by Manuel Deneu on 19/08/14.
//
//

#include "AudioGraph.h"


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

AudioNode::AudioNode(AUElement* audioElement, int nodeID ) :
m_ownedElement ( audioElement ),
m_nodeID       ( nodeID),

m_source       ( nullptr ),
m_destination  ( nullptr )
{
    
}

AudioNode::~AudioNode()
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

AUElement* AudioNode::detachAudioElement()
{
    AUElement* rtn = m_ownedElement;
    
    m_ownedElement = nullptr;
    m_destination->setSourceNode( nullptr );
    m_source->setDestinationNode( nullptr );
    
    return rtn;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

int AudioGraph::s_idCounter = 0;

AudioGraph::AudioGraph()
{
    
}

AudioGraph::~AudioGraph()
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

int AudioGraph::addNode(AUElement* elementToAdd)
{
    m_nodesList.push_back(new AudioNode( elementToAdd , getNextId() ) );
    
    return  getCurrentId();
  
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool AudioGraph::removeNode(int nodeIdToRemove)
{
    for(auto iter = m_nodesList.begin(); iter != m_nodesList.end(); iter++)
    {
        if ((*iter)->getNodeID() == nodeIdToRemove)
        {
            m_nodesList.remove( (*iter));
            
            return true;
        }
        
    }
    return false;

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool AudioGraph::connect(int sourceNode , int destNode)
{
    AudioNode *source = nullptr;
    AudioNode *dest = nullptr;
    
    
    for( auto iter = m_nodesList.begin();iter != m_nodesList.end(); iter++)
    {
        if ((*iter)->getNodeID() == sourceNode)
            source = (*iter);
        
        else if ((*iter)->getNodeID() == destNode)
            dest = (*iter);
        
    }
    
    if (source && dest)
    {
        source->setDestinationNode(dest);
        dest->setSourceNode(source);
        
        return true;
    }
    
    
    return false;
 
}