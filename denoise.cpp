#include "denoise.h"
#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

// keeps value between 0 and 255
int clamp(int v) {
    if (v < 0) return 0;
    if (v > 255) return 255;
    return v;
}

// gets a pixel safely - if out of bounds, uses nearest edge pixel
int getPixel(Image img, int r, int c) {
    int rows = img.size();
    int cols = img[0].size();
    if (r < 0) r = 0;
    if (r >= rows) r = rows - 1;
    if (c < 0) c = 0;
    if (c >= cols) c = cols - 1;
    return img[r][c];
}

Image loadImage(string filename) {
    ifstream fin(filename);
    if (!fin.is_open()) {
        cout << "Error: cannot open " << filename << endl;
        exit(1);
    }

    int rows, cols;
    fin >> rows >> cols;

    Image img(rows, vector<int>(cols));
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            fin >> img[r][c];
            img[r][c] = clamp(img[r][c]);
        }
    }
    fin.close();
    return img;
}

void saveImage(string filename, Image img) {
    ofstream fout(filename);
    int rows = img.size();
    int cols = img[0].size();
    fout << rows << " " << cols << endl;

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (c > 0) fout << " ";
            fout << img[r][c];
        }
        fout << endl;
    }
    fout.close();
}

// replaces each pixel with average of its 3x3 neighborhood
Image applyMeanFilter(Image img) {
    int rows = img.size();
    int cols = img[0].size();
    Image out(rows, vector<int>(cols));

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            int sum = 0;
            for (int dr = -1; dr <= 1; dr++) {
                for (int dc = -1; dc <= 1; dc++) {
                    sum += getPixel(img, r + dr, c + dc);
                }
            }
            out[r][c] = clamp(sum / 9);
        }
    }
    return out;
}

// replaces each pixel with median of its 3x3 neighborhood
Image applyMedianFilter(Image img) {
    int rows = img.size();
    int cols = img[0].size();
    Image out(rows, vector<int>(cols));

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            // collect 9 neighbors
            int neighbors[9];
            int idx = 0;
            for (int dr = -1; dr <= 1; dr++) {
                for (int dc = -1; dc <= 1; dc++) {
                    neighbors[idx] = getPixel(img, r + dr, c + dc);
                    idx++;
                }
            }
            // simple bubble sort
            for (int i = 0; i < 9; i++) {
                for (int j = i + 1; j < 9; j++) {
                    if (neighbors[j] < neighbors[i]) {
                        int temp = neighbors[i];
                        neighbors[i] = neighbors[j];
                        neighbors[j] = temp;
                    }
                }
            }
            out[r][c] = neighbors[4]; // middle value
        }
    }
    return out;
}

void printStats(string label, Image img) {
    int minVal = 255, maxVal = 0;
    long long sum = 0;
    int count = 0;

    for (int r = 0; r < (int)img.size(); r++) {
        for (int c = 0; c < (int)img[0].size(); c++) {
            if (img[r][c] < minVal) minVal = img[r][c];
            if (img[r][c] > maxVal) maxVal = img[r][c];
            sum += img[r][c];
            count++;
        }
    }

    double mean = (double)sum / count;
    double variance = 0;
    for (int r = 0; r < (int)img.size(); r++) {
        for (int c = 0; c < (int)img[0].size(); c++) {
            double diff = img[r][c] - mean;
            variance += diff * diff;
        }
    }
    variance /= count;

    cout << "[" << label << "] " << count << " pixels | "
         << "min=" << minVal << " max=" << maxVal
         << " mean=" << (int)mean
         << " stddev=" << (int)sqrt(variance) << endl;
}