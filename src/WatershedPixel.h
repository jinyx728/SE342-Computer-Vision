#ifndef WATERSHEDPIXEL_H
#define WATERSHEDPIXEL_H

#include <vector>

class WatershedPixel {
    static const int INIT = -1;
    static const int MASK = -2;
    static const int WSHED = 0;
    static const int FICTITIOUS = -3;

    int x;
    int y;
    char height;
    int label;
    int dist;

    std::vector<WatershedPixel*> neighbours;

public:
    WatershedPixel(int x, int y, char height) {
		this->x = x;
		this->y = y;
		this->height = height;
		label = INIT;
		dist = 0;
        neighbours.reserve(8);
	}
    WatershedPixel() { label = FICTITIOUS; }

    void addNeighbour(WatershedPixel* neighbour) {
		neighbours.push_back(neighbour);
	}
    std::vector<WatershedPixel*>& getNeighbours() {
		return neighbours; 
	}

	char getHeight() const { return height; } 
	int getIntHeight() const { return (int) height&0xff; } 
	int getX() const {	return x; }
    int getY() const {	return y; }

    void setLabel(int label) { this->label = label; }
    void setLabelToINIT() { label = INIT; }
    void setLabelToMASK() { label = MASK; }
    void setLabelToWSHED() { label = WSHED; }
    int getLabel() {   return label; }

    bool isLabelINIT() { return label == INIT; }
    bool isLabelMASK() { return label == MASK; }
    bool isLabelWSHED() { return label == WSHED; }

    void setDistance(int distance) { dist = distance; }
    int getDistance() { return dist; }

    bool isFICTITIOUS() { return label == FICTITIOUS; }

    bool allNeighboursAreWSHED() {
        for (unsigned i = 0 ; i < neighbours.size() ; i++) {
            WatershedPixel* r =  neighbours.at(i);
            if ( !r->isLabelWSHED() ) return false;
        }
        return true;
    }
};

#endif
