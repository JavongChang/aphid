/*
 *  LfWorld.h
 *  
 *
 *  Created by jian zhang on 11/22/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */
#include <string>
#include <vector>

class ZEXRImage;

namespace lfr {

class LfParameter {

	std::vector<std::string > m_imageNames;
	std::string m_imageName;
	int m_atomSize;
	int m_dictionaryLength;
	int m_numPatches;
	bool m_isValid;
public:
	LfParameter(int argc, char *argv[]);
	virtual ~LfParameter();
	
	bool isValid() const;
	int atomSize() const;
	int dictionaryLength() const;
	int numPatches() const;
	int dimensionOfX() const;
	int randomImageInd() const;
	std::string imageName(int i) const;
	static void PrintHelp();
protected:

private:
	bool searchImagesIn(const char * dirname);
	void countPatches();
};

template<typename T> class DenseMatrix;

class LfWorld  {
	struct ImageInd {
		ZEXRImage * _image;
		int _ind;
	};
	
#define MAX_NUM_OPENED_IMAGES 16 
	ImageInd m_openedImages[MAX_NUM_OPENED_IMAGES];
	int m_currentImage;
	
	const LfParameter * m_param;
	DenseMatrix<float> * m_D;
	DenseMatrix<float> * m_G;
public:

	LfWorld(const LfParameter & param);
	virtual ~LfWorld();
	
	const LfParameter * param() const;
/// randomly set dictionary, set result in image in qRgba format
	void fillDictionary(unsigned * imageBits, int imageW, int imageH);
	void fillPatch(unsigned * dst, float * color, int s, int imageW, int rank = 3);
protected:
	bool isImageOpened(const int ind, ZEXRImage * img) const;
	void openImage(const int ind, ZEXRImage * img);
private:
	void cleanDictionary();
};
}

