//
//  Commons.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 06.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_Commons_h
#define CKObjectTrackerLib_Commons_h

namespace ck {

template <typename T>
inline static bool vectorContains(const std::vector<T>& vector, const T& object)
{
    return find(vector.begin(), vector.end(), object) != vector.end();
}

template<typename T>
inline static void setItemCounts(const std::vector<std::pair<std::string, std::vector<T> > >& vectorItems, std::vector<std::pair<std::string, int> >& vectorItemCounts)
{
    typename std::vector<std::pair<std::string, std::vector<T> > >::const_iterator iter;
    for (iter = vectorItems.begin(); iter != vectorItems.end(); iter++) {
        vectorItemCounts.push_back(make_pair((*iter).first, (*iter).second.size()));
    }
}

template<typename T>
inline void sumItems(const std::vector<std::pair<std::string, T> >& addends, std::vector<std::pair<std::string, T> >& sums)
{
    bool isPresent;
    for (int i = 0; i < addends.size(); i++) {
        isPresent = false;
        for (int j = 0; j < sums.size(); j++) {
            if (sums[j].first == addends[i].first) {
                sums[j].second += addends[i].second;
                isPresent = true;
                break;
            }
        }
        if (!isPresent) { sums.push_back(addends[i]); }
    }
}

template<typename T>
inline void divideItems(const std::vector<std::pair<std::string, T> >& numerators, std::vector<std::pair<std::string, T> >& fractions, float denominator)
{
    for (int i = 0; i < numerators.size(); i++) {
        std::pair<std::string, T> pair = numerators[i];
        fractions.push_back(make_pair(pair.first, pair.second / denominator));
    }
}

} // end of namespace
    
#endif
