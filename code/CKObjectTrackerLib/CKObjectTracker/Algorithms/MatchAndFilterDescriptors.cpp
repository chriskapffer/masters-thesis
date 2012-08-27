//
//  MatchAndFilterDescriptors.cpp
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 27.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#include "MatchAndFilterDescriptors.h"
#include "Profiler.h"
#include "Utils.h"

#define MATCHES_TOTAL "total"
#define MATCHES_MASKED "masked"
#define MATCHES_CROPPED "cropped"
#define MATCHES_AFTER_RATIO "ratio"
#define MATCHES_AFTER_SYMMETRY "symmetry"

using namespace std;
using namespace cv;
using namespace ck;

void MatcherFilter::getFilteredMatches(const DescriptorMatcher& matcher, const Mat& descriptors1, const Mat& descriptors2, vector<DMatch>& result, const vector<FilterFlag>& flags, bool sortMatches, float ratio, int nBestMatches, vector<pair<string, vector<DMatch> > >& debugger)
{
    if (descriptors1.empty() || descriptors2.empty())
        return;
    
    if(contains(flags, FILTER_FLAG_RATIO)) {
        if (contains(flags, FILTER_FLAG_SYMMETRY)) {
            matchAndApplyFiltersAll(matcher, descriptors1, descriptors2, result, flags, sortMatches, ratio, nBestMatches, debugger);
        } else {
            matchAndApplyFiltersNoSymmetry(matcher, descriptors1, descriptors2, result, flags, sortMatches, ratio, nBestMatches, debugger);
        }
    } else {
        if (contains(flags, FILTER_FLAG_SYMMETRY)) {
            matchAndApplyFiltersNoRatio(matcher, descriptors1, descriptors2, result, flags, sortMatches, nBestMatches, debugger);
        } else {
            matchAndApplyFiltersOnlyCrop(matcher, descriptors1, descriptors2, result, flags, sortMatches, nBestMatches, debugger);
        }
    }
}

void MatcherFilter::matchAndApplyFiltersAll(const DescriptorMatcher& matcher, const Mat& descriptors1, const Mat& descriptors2, vector<DMatch>& result, const vector<FilterFlag>& flags, bool sortMatches, float ratio, int nBestMatches, vector<pair<string, vector<DMatch> > >& debugger)
{
    vector<vector<DMatch> > matches12;
    vector<vector<DMatch> > matches21;
    Profiler* profiler = Profiler::Instance();
    profiler->startTimer(TIMER_MATCH);
    matcher.knnMatch(descriptors1, descriptors2, matches12, 2);
    matcher.knnMatch(descriptors2, descriptors1, matches21, 2);
    debugger.push_back(make_pair(MATCHES_TOTAL, utils::stripNeighbors(matches12)));
    profiler->stopTimer(TIMER_MATCH);
    profiler->startTimer(TIMER_FILTER);
    if (sortMatches) {
        sort(matches12.begin(), matches12.end(), compareKnnMatch);
        sort(matches21.begin(), matches21.end(), compareKnnMatch);
    }
    
    vector<vector<DMatch> > tmp;
    bool didSymmetryMatch = false;
    for (int i = 0; i < flags.size(); i++) {
        switch (flags[i]) {
            case FILTER_FLAG_CROP:
                utils::nBestMatches(matches12, tmp, nBestMatches, sortMatches);
                debugger.push_back(make_pair(MATCHES_CROPPED, utils::stripNeighbors(tmp)));
                matches12 = tmp;
                if (!didSymmetryMatch) {
                    utils::nBestMatches(matches21, tmp, nBestMatches, sortMatches);
                    matches21 = tmp;
                }
                break;
            case FILTER_FLAG_RATIO:
                utils::ratioTest(matches12, tmp, ratio);
                debugger.push_back(make_pair(MATCHES_AFTER_RATIO, utils::stripNeighbors(tmp)));
                matches12 = tmp;
                if (!didSymmetryMatch) {
                    utils::ratioTest(matches21, tmp, ratio);
                    matches21 = tmp;
                }
                break;
            case FILTER_FLAG_SYMMETRY:
                utils::symmetryTest(matches12, matches21, tmp);
                debugger.push_back(make_pair(MATCHES_AFTER_SYMMETRY, utils::stripNeighbors(tmp)));
                matches12 = tmp;
                didSymmetryMatch = true;
                break;
        }
    }
    result = utils::stripNeighbors(matches12);
    profiler->stopTimer(TIMER_FILTER);
}

