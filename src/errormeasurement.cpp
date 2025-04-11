#include "header/errormeasurement.hpp"

RGB ErrorMeasurement::computeAvgColor(const vector<vector<RGB>>& image, int x, int y, int width, int height)
{
    long long sumR = 0, sumG = 0, sumB = 0;
    int totalPixels = width * height;

    for (int i = y; i < y + height; ++i)
    {
        for (int j = x; j < x + width; ++j)
        {
            const RGB& pixel = image[i][j];
            sumR += pixel.r;
            sumG += pixel.g;
            sumB += pixel.b;
        }
    }
    
    return RGB{
        static_cast<int>(sumR/totalPixels),
        static_cast<int>(sumG/totalPixels),
        static_cast<int>(sumB/totalPixels)
    };
}

float ErrorMeasurement::computeVariance(const vector<vector<RGB>>& image, int x, int y, int width, int height)
{
    float varR = 0.0f, varG = 0.0f, varB = 0.0f;
    int totalPixels = width * height;
    RGB mean = computeAvgColor(image, x, y, width, height);

    for (int i = y; i < y + height; ++i)
    {
        for (int j = x; j < x + width; ++j)
        {
            const RGB& pixel = image[i][j];
            varR += pow(pixel.r - mean.r, 2);
            varG += pow(pixel.g - mean.g, 2);
            varB += pow(pixel.b - mean.b, 2);
        }
    }

    varR /= totalPixels;
    varG /= totalPixels;
    varB /= totalPixels;

    return (varR + varG + varB) / 3;
}

float ErrorMeasurement::computeMAD(const vector<vector<RGB>>& image, int x, int y, int width, int height)
{
    float madR = 0.0f, madG = 0.0f, madB = 0.0f;
    int totalPixels = width * height;
    RGB mean = computeAvgColor(image, x, y, width, height);

    for (int i = y; i < y + height; ++i)
    {
        for (int j = x; j < x + width; ++j)
        {
            const RGB& pixel = image[i][j];
            madR += abs(pixel.r - mean.r);
            madG += abs(pixel.g - mean.g);
            madB += abs(pixel.b - mean.b);
        }
    }

    madR /= totalPixels;
    madG /= totalPixels;
    madB /= totalPixels;

    return (madR + madG + madB) / 3;
}

float ErrorMeasurement::computeMaxPixelDiff(const vector<vector<RGB>>& image, int x, int y, int width, int height)
{
    int minR = 255, minG = 255, minB = 255;
    int maxR = 0, maxG = 0, maxB = 0;
    int totalPixels = width * height;
    RGB mean = computeAvgColor(image, x, y, width, height);
    
    for (int i = y; i < y + height; ++i)
    {
        for (int j = x; j < x + width; ++j)
        {
            const RGB& pixel = image[i][j];
            minR = min(minR, image[i][j].r); minG = min(minG, image[i][j].g); minB = min(minB, image[i][j].b);
            maxR = max(maxR, image[i][j].r); maxG = max(maxG, image[i][j].g); maxB = max(maxB, image[i][j].b);
        }
    }
    
    float D_R = maxR - minR;
    float D_G = maxG - minG;
    float D_B = maxB - minB;
    
    return (D_R + D_G + D_B) / 3.0f;
} 

float entropy(const vector<vector<RGB>>& image, int x, int y, int width, int height)
{
    const int CHANNEL_RANGE = 256;
    array<int, CHANNEL_RANGE> histR{}, histG{}, histB{};
    int totalPixels = width * height;
    if (totalPixels <= 0)
    {
        return 0.0f;
    }

    for (int i = y; i < y + height && i < image.size(); ++i) {
        for (int j = x; j < x + width && j < image[j].size(); ++j) {
            const RGB& pixel = image[i][j];
            ++histR[pixel.r];
            ++histG[pixel.g];
            ++histB[pixel.b];
        }
    }

    auto calcEntropy = [totalPixels](const array<int, CHANNEL_RANGE>& hist) -> float
    {
        float entropy = 0.0f;
        for (int freq : hist)
        {
            if (freq > 0)
            {
                float p = static_cast<float>(freq) / totalPixels;
                entropy -= p * log2f(p);
            }
        }
        return entropy;
    };

    return (calcEntropy(histR) + calcEntropy(histG) + calcEntropy(histB)) / 3.0f;
}
