#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>
#include <chrono>
#include "quadtree.hpp"

using namespace std;

bool fileExists(const string& filename);
bool hasValidExtension(const string& filename);
bool isValidErrorMethod(const string& errorMethodStr);
ErrorMethod parseErrorMethod(const string& errorMethodStr);
bool isValidThreshold(ErrorMethod method, float threshold);

string trim(const string& s);
string getNonEmptyLine(const string& prompt);

bool processImage(const string& imagePath, vector<vector<RGB>>& image);

long long getFileSize(const string& path);

void inputHandler(string& inputImagePath, vector<vector<RGB>>& image,
                  string& errorMethodStr, ErrorMethod& method, float& threshold,
                  int& minBlockSize, string& outputImagePath);

void saveCompressedImage(const vector<vector<RGB>>& image, const string& outputImagePath);

void outputHandler(const string &outputImagePath, const string &inputImagePath,
                   int maxDepth, int nodeCount, chrono::milliseconds duration);

#endif // UTILS_HPP
