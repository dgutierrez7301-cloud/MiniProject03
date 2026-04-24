# Testing Guide

## How to Run All Tests

./denoise tests/test1_small.txt tests/out_test1.txt mean 1
./denoise tests/test2_impulse.txt tests/out_test2.txt median 1
./denoise tests/test3_border.txt tests/out_test3.txt mean 2

## Test 1: Small Manual-Verify Matrix

File: tests/test1_small.txt
Filter: mean, 1 iteration
A 4x5 grid with four bright spike pixels (200) on a flat background (10). Small enough to verify every output pixel by hand.

Expected: The pixel at row 1, col 1 (value 200) has eight neighbors of value 10. Mean = (8x10 + 200) / 9 = 31. So that pixel becomes 31.

## Test 2: Salt-and-Pepper Impulse Noise

File: tests/test2_impulse.txt
Filter: median, 1 iteration
A 6x6 grid with uniform background of 100 plus five impulse noise pixels (0 or 255).

Expected: Median filter completely eliminates noise in one pass. All pixels become 100, stddev drops to 0.

## Test 3: Border/Edge Behavior

File: tests/test3_border.txt
Filter: mean, 2 iterations
A 3x3 checkerboard with extreme values (0 and 255). Every pixel is on a border or corner, which stresses the clamp border policy.

Expected: After 2 iterations, all values converge to 127 (the global average).