/*
 *  pkmOpenCVUtils.cpp
 *  pkmSIFTImage
 *
 *  Created by Mr. Magoo on 9/24/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "pkmOpenCVUtils.h"

Mat meshgridX(int x1, int x2, int y1, int y2, int dx = 1, int dy = 1)
{
    int i,j,height,width;
    
    width = 1 + (int) floor((x2-x1)/dx);
    height  = 1 + (int) floor((y2-y1)/dy);
    
    
    Mat grid(height, width, CV_32FC1); 
    
    for(i=0; i<height; i++)
        for(j=0; j<width; j++)
            grid.at<float>(i,j) = x1 + j*dx;
    
    return grid;
}
Mat meshgridY(int x1, int x2, int y1, int y2, int dx = 1, int dy = 1)
{
    int i,j,height,width;
    
    width = 1 + (int) floor((x2-x1)/dx);
    height  = 1 + (int) floor((y2-y1)/dy);
    
    Mat grid(height, width, CV_32FC1); 
    
    for(j=0; j<width; j++)
        for(i=0; i<height; i++)
            grid.at<float>(i,j) = y1 + i*dy;
    
}