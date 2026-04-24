#include "denoise.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <cstdlib>
#include <cmath>
#include "denoise.h"

// ═══════════════════════════════════════════════════════
//  Helpers
// ═══════════════════════════════════════════════════════

int clampValue(int v) {
    if (v < 0)   return 0;
    if (v > 255) return 255;
    return v;
}

// Safe pixel access using CLAMP border policy:
// If the requested row/col is outside the image, we pretend
// it's the nearest edge pixel (like stretching the border).
int getPixelClamped(const Image& img, int r, int c) {
    int rows = static_cast<int>(img.size());
    int cols = static_cast<int>(img[0].size());
    r = std::max(0, std::min(r, rows - 1));
    c = std::max(0, std::min(c, cols - 1));
    return img[r][c];
}

// ═══════════════════════════════════════════════════════
//  I/O
// ═══════════════════════════════════════════════════════

Image loadImage(const std::string& filename) {
    std::ifstream fin(filename);
    if (!fin.is_open()) {
        throw std::runtime_error("Cannot open input file: " + filename);
    }

    int rows, cols;
    fin >> rows >> cols;
    if (rows <= 0 || cols <= 0) {
        throw std::runtime_error("Invalid dimensions in file");
    }

    Image img(rows, std::vector<int>(cols));
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (!(fin >> img[r][c])) {
                throw std::runtime_error("Not enough pixel data in file");
            }
            img[r][c] = clampValue(img[r][c]);  // sanitize on load
        }
    }
    return img;
}

void saveImage(const std::string& filename, const Image& img) {
    std::ofstream fout(filename);
    if (!fout.is_open()) {
        throw std::runtime_error("Cannot open output file: " + filename);
    }

    int rows = static_cast<int>(img.size());
    int cols = static_cast<int>(img[0].size());
    fout << rows << " " << cols << "\n";

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (c > 0) fout << " ";
            fout << img[r][c];
        }
        fout << "\n";
    }
}

// ═══════════════════════════════════════════════════════
//  Mean Filter (3×3)
// ═══════════════════════════════════════════════════════
//  For each pixel, replace it with the average of itself
//  and its 8 neighbors. Like asking "what's the consensus
//  value around here?" — smooths out lone outliers.

Image applyMeanFilter(const Image& img) {
    int rows = static_cast<int>(img.size());
    int cols = static_cast<int>(img[0].size());
    Image out(rows, std::vector<int>(cols));

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            int sum = 0;
            for (int dr = -1; dr <= 1; dr++) {
                for (int dc = -1; dc <= 1; dc++) {
                    sum += getPixelClamped(img, r + dr, c + dc);
                }
            }
            out[r][c] = clampValue(sum / 9);
        }
    }
    return out;
}

// ═══════════════════════════════════════════════════════
//  Median Filter (3×3)
// ═══════════════════════════════════════════════════════
//  For each pixel, collect all 9 neighbors, sort them,
//  and pick the middle value. This is great at killing
//  "salt and pepper" noise because outliers can't survive
//  a vote — like picking the median salary in a room;
//  one billionaire doesn't skew it.

Image applyMedianFilter(const Image& img) {
    int rows = static_cast<int>(img.size());
    int cols = static_cast<int>(img[0].size());
    Image out(rows, std::vector<int>(cols));

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            int neighbors[9];
            int idx = 0;
            for (int dr = -1; dr <= 1; dr++) {
                for (int dc = -1; dc <= 1; dc++) {
                    neighbors[idx++] = getPixelClamped(img, r + dr, c + dc);
                }
            }
            std::sort(neighbors, neighbors + 9);
            out[r][c] = neighbors[4];  // middle of 9 sorted values
        }
    }
    return out;
}

// ═══════════════════════════════════════════════════════
//  Denoise driver — applies chosen filter k times
// ═══════════════════════════════════════════════════════

Image denoise(const Image& img, const std::string& filter, int iterations) {
    Image current = img;
    for (int i = 0; i < iterations; i++) {
        if (filter == "mean") {
            current = applyMeanFilter(current);
        } else if (filter == "median") {
            current = applyMedianFilter(current);
        } else {
            throw std::runtime_error("Unknown filter: " + filter +
                                     " (use 'mean' or 'median')");
        }
    }
    return current;
}

// ═══════════════════════════════════════════════════════
//  Statistics
// ═══════════════════════════════════════════════════════

void printStats(const std::string& label, const Image& img) {
    int minVal = 255, maxVal = 0;
    long long sum = 0;
    int count = 0;

    for (const auto& row : img) {
        for (int v : row) {
            minVal = std::min(minVal, v);
            maxVal = std::max(maxVal, v);
            sum += v;
            count++;
        }
    }

    double mean = static_cast<double>(sum) / count;

    // Compute standard deviation
    double variance = 0;
    for (const auto& row : img) {
        for (int v : row) {
            double diff = v - mean;
            variance += diff * diff;
        }
    }
    variance /= count;
    double stddev = std::sqrt(variance);

    std::cout << "[" << label << "] "
              << count << " pixels | "
              << "min=" << minVal << " max=" << maxVal
              << " mean=" << static_cast<int>(mean)
              << " stddev=" << static_cast<int>(stddev)
              << "\n";
}

// ═══════════════════════════════════════════════════════
//  Noisy image generator (for testing)
// ═══════════════════════════════════════════════════════

Image generateNoisyImage(int rows, int cols, int baseVal,
                         int noiseCount, int seed) {
    std::srand(seed);
    Image img(rows, std::vector<int>(cols, baseVal));

    for (int i = 0; i < noiseCount; i++) {
        int r = std::rand() % rows;
        int c = std::rand() % cols;
        // salt-and-pepper: randomly 0 or 255
        img[r][c] = (std::rand() % 2 == 0) ? 0 : 255;
    }
    return img;
}
