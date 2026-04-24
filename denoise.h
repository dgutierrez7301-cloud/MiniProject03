#ifndef DENOISE_H
#define DENOISE_H

#include <vector>
#include <string>

using Image = std::vector<std::vector<int>>;

// ── I/O ──────────────────────────────────────────────
Image loadImage(const std::string& filename);
void  saveImage(const std::string& filename, const Image& img);

// ── Filters ──────────────────────────────────────────
// Border policy: CLAMP (replicate edge pixels)
Image applyMeanFilter(const Image& img);
Image applyMedianFilter(const Image& img);

// Run the chosen filter for k iterations
Image denoise(const Image& img, const std::string& filter, int iterations);

// ── Helpers ──────────────────────────────────────────
int  clampValue(int v);                       // clamp to [0,255]
int  getPixelClamped(const Image& img, int r, int c); // safe access
void printStats(const std::string& label, const Image& img);

// ── Noise generator (for testing) ────────────────────
Image generateNoisyImage(int rows, int cols, int baseVal, int noiseCount, int seed);

#endif
