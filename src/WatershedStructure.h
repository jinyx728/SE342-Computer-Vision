#ifndef WATERSHEDSTRUCTURE_H
#define WATERSHEDSTRUCTURE_H

#include <algorithm>
#include "WatershedPixel.h"

class WatershedStructure {
    std::vector<WatershedPixel*> watershedStructure;

public:
    WatershedStructure(char* pixels, int width, int height) {

        watershedStructure.reserve(width * height);

        for (int y = 0; y < height; ++y)
            for (int x = 0; x < width; ++x)
                watershedStructure.push_back(new WatershedPixel(x, y, pixels[x+y*width]));


        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int currentindex = x + y*width;
                WatershedPixel* currentPixel = watershedStructure.at(currentindex);

				if (x - 1 >= 0) {
                    currentPixel->addNeighbour(watershedStructure.at(currentindex-1));
                    if (y - 1 >= 0)
                        currentPixel->addNeighbour(watershedStructure.at(currentindex-1 - width));
                    if (y + 1 < height)
                        currentPixel->addNeighbour(watershedStructure.at(currentindex-1 + width));
                }

                if (x + 1 < width) {
                    currentPixel->addNeighbour(watershedStructure.at(currentindex+1));
                    if (y - 1 >= 0)
                        currentPixel->addNeighbour(watershedStructure.at(currentindex+1 - width));
                    if (y + 1 < height)
                        currentPixel->addNeighbour(watershedStructure.at(currentindex+1 + width));
                }

                if (y - 1 >= 0)
                    currentPixel->addNeighbour(watershedStructure.at(currentindex-width));

                if (y + 1 < height)
                    currentPixel->addNeighbour(watershedStructure.at(currentindex+width));
            }
        }

        std::sort(watershedStructure.begin(), watershedStructure.end(),
			[](WatershedPixel * pl, WatershedPixel * pr) { return pl->getIntHeight() < pr->getIntHeight(); });
    }

    ~WatershedStructure() {
        while (!watershedStructure.empty()) {
            WatershedPixel* p = watershedStructure.back();
            delete p; p = NULL;
            watershedStructure.pop_back();
        }
    }

    int size() { return watershedStructure.size(); }

    WatershedPixel* at(int i) { return watershedStructure.at(i); }
};

#endif
