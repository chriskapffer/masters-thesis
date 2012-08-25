//
//  Singleton.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 22.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_Singleton_h
#define CKObjectTrackerLib_Singleton_h

// define for singleton declaration
#define DECLARE_SINGLETON( NAME )           \
    public:                                 \
        static NAME * Instance();           \
        static void Finish();               \
                                            \
    private:                                \
        NAME();                             \
        NAME( const NAME& );                \
        ~NAME();                            \
                                            \
        static NAME * instance;

// define for singleton definition
#define DEFINE_SINGLETON( NAME )            \
                                            \
    NAME * NAME::instance = NULL;           \
                                            \
    NAME* NAME::Instance()                  \
    {                                       \
        if (instance == NULL) {             \
            instance = new NAME();          \
        }                                   \
        return instance;                    \
    }                                       \
                                            \
    void NAME::Finish()                     \
    {                                       \
        if (instance != NULL) {             \
            delete instance;                \
            instance = NULL;                \
        }                                   \
    }

#endif
