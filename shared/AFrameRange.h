#ifndef AFRAMERANGE_H
#define AFRAMERANGE_H

/*
 *  AFrameRange.h
 *  
 *
 *  Created by jian zhang on 7/2/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */
#include <vector>
#include <string>

namespace aphid {

class AFrameRange {
public:
	AFrameRange();
	virtual ~AFrameRange();
	void reset();
	bool isValid() const;
	int numFramesInRange() const;
    static float FramesPerSecond;
	static int FirstFrame;
	static int LastFrame;
	static int SamplesPerFrame;
	static std::string SegmentExpr;
};

class AFrameRangeSegment {
  
    struct RangeSegment {
        int m_begin;
        int m_end;
        int m_samples;
    };
    
    std::vector<RangeSegment> m_data;
    
public:
    AFrameRangeSegment();
    
    bool create(const std::string & src);
    bool getSamples(int & dst, const int & frame) const;
    
    friend std::ostream& operator<<(std::ostream &output, const AFrameRangeSegment & p) {
        output << p.str();
        return output;
    }
    
    const std::string str() const;
	
};

}
#endif        //  #ifndef AFRAMERANGE_H
