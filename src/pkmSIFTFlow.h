/*
 *  pkmSIFTFlow.h
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
#include "BPFlow.h"

using namespace cv;

class pkmSIFTFlow
{
public:  

    pkmSIFTFlow();
    
    void allocate(int width, int height);
    
    void computeFlow(Mat image1, Mat image2, int nchannels);
    
    int             sift1_width, sift1_height;
    int             sift2_width, sift2_height;
    
    Mat             sift1_level1, sift2_level1;
    Mat             sift1_level2, sift2_level2;
    Mat             sift1_level3, sift2_level3;
    Mat             sift1_level4, sift2_level4;

    Mat             vx_level1, vy_level1;
    Mat             vx_level2, vy_level2;
    Mat             vx_level3, vy_level3;
    Mat             vx_level4, vy_level4;
    
    Mat             winSizeX_level1, winSizeY_level1;
    Mat             winSizeX_level2, winSizeY_level2;
    Mat             winSizeX_level3, winSizeY_level3;
    Mat             winSizeX_level4, winSizeY_level4;
    
    Mat             xx, yy;
    int             width, height;
    
    BPFlow          bpflow;
    double          alpha, d, gamma, *pEnergyList;
    int             nlevels, wsize, topwsize, nTopIterations, nIterations;
    
    bool            bAllocated;
};