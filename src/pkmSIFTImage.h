/*
 *  pkmSIFTImage.h
 *  pkmSIFTImage
 *
 Copyright (C) 2011 Parag K. Mital
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#pragma once

#include <opencv2/opencv.hpp>
using namespace cv;
#include "Image.h"
#include "ImageFeature.h"

enum SIFTMODE {
    MODE_TORRALBA = 0,
    MODE_OPENCV = 1,
    MODE_OPENCV_SURF = 2
};

class pkmSIFTImage
{
public:
    
    pkmSIFTImage();
    ~pkmSIFTImage();
    
    void allocate(int w, int h);
    void computeSIFTImage(unsigned char *pixels, int w, int h);
    
    void toggleMode()
    {
        if (mode == MODE_TORRALBA) {
            mode = MODE_OPENCV;
        }
        else if( mode == MODE_OPENCV) {
            mode = MODE_OPENCV_SURF;
        }
        else if( mode == MODE_OPENCV_SURF) {
            mode = MODE_TORRALBA;
        }
        
    }
    
    string getMode()
    {
        if (mode == MODE_OPENCV) {
            return string("OpenCV based SIFT");
        }
        else if(mode == MODE_TORRALBA) {
            return string("Torralba based SIFT");
        }
        else if(mode == MODE_OPENCV_SURF) {
            return string("OpenCV based SURF");
        }
    }
    
    unsigned char * getSIFTImageForDrawing();
    
protected:
    
    // SIFT Parameters
    int cellSize, stepSize, alpha, siftDim;
    
    // Image Dimensions
    int width, height;
    
    // OpenCV based SIFT
    cv::Ptr<SiftDescriptorExtractor> descriptorExtractor;
    cv::Ptr<SurfDescriptorExtractor> descriptorExtractorSurf;
    vector<KeyPoint> imageKeypoints;
    Mat descriptors;
    
    // Methods for computing the Dense SIFT Image
    void computeSIFTImageTorralba(unsigned char *pixels, int w, int h);
    void computeSIFTImageOpenCV(unsigned char *pixels, int w, int h);
    
    // Image containers
    Image<unsigned char> grayImg, siftImg;
    
    // For compressing the SIFT image (for drawing only)
    bool computeCompressedSIFTImage();
    
    Mat pcaset, pcaset32, A;   
    Mat pcaSIFT, projected, siftColor;
    unsigned char *compressedSiftImg;
    
    SIFTMODE mode;
    
    bool bComputedSIFT, bAllocatedCompressedSIFT, bComputedCompressedSIFT;
    

};