void MatcherFilter::matchAndApplyFiltersNoRatio(const DescriptorMatcher& matcher, const Mat& descriptors1, const Mat& descriptors2, vector<DMatch>& result, const vector<FilterFlag>& flags, bool sortMatches, int nBestMatches, vector<pair<string, vector<DMatch> > >& debugger)
{
    vector<DMatch> matches12;
    vector<DMatch> matches21;
    Profiler* profiler = Profiler::Instance();
    profiler->startTimer(TIMER_MATCH);
    matcher.match(descriptors1, descriptors2, matches12);
    matcher.match(descriptors2, descriptors1, matches21);
    debugger.push_back(make_pair(MATCHES_TOTAL, matches12));
    profiler->stopTimer(TIMER_MATCH);
    profiler->startTimer(TIMER_FILTER);
    if (sortMatches) {
        sort(matches12.begin(), matches12.end());
        sort(matches21.begin(), matches21.end());
    }
    
    vector<DMatch> tmp;
    bool didSymmetryMatch = false;
    for (int i = 0; i < flags.size(); i++) {
        switch (flags[i]) {
            case FILTER_FLAG_CROP:
                utils::nBestMatches(matches12, tmp, nBestMatches, sortMatches);
                debugger.push_back(make_pair(MATCHES_CROPPED, tmp));
                matches12 = tmp;
                if (!didSymmetryMatch) {
                    utils::nBestMatches(matches21, tmp, nBestMatches, sortMatches);
                    matches21 = tmp;
                }
                break;
            case FILTER_FLAG_SYMMETRY:
                utils::symmetryTest(matches12, matches21, tmp);
                debugger.push_back(make_pair(MATCHES_AFTER_SYMMETRY, tmp));
                matches12 = tmp;
                didSymmetryMatch = true;
                break;
            default:
                break;
        }
    }
    result = matches12;
    profiler->stopTimer(TIMER_FILTER);
}

void MatcherFilter::matchAndApplyFiltersNoSymmetry(const DescriptorMatcher& matcher, const Mat& descriptors1, const Mat& descriptors2, vector<DMatch>& result, const vector<FilterFlag>& flags, bool sortMatches, float ratio, int nBestMatches, vector<pair<string, vector<DMatch> > >& debugger)
{
    vector<vector<DMatch> > matches;
    Profiler* profiler = Profiler::Instance();
    profiler->startTimer(TIMER_MATCH);
    matcher.knnMatch(descriptors1, descriptors2, matches, 2);
    debugger.push_back(make_pair(MATCHES_TOTAL, utils::stripNeighbors(matches)));
    profiler->stopTimer(TIMER_MATCH);
    profiler->startTimer(TIMER_FILTER);
    if (sortMatches) { sort(matches.begin(), matches.end(), compareKnnMatch); }
    
    vector<vector<DMatch> > tmp;
    for (int i = 0; i < flags.size(); i++) {
        switch (flags[i]) {
            case FILTER_FLAG_CROP:
                utils::nBestMatches(matches, tmp, nBestMatches, sortMatches);
                debugger.push_back(make_pair(MATCHES_CROPPED, utils::stripNeighbors(tmp)));
                matches = tmp;
                break;
            case FILTER_FLAG_RATIO:
                utils::ratioTest(matches, tmp, ratio);
                debugger.push_back(make_pair(MATCHES_AFTER_RATIO, utils::stripNeighbors(tmp)));
                matches = tmp;
                break;
            default:
                break;
        }
    }
    result = utils::stripNeighbors(matches);
    profiler->stopTimer(TIMER_FILTER);
}

void MatcherFilter::matchAndApplyFiltersOnlyCrop(const DescriptorMatcher& matcher, const Mat& descriptors1, const Mat& descriptors2, vector<DMatch>& result, const vector<FilterFlag>& flags, bool sortMatches, int nBestMatches, vector<pair<string, vector<DMatch> > >& debugger)
{
    vector<DMatch> matches;
    Profiler* profiler = Profiler::Instance();
    profiler->startTimer(TIMER_MATCH);
    matcher.match(descriptors1, descriptors2, matches);
    debugger.push_back(make_pair(MATCHES_TOTAL, matches));
    profiler->stopTimer(TIMER_MATCH);
    profiler->startTimer(TIMER_FILTER);
    if (sortMatches) { sort(matches.begin(), matches.end()); }
    
    vector<DMatch> tmp;
    if (!flags.empty()) { // can not be anything else than crop
        utils::nBestMatches(matches, tmp, nBestMatches, sortMatches);
        debugger.push_back(make_pair(MATCHES_CROPPED, tmp));
        matches = tmp;
    }
    result = matches;
    profiler->stopTimer(TIMER_FILTER);
}

void MatcherFilter::filterMatchesWithMask(const vector<DMatch>& matches, const vector<unsigned char>& mask, vector<DMatch>& result, vector<pair<string, vector<DMatch> > >& debugger)
{
    assert(matches.size() == mask.size());
    Profiler* profiler = Profiler::Instance();
    profiler->startTimer(TIMER_FILTER);
    if (!result.empty()) { result.clear(); }
    for (int i = 0; i < mask.size(); i++) {
        if (mask[i] == 1) {
            result.push_back(matches[i]);
        }
    }
    debugger.push_back(make_pair(MATCHES_MASKED, result));
    profiler->stopTimer(TIMER_FILTER);
}
