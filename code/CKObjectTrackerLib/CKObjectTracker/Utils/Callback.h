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
        virtual ~CallBackSingleArg() { }
        virtual CallBackSingleArg* clone() = 0;
        virtual void call(const TParam& p) = 0;
    };
        
    template <typename TParam>
    class CallBackToStaticSingleArg : public CallBackSingleArg<TParam>
    {
        void (*function)(const TParam&);
    public:
        CallBackToStaticSingleArg(void(*function)(const TParam&)) : function(function) { }
        ~CallBackToStaticSingleArg() { /* DO NOT DELETE ANYTHING */ }
        CallBackSingleArg<TParam>* clone() { return new CallBackToStaticSingleArg(function); }
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
        CallBackToMemberSingleArg(TClass* object, void(TClass::*function)(const TParam&)) : object(object), function(function) { }
        ~CallBackToMemberSingleArg() { /* DO NOT DELETE ANYTHING */ }
        CallBackSingleArg<TParam>* clone() { return new CallBackToMemberSingleArg(object, function); }
        void call(const TParam& p) {
            (*object.*function)(p);
        }
    };
    
    template <typename TParam>
    class CallBackReturn
    {
    public:
        virtual ~CallBackReturn() { }
        virtual CallBackReturn* clone() = 0;
        virtual TParam call() = 0;
    };
    
    template <typename TParam>
    class CallBackToStaticReturn : public CallBackReturn<TParam>
    {
        TParam (*function)();
    public:
        CallBackToStaticReturn(TParam(*function)()) : function(function) { }
        ~CallBackToStaticReturn() { /* DO NOT DELETE ANYTHING */ }
        CallBackReturn<TParam>* clone() { return new CallBackToStaticReturn(function); }
        TParam call() {
            return (*function)();
        }
    };
    
    template <typename TParam, typename TClass>
    class CallBackToMemberReturn : public CallBackReturn<TParam>
    {
        TParam (TClass::*function)() const;
        TClass* object;
    public:
        CallBackToMemberReturn(TClass* object, TParam(TClass::*function)() const) : object(object), function(function) { }
        ~CallBackToMemberReturn() { /* DO NOT DELETE ANYTHING */ }
        CallBackReturn<TParam>* clone() { return new CallBackToMemberReturn(object, function); }
        TParam call() {
            return (*object.*function)();
        }
    };

} // end of namespace

#endif
