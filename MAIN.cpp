#include "denoise.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <cmath>

void printUsage(const char* progName) {
    std::cout << "Usage:\n"
              << "  " << progName << " <input.txt> <output.txt> <filter> [iterations]\n\n"
              << "  filter:     'mean' or 'median'\n"
              << "  iterations: number of filter passes (default: 1)\n\n"
              << "Examples:\n"
              << "  " << progName << " noisy.txt clean.txt mean 2\n"
              << "  " << progName << " noisy.txt clean.txt median\n\n"
              << "Special:\n"
              << "  " << progName << " --generate <output.txt> <rows> <cols> <noise_count>\n"
              << "    Generates a noisy test image.\n";
}

int main(int argc, char* argv[]) {
    // ── Handle --generate mode ───────────────────────
    if (argc >= 2 && std::string(argv[1]) == "--generate") {
        if (argc < 6) {
            std::cerr << "Usage: " << argv[0]
                      << " --generate <out.txt> <rows> <cols> <noise_count>\n";
            return 1;
        }
        std::string outFile = argv[2];
        int rows   = std::atoi(argv[3]);
        int cols   = std::atoi(argv[4]);
        int noise  = std::atoi(argv[5]);

        Image noisy = generateNoisyImage(rows, cols, 128, noise, 42);
        saveImage(outFile, noisy);
        std::cout << "Generated " << rows << "x" << cols
                  << " image with " << noise << " noisy pixels -> "
                  << outFile << "\n";
        return 0;
    }

    // ── Normal denoise mode ──────────────────────────
    if (argc < 4) {
        printUsage(argv[0]);
        return 1;
    }

    std::string inputFile  = argv[1];
    std::string outputFile = argv[2];
    std::string filter     = argv[3];
    int iterations = 1;
    if (argc >= 5) {
        iterations = std::atoi(argv[4]);
        if (iterations < 1) {
            std::cerr << "Error: iterations must be >= 1\n";
            return 1;
        }
    }

    try {
        // Load
        std::cout << "Loading image from: " << inputFile << "\n";
        Image original = loadImage(inputFile);
        printStats("BEFORE", original);

        // Denoise
        std::cout << "Applying " << filter << " filter ("
                  << iterations << " iteration"
                  << (iterations > 1 ? "s" : "") << ")...\n";
        Image cleaned = denoise(original, filter, iterations);
        printStats("AFTER ", cleaned);

        // Save
        saveImage(outputFile, cleaned);
        std::cout << "Saved denoised image to: " << outputFile << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
