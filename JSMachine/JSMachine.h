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

#ifndef __JSMachine__JSMachine__
#define __JSMachine__JSMachine__

#include <iostream>
#include <string.h>
#include <vector>



#include "../Internal/Object.h"
#include "../Internal/Element.h"

#include "../Data/Database.h"


#define TINYJS_CALL_STACK
#include "TinyJS/TinyJS.h"


class JSMachineDelegate; // forward


//! Selector
/********************************************//**
*
*    Class for callback from JSMachine's to JSMachineDelegate's instances
*
***********************************************/
class Selector : public Object
{
public :
    

    //! Selector's Constructor
    /*!
     \param _signature the JavaScript signature of the called function without ';'. E.g: foo( var1 , var2 ).
     \param _delegate  the instance JSMachineDelegate on whitch to call the function.
     \sa JSMachine, JSMachineDelegate
     */
    Selector(const std::string &_signature , JSMachineDelegate* _delegate);
    
    
    ~Selector();
    
    void callFunctionwithVars(CScriptVar *vars);
    

    
    //! The signature of the function passed in the constructor.
    std::string         signature;
    
    //! The name of the function
    /*!
        Eg : signature = "foo(var1)" -> identifier = "foo". 
        This variable can be used for comparisons, or retrieve a selector.
     */
    std::string         identifier;
    
    //! A ref to the JSMachineDelegate's instance
    /*!
        This pointer is ref-only, it will not be deleted!
     */
    JSMachineDelegate  *delegate;

    //! The set of variables to pass with the call
    /*!
        See TinyJS's Documentation
     */
    CScriptVar         *variables; // refs

    //! Equality comparison between to instances of Selector
    bool operator==( const Selector& rhs);
};




/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** ****  */

//! JSMachine
/********************************************//**
*
*    This class can parse js files, run a real time interpreter, and notify a delegate when a function is called using Selectors.
*
*    In order to parse & interpret javascript code, TinyJS is used.
*    See https://code.google.com/p/tiny-js/                                               
***********************************************/

class JSMachine : public Object
{
public:
    JSMachine();

    ~JSMachine();
    
    void allowSystemCalls( bool shouldI )
    {
        _allowSystemCall = shouldI;
    }
    
    void test();

    
    //! Sets the delegate that will be called
    /*!
     \param delegate The subclass of JSMachineDelegate you want to be notified
     \sa Selector, JSMachineDelegate
     */
    void setDelegate( JSMachineDelegate *delegate )
    {
        _delegate = delegate;
    }

    //! Add and register a JS function to the look-up table
    /*!
     \param signature The function's signature. E.g: "foo(var1 , var2)". 
     */
    void registerFunctionWithSignature(const std::string &signature);


    
    //! Remove a Selector from the look-up table
    /*!
     \param signature The signature of the function to remove. 
     */
    void removeRegisteredFunctionWithSignature(const std::string &signature);
    
    //! Get the registered functions' count.
    /*!
     \return The look-up table' size.
     */
    int getRegisteredFunctionsCount() const
    {
        return ( int ) _registeredSelectors.size();
    }
    
    //! Objective-C style test method.
    /*!
     \param signature The signature to test.
     \return True if the signature can be called.
     */
    bool respondsToSelector(const std::string &signature);
    
    
    //! Text file Parser
    /*!
     \param filename The JS file to parse.
     \return True if no error occured during parsing.
     */
    bool parseScriptFile( const std::string &filename );
    
    bool importScriptFile( const std::string &filename );
    
    //! Evaluate the content of a text buffer
    /*!
     \param buffer The buffer to parse.
     \return true if no errors.
     */
    bool executeBuffer(  const std::string &buffer);
    
    // without ";" !!!!
    CScriptVarLink evaluate( const std::string &buffer);
    std::string    evaluateAsString(const std::string &buffer);

    void reset();
    
    
    static VariantList getArgumentsFromJSArray( CScriptVar *vars);
    
    static CScriptVar* getArgumentsAsJSArray( const VariantList &list );
    
    static std::string getArgumentsAsJSArrayString( const VariantList &list );
    static std::string getArgumentsAsStringList( const VariantList &list );
    
    template<typename Type>
    static CScriptVar* getJSArrayFromVector( const std::vector<Type> &vect)
    {
        if ( vect.empty() )
            return nullptr;
        
        CScriptVar* ret = new CScriptVar();
        
        ret->setArray();
        
        int count = 0;
        
        for ( auto val : vect)
        {
            ret->setArrayIndex(count, new CScriptVar( val ));
            count++;
        }
        
        return ret;
    }
    
    template<typename Type>
    static CScriptVar* getJSArrayFromDatabase( const Database &data)
    {
        const int size = data.getSize();
        if ( size == 0 )
            return nullptr;
        
        CScriptVar* ret = new CScriptVar();
        
        ret->setArray();
        
        for (int i = 0; i< size; i++)
        {
            const std::string name = data.getItemNameAtIndex( i );
            const Type value = data.getValueAtIndex( i );
            
            CScriptVar *item = new CScriptVar();
            item->setArray();
            
            item->setArrayIndex(0, new CScriptVar( name ));
            item->setArrayIndex(1, new CScriptVar( value ));
            
            ret->setArrayIndex( i, item);
        }
        
        return ret;
    }

    static std::string getDatabaseAsJSArrayString( const Database &data)
    {
        std::ostringstream stream;
        
        const int size = data.getSize();
        if ( size == 0 )
            return "undefined";
        
        stream << "[";


            
        for (int i = 0; i< size; i++)
        {
            if (i != 0)
                stream << ",";
            
            stream << "["
            << "\"" << data.getItemNameAtIndex( i ) << "\""
            << ","
            << "\"" << data.getValueAtIndex( i ).getString() << "\""
            << "]";
            
        }

        stream << "]";
        
        return stream.str();
    }
    
    
protected:
    
    void prepareEnvironment();

    // called by reset() only!
    void addRegisteredSelectors();
    
    void clearStack();
    
    //! Dump the stack's content
    /*!
        See TinyJS documentation
     */
    static void js_dumpStack(CScriptVar *v, void *userdata);
    
    //! print() method
    /*!
     See TinyJS documentation
     */
    static void js_print(CScriptVar *v, void *userdata);
    
    //! internal Callback for selectors & registered methods
    /*!
        This will make a call to JSMachineDelegate's functionCalled() method
     */
    static void js_nativeFunctWithID(CScriptVar *v, void *userdata);
    
    static void js_readDataFile( CScriptVar *v, void *userdata );
//    static void js_readDir( CScriptVar *v, void *userdata );
    static void js_system( CScriptVar *v, void *userdata );
    static void js_getTime( CScriptVar *v, void *userdata );
    
    static void js_importFile( CScriptVar *v, void *userdata );
    
private:
    
    //! remove all selectors from the look-up table
    void removeAllRegisteredFunctions();
    
    void removeAllImportedScripts();
    
    bool findSelectorBySignature( const std::string &signature );
    
    bool _allowSystemCall;
    
    JSMachineDelegate *_delegate;
    
    CTinyJS _machine;
    
    std::vector< Selector* > _registeredSelectors;
    
    std::vector< std::string > _importedHeaders;
    
    
    
    

};



#endif /* defined(__JSMachine__JSMachine__) */
