//
//  MapInitializer.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 26.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_MapInitializer_h
#define CKObjectTrackerLib_MapInitializer_h

namespace ck {

// based on http://stackoverflow.com/a/1730798/782862
template <typename TKey, typename TValue>
class createMap
{
private:
    std::map<TKey, TValue> _map;
public:
    createMap(const TKey& key, const TValue& val)
    {
        _map[key] = val;
    }
    
    createMap<TKey, TValue>& operator()(const TKey& key, const TValue& val)
    {
        _map[key] = val;
        return *this;
    }
    
    operator std::map<TKey, TValue>()
    {
        return _map;
    }
};
    
} // end of namespace
    
#endif
