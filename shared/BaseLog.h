#ifndef BASELOG_H
#define BASELOG_H

/*
 *  BaseLog.h
 *  testnarrowpahse
 *
 *  Created by jian zhang on 3/29/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
class BaseLog {
public:
	BaseLog(const std::string & fileName);
	virtual ~BaseLog();
	
	void write(const std::string & os);
	void write(unsigned & i);
	void writeTime();
	void newLine();
protected:
	template <typename T>
	void _write(const T & a) {
		m_file<<" "<<a<<" ";
	}

private:
	std::ofstream m_file;
};
#endif        //  #ifndef BASELOG_H

