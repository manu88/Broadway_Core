//
//  AudioGraph.h
//  
//
//  Created by Manuel Deneu on 19/08/14.
//
//

#ifndef ____AudioGraph__
#define ____AudioGraph__

#include <iostream>
#include <list>
#include <assert.h>

class AUElement; //forward

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
//! Internal representation of an audio element in a dsp chain.
/********************************************//**
*
*   the duo AudioNode / AudioGraph let you interconnect multiple audio elements inside 
*   a complex routing scheme, and handle the data for you.
*   You're not supposed to use this class and its methods directicaly, but rather use an AudioGraph.
*
*
*   Note : when attaching an AUElement to an AudioNode, the audio element will be owned by the node and thus
*   maybe deleted or changed, unless you call AudioNode::detachElement() at some point, whitch will remove the element from 
*   its node, and also its graph, and therefore will not be deleted nor modified (It will also become kind of useless, since
*   no graph will handle it).
*
*
*   \sa AudioGraph AUELement
*
***********************************************/
class AudioNode
{
public:
    //! Constructor
    /*!
     \param audioElement The audio Element to attach to the created node. 
     \param nodeID the node's unique identifier.
     
     Once again, the attached AUElement will be owned by it's node, and the overall graph.
     
     \sa AudioGraph::addNode()
     */
    AudioNode(AUElement* audioElement, int nodeID );
    
    //! will delete the attached AUElement
    ~AudioNode();
    
    //! return the node's unique identifier.
    int getNodeID() const
    {
        return m_nodeID;
    }
    
    //! return the attached AUElement.
    AUElement* getAudioElement() const
    {
        return m_ownedElement;
    }
    
    //! detach the audio element from it's node
    /*!
        By calling this method, you remove a element from the overall graph and create a "hole" in 
        the DSP Chain. 
        Also, you'll get custody of the returned AUElement, whitch will not be deleted automaticaly.
     
     \return the detached audio element.
     */
    AUElement* detachAudioElement();
    
    /* **** **** */

    //! Sets the node, to borrow audio data from.
    void setSourceNode(AudioNode* sourceNode)
    {
        assert( (sourceNode != nullptr) && (sourceNode != this) );
        
        m_source = sourceNode;
    }

    //! Sets the node, to feed audio fata.
    void setDestinationNode(AudioNode* dstNode)
    {
        assert( (dstNode != nullptr) && ( dstNode != this) );
        
        m_destination = dstNode;
    }
    
    /* **** **** */
    
    bool hasSource() const
    {
        return m_source != nullptr;
    }
    
    bool hasDestination() const
    {
        return m_destination != nullptr;
    }
    
protected:
    
    AUElement      *m_ownedElement;
    int             m_nodeID;
    
    AudioNode      *m_source;
    AudioNode      *m_destination;
    
    
};


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class AudioGraph
{
public:
    AudioGraph();
    ~AudioGraph();
    
    int addNode(AUElement* elementToAdd);
    bool removeNode(int nodeIdToRemove);
    
    bool connect(int sourceNode , int destNode);
    
    int getNodeCount() const
    {
        return (int) m_nodesList.size();
    }
    
    
private:
    std::list<AudioNode*> m_nodesList;

    static int s_idCounter;
    
    static int getNextId()
    {
        return ++s_idCounter;
    }
    
    static int getCurrentId()
    {
        return s_idCounter;
    }

    
};



#endif /* defined(____AudioGraph__) */
