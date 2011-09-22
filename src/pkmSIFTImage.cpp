/*
 *  pkmSIFTImage.cpp
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

#include "pkmSIFTImage.h"
#include "PCAColorModel.h"

pkmSIFTImage::pkmSIFTImage()
{
    mode = MODE_OPENCV;
    
    stepSize = 1;
    cellSize = 3;
    alpha = 9;
    siftDim = 128;
    compressedSiftImg = NULL;
    
    bComputedSIFT = false;
    bComputedCompressedSIFT = false;
    bAllocatedCompressedSIFT = false;
    
}

pkmSIFTImage::~pkmSIFTImage()
{
    if (bAllocatedCompressedSIFT) {
        delete compressedSiftImg;
        bAllocatedCompressedSIFT = false;
    }
}

void pkmSIFTImage::allocate(int w, int h)
{
    if(width != w || height != h)
    {
        printf("[pkmSIFTImage]: Reallocating...\n");
        
        if (mode == MODE_TORRALBA) {
            grayImg.allocate(w,h);
            if (bAllocatedCompressedSIFT) {
                delete compressedSiftImg;
                bAllocatedCompressedSIFT = false;
            }
        }
        else if (mode == MODE_OPENCV) {
            imageKeypoints.clear();
            for(int i = 0; i < h; i+=stepSize)
            {
                for(int j = 0; j < w; j+=stepSize)
                {
                    imageKeypoints.push_back(KeyPoint(Point2f(j,i), cellSize));
                }
            }
            
            descriptorExtractor = new SiftDescriptorExtractor( SIFT::DescriptorParams(2,        // magnification
                                                                                      true,     // normalize?
                                                                                      true),    // recalculate angles?
                                                              SIFT::CommonParams(3,             // number of octaves
                                                                                 1,             // number of octave layers
                                                                                 -1,            // first octave
                                                                                 1) );          // FIRST_ANGLE = 0, AVERAGE_ANGLE = 1
    
        }
    }
    
    
    width = w;
    height = h;
    
}

void pkmSIFTImage::computeSIFTImage(unsigned char *pixels, int w, int h)
{
    if (width != w || height != h) {
        allocate(w,h);
    }
    
    if (mode == MODE_TORRALBA) {
        computeSIFTImageTorralba(pixels, w, h);
    }
    else if (mode == MODE_OPENCV) {
        computeSIFTImageOpenCV(pixels, w, h);
    }
}

void pkmSIFTImage::computeSIFTImageTorralba(unsigned char *pixels, int w, int h)
{

#ifdef _UNSAFE
    memcpy(grayImg.pData, pixels, sizeof(unsigned char)*w*h);
    ImageFeature::imSIFT(grayImg, siftImg, cellSize, stepSize, true, 8);
#else
    std::swap(grayImg.pData, pixels);
    ImageFeature::imSIFT(grayImg, siftImg, cellSize, stepSize, true, 8);
    std::swap(grayImg.pData, pixels);
#endif
    
    siftDim = siftImg.nchannels();
    bComputedSIFT = true;
    bComputedCompressedSIFT = false;
}

void pkmSIFTImage::computeSIFTImageOpenCV(unsigned char *pixels, int w, int h)
{
    Mat img = Mat(h,w,CV_8UC1,pixels);
    descriptorExtractor->compute(img, imageKeypoints, descriptors);

    siftDim = descriptors.cols;
    bComputedSIFT = true;
    bComputedCompressedSIFT = false;
}

bool pkmSIFTImage::computeCompressedSIFTImage()
{
    if (!bComputedSIFT) 
    {
        printf("[ERROR: pkmSIFTImage]: Compute SIFT image first by calling computeSIFTImage(...)\n");
        return false;
    }
    if (!bAllocatedCompressedSIFT) 
    {
        compressedSiftImg = new unsigned char[width*height*3];
        
        // Map the first PCA dimension to luminance (R+G+B), 
        // map the second to R-G and the third one to (R+G)/2-B
        // inv([1 1 1; 1 -1 0; .5 .5 -1]);
        float A_data[9] = { 0.3333,    0.5,  0.3333,
                            0.3333,   -0.5,  0.3333,
                            0.3333,    0  , -0.6667};
        A = Mat(3,3,CV_32FC1, A_data);
        Mat m = Mat(128, 100, CV_32FC1, pcSIFT);
        pcaSIFT = m.rowRange(0,siftDim).colRange(0, 3).t();
        
        pcaset = Mat(width*height, siftDim, CV_8UC1);
        pcaset32 = Mat(width*height, siftDim, CV_32FC1);
        projected = Mat(3, width*height, CV_32FC1);
        siftColor = Mat(3, width*height, CV_32FC1);
        
        bAllocatedCompressedSIFT = true;
    }
    if (!bComputedCompressedSIFT) 
    {
        if (mode == MODE_TORRALBA) {
            for(int i = 0; i < pcaset.rows; i++)
            {
                unsigned char* pca_ptr = pcaset.ptr<unsigned char>(i);
                const unsigned char* sift_ptr = siftImg.pData + i*siftDim;
                memcpy(pca_ptr, sift_ptr, sizeof(unsigned char) * siftDim);
            }
            
            pcaset.convertTo(pcaset32, CV_32FC1);
            projected = pcaSIFT * pcaset32.t();
        }
        else if(mode == MODE_OPENCV)
        {
            projected = pcaSIFT * descriptors.t();
        }

        // Map the first PCA dimension to luminance (R+G+B), 
        // map the second to R-G and the third one to (R+G)/2-B
        siftColor = A * projected;
        double minv, maxv;
        minMaxLoc(siftColor, &minv, &maxv);
        siftColor = (siftColor - minv) / (maxv - minv) * 255.0f;
        for(int i = 0; i < height; i++)
        {
            unsigned char* pix_ptr = compressedSiftImg + i*width*3;
            for (int j = 0; j < width; j++) {
                *pix_ptr++ = siftColor.at<float>(2,i*width + j);
                *pix_ptr++ = siftColor.at<float>(1,i*width + j);
                *pix_ptr++ = siftColor.at<float>(0,i*width + j);
            }
        }
        bComputedCompressedSIFT = true;
    }
    
    return bComputedCompressedSIFT;
}

unsigned char * pkmSIFTImage::getSIFTImageForDrawing()
{
    if (!computeCompressedSIFTImage())
    {
        return NULL;
    }
    return compressedSiftImg;
}