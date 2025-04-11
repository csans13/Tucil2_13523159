#include <iostream>
#include <vector>
#include <chrono>
#include "header/utils.hpp"
#include "header/quadTree.hpp"

using namespace std;

int main() {
    string inputImagePath, errorMethodStr, outputImagePath;
    ErrorMethod method;
    vector<vector<RGB>> image;
    float threshold = 0.0f;
    int minBlockSize = 2, maxDepth = 0, nodeCount = 0;

    // Handle user input and load the image
    inputHandler(inputImagePath, image, errorMethodStr, method, threshold, minBlockSize, outputImagePath);

    // Start timing the compression process
    auto start = chrono::high_resolution_clock::now();

    QuadTree qt;
    qt.buildTree(image, 0, 0, image[0].size(), image.size(), method, threshold, minBlockSize);
    maxDepth = qt.getMaxDepth();
    nodeCount = qt.getNodeCount();

    qt.reconstructImage(image);
    saveCompressedImage(image, outputImagePath);

    // End timing
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    // Display output summary
    outputHandler(outputImagePath, inputImagePath, maxDepth, nodeCount, duration);

    return 0;
}
