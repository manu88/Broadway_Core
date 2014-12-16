/*!
 *  \brief     Java Script Virtual Machine. See TinyJS
 *  \details
 *  \author    Manuel Deneu
 *  \version   0
 *  \date      2014
 *  \pre       .
 *  \bug       .
 *  \warning   .
 *  \copyright GNU Public License.
 */

#ifndef __JSMachine__JSMachineDelegate__
#define __JSMachine__JSMachineDelegate__


#include "../Internal/Object.h"


//! JSMachineDelegate
/********************************************//**
*
*   An instance of this Class catches notification from a JSMachine object.
*
***********************************************/

class Selector; // forward

class JSMachineDelegate 
{
public:
    virtual ~JSMachineDelegate();
    
    //! This method needs to be subclassed by your class.
    /*!
     \param selector The called selector
     \sa JSMachine, Selector
     */
    virtual void functionCalled( const Selector *selector ) =0;
    
protected:
    JSMachineDelegate();

private:
    
    
    
};
#endif /* defined(__JSMachine__JSMachineDelegate__) */
