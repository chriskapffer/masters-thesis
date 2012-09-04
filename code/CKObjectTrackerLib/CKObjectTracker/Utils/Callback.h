//
//  Callback.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 02.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_Callback_h
#define CKObjectTrackerLib_Callback_h

namespace ck {
    
    template <typename TParam>
    class CallBackSingleArg
    {
    public:
        virtual ~CallBackSingleArg() { std::cout << "base" << std::endl; }
        virtual void call(const TParam& p) = 0;
    };
        
    template <typename TParam>
    class CallBackToStaticSingleArg : public CallBackSingleArg<TParam>
    {
        void (*function)(const TParam&);
    public:
        CallBackToStaticSingleArg(void(*function)(const TParam&)) : function(function) { };
        void call(const TParam& p) {
            (*function)(p);
        }
    };
    
    template <typename TParam, typename TClass>
    class CallBackToMemberSingleArg : public CallBackSingleArg<TParam>
    {
        void (TClass::*function)(const TParam&);
        TClass* object;
    public:
        CallBackToMemberSingleArg(TClass* object, void(TClass::*function)(const TParam&)) : object(object), function(function) { };
        ~CallBackToMemberSingleArg() { std::cout << "sub" << std::endl; }
        void call(const TParam& p) {
            (*object.*function)(p);
        }
    };
    
    template <typename TParam>
    class CallBackReturn
    {
    public:
        virtual ~CallBackReturn() { }
        virtual TParam call() = 0;
    };
    
    template <typename TParam>
    class CallBackToStaticReturn : public CallBackReturn<TParam>
    {
        TParam (*function)();
    public:
        CallBackToStaticReturn(TParam(*function)()) : function(function) { };
        TParam call() {
            return (*function)();
        }
    };
    
    template <typename TParam, typename TClass>
    class CallBackToMemberReturn : public CallBackReturn<TParam>
    {
        TParam (TClass::*function)();
        TClass* object;
    public:
        CallBackToMemberReturn(TClass* object, TParam(TClass::*function)()) : object(object), function(function) { };
        ~CallBackToMemberReturn() { std::cout << "sub" << std::endl; }
        TParam call() {
            return (*object.*function)();
        }
    };
    
    template <typename TParam, typename TClass>
    class CallBackToMemberConstReturn : public CallBackReturn<TParam>
    {
        TParam (TClass::*function)() const;
        TClass* object;
    public:
        CallBackToMemberConstReturn(TClass* object, TParam(TClass::*function)() const) : object(object), function(function) { };
        TParam call() {
            return (*object.*function)();
        }
    };
} // end of namespace

#endif
