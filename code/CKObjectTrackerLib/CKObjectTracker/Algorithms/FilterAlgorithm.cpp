//
//  FilterAlgorithm.cpp
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 06.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#include "FilterAlgorithm.h"

using namespace std;
using namespace cv;

namespace ck {
    
    void FilterAlgorithm::ratioTest(const vector<vector<DMatch> >& matches, vector<DMatch>& result, float ratio)
    {
        if (!result.empty()) { result.clear(); }
        vector<vector<DMatch> >::const_iterator iter = matches.begin();
        // for all matches
		for (; iter!= matches.end(); iter++) {
            // ignore matches with less than two neighbors
            if (iter->size() < 2)
                continue;
            // add match if first and second match are far apart from each other
            if ((*iter)[0].distance / (*iter)[1].distance < ratio)
                result.push_back((*iter)[0]);
		}
    }
    
    void FilterAlgorithm::ratioTest(const vector<vector<DMatch> >& matches, vector<vector<DMatch> >& result, float ratio)
    {
        if (!result.empty()) { result.clear(); }
        vector<vector<DMatch> >::const_iterator iter = matches.begin();
        // for all matches
		for (; iter!= matches.end(); iter++) {
            // ignore matches with less than two neighbors
            if (iter->size() < 2)
                continue;
            // add match if first and second match are far apart from each other
            if ((*iter)[0].distance / (*iter)[1].distance < ratio)
                result.push_back(*iter);
		}
    }
    
//    TODO: Cross-check
//    
//    BruteForceMatcher<L2<float> > descriptorMatcher;
//    vector<DMatch> filteredMatches12, matches12, matches21;
//    descriptorMatcher.match( descriptors1, descriptors2, matches12 );
//    descriptorMatcher.match( descriptors2, descriptors1, matches21 );
//    for( size_t i = 0; i < matches12.size(); i++ )
//    {
//        DMatch forward = matches12[i];
//        DMatch backward = matches21[forward.trainIdx];
//        if( backward.trainIdx == forward.queryIdx )
//            filteredMatches12.push_back( forward );
//            }
    
    void FilterAlgorithm::symmetryTest(const vector<DMatch>& matches12, const vector<DMatch>& matches21, vector<DMatch>& result)
    {
        if (!result.empty()) { result.clear(); }
        
        for(int i = 0; i < matches12.size(); i++)
        {
            DMatch forward = matches12[i];
            DMatch backward = matches21[forward.trainIdx];
            if (backward.trainIdx == forward.queryIdx) {
                result.push_back(forward);
            }
        }
//        vector<DMatch>::const_iterator iter12 = matches12.begin();
//        vector<DMatch>::const_iterator iter21 = matches21.begin();
//        // for all matches image 1 -> image 2
//        for (; iter12 != matches12.end(); iter12++) {
//            // for all matches image 2 -> image 1
//            for (; iter21 != matches21.end(); iter21++) {
//                // test if match12 points to same descriptors as match21
//                if ((*iter12).queryIdx == (*iter21).trainIdx
//                    && (*iter21).queryIdx == (*iter12).trainIdx) {
//                    // add match if symmetrical
//                    result.push_back(DMatch((*iter12).queryIdx, (*iter12).trainIdx, (*iter12).distance));
//                    break; // don't need to investigate matches image 2 -> image 1 any further
//                }
//            }
//        }
    }
    
    void FilterAlgorithm::symmetryTest(const vector<vector<DMatch> >& matches12, const vector<vector<DMatch> >& matches21, vector<vector<DMatch> >& result)
    {
        if (!result.empty()) { result.clear(); }
        
        for(int i = 0; i < matches12.size(); i++)
        {
            vector<DMatch> forward = matches12[i];
            vector<DMatch> backward = matches21[forward[0].trainIdx];
            if (backward[0].trainIdx == forward[0].queryIdx) {
                vector<DMatch> symMatch;
                symMatch.push_back(DMatch(forward[0].queryIdx, forward[0].trainIdx, forward[0].distance));
                symMatch.push_back(DMatch(forward[1].queryIdx, forward[1].trainIdx, forward[1].distance));
                result.push_back(symMatch);
            }
        }
//        vector<vector<DMatch> >::const_iterator iter12 = matches12.begin();
//        vector<vector<DMatch> >::const_iterator iter21 = matches21.begin();
//        // for all matches image 1 -> image 2
//        for (; iter12 != matches12.end(); iter12++) {
//            if (iter12->size() < 2)
//                continue; // ignore matches with less than two neighbors
//            
//            // for all matches image 2 -> image 1
//            for (; iter21 != matches21.end(); iter21++) {
//                if (iter21->size() < 2)
//                    continue; // ignore matches with less than two neighbors
//                
//                // test if match12 points to same descriptors as match21
//                if ((*iter12)[0].queryIdx == (*iter21)[0].trainIdx
//                    && (*iter21)[0].queryIdx == (*iter12)[0].trainIdx) {
//                    // add match if symmetrical
//                    vector<DMatch> symMatch;
//                    symMatch.push_back(DMatch((*iter12)[0].queryIdx, (*iter12)[0].trainIdx, (*iter12)[0].distance));
//                    symMatch.push_back(DMatch((*iter12)[1].queryIdx, (*iter12)[1].trainIdx, (*iter12)[1].distance));
//                    result.push_back(symMatch);
//                    break; // don't need to investigate matches image 2 -> image 1 any further
//                }
//            }
//        }
    }
    
    void FilterAlgorithm::nBestMatches(const vector<DMatch>& matches, vector<DMatch>& result, int n, bool sorted)
    {
        n = MIN((int)matches.size(), n);
        
        if (sorted) {
            // copy the first n matches into result vector
            result.assign(matches.begin(), matches.begin() + n);
        } else {
            // copy all matches
            result = matches;
            // make sure that the n best elements are at the first n positions (not ordered themself)
            nth_element(result.begin(), result.begin() + n - 1, result.end());
            // remove all others
            result.erase(result.begin() + n, result.end());
        }
    }
    
    void FilterAlgorithm::nBestMatches(const vector<vector<DMatch> >& matches, vector<vector<DMatch> >& result, int n, bool sorted)
    {
        n = MIN((int)matches.size(), n);
        
        if (sorted) {
            // copy the first n matches into result vector
            result.assign(matches.begin(), matches.begin() + n);
        } else {
            // copy all matches
            result = matches;
            // make sure that the n best elements are at the first n positions (not ordered themself)
            nth_element(result.begin(), result.begin() + n - 1, result.end(), compareKnnMatch);
            // remove all others
            result.erase(result.begin() + n, result.end());
        }
    }
    
    vector<DMatch> FilterAlgorithm::stripNeighbors(const vector<vector<DMatch> >& matches)
    {
        vector<DMatch> result;
        // for all matches - add first neighbor to result and ignore others
		for (vector<vector<DMatch> >::const_iterator iter = matches.begin(); iter!= matches.end(); iter++) {
            if (!(*iter).empty()) {
                result.push_back((*iter)[0]);
            }
		}
        return result;
    }
    
} // end of namespace