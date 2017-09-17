#ifndef __SINGLETON_H__
#define __SINGLETON_H__

#include <Lock.h>
//----------------------------------------------------------------------

template <class T> struct CreateUsingNew
{
    static T* Create (void)
    {
        return new T;
    }

    static void Destroy(T* p)
    {
        delete p;
    }
};
//----------------------------------------------------------------------

template <class T, template <class> class CreationPolicy = CreateUsingNew>
class CSingleton
{
public: //method
    static T* Instance (void);
    static void Destroy (void);
    
public: //properties

protected: //method
protected: //properties

private: //method
    CSingleton (void);
    CSingleton (const CSingleton&);
    CSingleton& operator= (const CSingleton&);

private: //properties
    static T*       _instance;
    static CMutex   _mutex;
};
//----------------------------------------------------------------------

//implement
#include "Singleton.inl"
//----------------------------------------------------------------------

#endif //__SINGLETON_H__
