
#ifndef WATERSHEDALGORITHM_H
#define WATERSHEDALGORITHM_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>
#include <queue>
#include "WatershedStructure.h"

class WatershedAlgorithm {
    static const int HMIN = 0;
    static const int HMAX = 256;

public:
    IplImage* run(IplImage* pSrc) {
        std::string inTmp;

        IplImage* pGray = cvCreateImage(cvGetSize(pSrc), IPL_DEPTH_8U, 1);
        if (pSrc->nChannels == 3) {
            cvCvtColor(pSrc, pGray, CV_BGR2GRAY);
        }
        else if (pSrc->nChannels == 1)
            cvCopy(pSrc, pGray);

        IplImage* pBW = cvCreateImage(cvGetSize(pGray), IPL_DEPTH_8U, 1);
        cvAdaptiveThreshold(pGray, pBW, 255, 0, 0, 31);

        char* pixels = pBW->imageData;
        int width = pBW->width;
        int height = pBW->height;

        WatershedStructure  watershedStructure(pixels, width, height);

        std::queue<WatershedPixel*> pque;
        int curlab = 0;
        int heightIndex1 = 0;
        int heightIndex2 = 0;

        for (int h = HMIN; h < HMAX; ++h) {

            for (int pixelIndex = heightIndex1 ; pixelIndex < watershedStructure.size() ; ++pixelIndex) {
                WatershedPixel* p = watershedStructure.at(pixelIndex);

                if (p->getIntHeight() != h) { heightIndex1 = pixelIndex; break; }

                p->setLabelToMASK();

                std::vector<WatershedPixel*> neighbours = p->getNeighbours();
                for (unsigned i = 0 ; i < neighbours.size() ; ++i) {
                    WatershedPixel* q =  neighbours.at(i);

                    if (q->getLabel() >= 0) { p->setDistance(1); pque.push(p); break; }
                }
            }

            int curdist = 1;
            pque.push(new WatershedPixel());

            while (true) {
                WatershedPixel* p = pque.front(); pque.pop();

                if (p->isFICTITIOUS()) {
                    if (pque.empty()) { delete p; p = NULL; break; }
                    else {
                        pque.push(new WatershedPixel());
                        curdist++;
                        delete p; p = pque.front(); pque.pop();
                    }
                }

                std::vector<WatershedPixel*> neighbours = p->getNeighbours();
                for (unsigned i = 0 ; i < neighbours.size() ; ++i) {
                    WatershedPixel* q =  neighbours.at(i);

                    if ( (q->getDistance() <= curdist) &&  (q->getLabel() >= 0) ) {

                        if (q->getLabel() > 0) {
                            if ( p->isLabelMASK() )
                                p->setLabel(q->getLabel());
                            else if (p->getLabel() != q->getLabel())
                                p->setLabelToWSHED();
                        } else if (p->isLabelMASK())
                            p->setLabelToWSHED();
                    } else if ( q->isLabelMASK() && (q->getDistance() == 0) ) {
                        q->setDistance( curdist + 1 );
                        pque.push(q);
                    }
                }
            }

            for (int pixelIndex = heightIndex2 ; pixelIndex < watershedStructure.size() ; pixelIndex++) {
                WatershedPixel* p = watershedStructure.at(pixelIndex);

                if (p->getIntHeight() != h) { heightIndex2 = pixelIndex; break; }

                p->setDistance(0);

                if (p->isLabelMASK()) {
                    curlab++;
                    p->setLabel(curlab);
                    pque.push(p);

                    while (!pque.empty()) {
                        WatershedPixel* q = pque.front();
                        pque.pop();

                        std::vector<WatershedPixel*> neighbours = q->getNeighbours();

                        for (unsigned i = 0 ; i < neighbours.size() ; i++) {
                            WatershedPixel* r =  neighbours.at(i);

                            if ( r->isLabelMASK() ) { r->setLabel(curlab); pque.push(r); }
                        }
                    }
                }
            }
        }

        IplImage* pWS = cvCreateImage(cvGetSize(pBW), IPL_DEPTH_8U, 1);
        cvZero(pBW);
        char* wsPixels = pWS->imageData;
        char* grayPixels = pGray->imageData;

        for (int pixelIndex = 0 ; pixelIndex < watershedStructure.size() ; pixelIndex++) {
            WatershedPixel* p = watershedStructure.at(pixelIndex);

            if (p->isLabelWSHED() && !p->allNeighboursAreWSHED()) {
                wsPixels[p->getX() + p->getY()*width] = (char)255;
                grayPixels[p->getX() + p->getY()*width] = (char)255;
            }
        }

        return pGray;
    }
};

#endif
