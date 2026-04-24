#include "denoise.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 4) {
        cout << "Usage: ./denoise <input.txt> <output.txt> <filter> [iterations]" << endl;
        cout << "  filter: mean or median" << endl;
        cout << "  iterations: number of passes (default 1)" << endl;
        return 1;
    }

    string inputFile = argv[1];
    string outputFile = argv[2];
    string filter = argv[3];
    int iterations = 1;
    if (argc >= 5) {
        iterations = atoi(argv[4]);
    }

    // load the image
    cout << "Loading image from: " << inputFile << endl;
    Image original = loadImage(inputFile);
    printStats("BEFORE", original);

    // apply filter
    cout << "Applying " << filter << " filter (" << iterations << " iteration";
    if (iterations > 1) cout << "s";
    cout << ")..." << endl;

    Image result = original;
    for (int i = 0; i < iterations; i++) {
        if (filter == "mean") {
            result = applyMeanFilter(result);
        } else if (filter == "median") {
            result = applyMedianFilter(result);
        } else {
            cout << "Error: unknown filter. Use mean or median." << endl;
            return 1;
        }
    }

    printStats("AFTER ", result);

    // save result
    saveImage(outputFile, result);
    cout << "Saved denoised image to: " << outputFile << endl;

    return 0;
}
