# Mini Project 3: Image Denoiser

A C++ program that removes noise from a grayscale image stored as a 2D matrix of integers (0-255).

## How to Build

g++ -std=c++17 -Wall -o denoise MAIN.cpp denoise.cpp

## How to Run

./denoise <input.txt> <output.txt> <filter> [iterations]

- filter: "mean" or "median"
- iterations: optional, default is 1

Examples:
./denoise tests/test1_small.txt output.txt mean 1
./denoise tests/test2_impulse.txt clean.txt median 2

## Input/Output Format

First line is rows and cols, then pixel values:

4 5
10 10 10 10 10
10 200 10 200 10
10 10 10 10 10
10 200 10 200 10

## Design Notes

- Image is stored as vector<vector<int>>
- Mean filter: replaces each pixel with the average of its 3x3 neighborhood
- Median filter: replaces each pixel with the middle value of its 3x3 neighborhood (sorted with bubble sort)
- Border policy: Clamp — when the 3x3 window goes off the edge, the nearest edge pixel is reused
- Values are clamped to [0, 255] after filtering

## Demo Output

./denoise tests/test2_impulse.txt clean.txt median 1
Loading image from: tests/test2_impulse.txt
[BEFORE] 36 pixels | min=0 max=255 mean=110 stddev=52
Applying median filter (1 iteration)...
[AFTER ] 36 pixels | min=100 max=100 mean=100 stddev=0
Saved denoised image to: clean.txt

## Copilot Usage

### What Copilot helped with
- File I/O (loadImage and saveImage functions)
- The before/after statistics and standard deviation calculation
- Autocompleting the nested for loops for the 3x3 neighborhood scan

### What I wrote/verified myself
- Program structure and the header file
- The clamp border policy 
- Command-line argument parsing in main
- Bubble sort for the median filter
- All three test cases and manually checking the output

### Bug Copilot introduced
Copilot suggested computing the mean filter without clamping the result to [0, 255]. After multiple iterations this could produce out-of-range values. I added the clamp() function call after every filter computation to fix it.