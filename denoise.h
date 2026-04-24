#ifndef DENOISE_H
#define DENOISE_H

#include <vector>
#include <string>

using namespace std;

typedef vector<vector<int>> Image;

Image loadImage(string filename);
void saveImage(string filename, Image img);
Image applyMeanFilter(Image img);
Image applyMedianFilter(Image img);
void printStats(string label, Image img);

#endif