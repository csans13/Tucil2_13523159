#define STB_IMAGE_IMPLEMENTATION
#include "header/stb_image.h" 
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "header/stb_image_write.h" 
#include "header/utils.hpp"
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <cctype>

using namespace std;

const unordered_map<string, ErrorMethod> errorMethodMap = {
    {"variance", Variance},
    {"mad", MAD},
    {"mean absolute deviation", MAD},
    {"max pixel difference", MaxPixelDiff},
    {"mpd", MaxPixelDiff},
    {"entropy", Entropy},
};

bool fileExists(const string& filename)
{
    ifstream file(filename);
    return file.good();
}

bool hasValidExtension(const string& filename) {
    static const vector<string> validExtensions = {".jpg", ".jpeg", ".png", ".bmp"};
    auto pos = filename.find_last_of('.');
    if (pos == string::npos) return false;

    string ext = filename.substr(pos);
    transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    return find(validExtensions.begin(), validExtensions.end(), ext) != validExtensions.end();
}

bool isValidErrorMethod(const string& errorMethodStr)
{
    return errorMethodMap.find(errorMethodStr) != errorMethodMap.end();
}

ErrorMethod parseErrorMethod(const string& errorMethodStr)
{
    auto it = errorMethodMap.find(errorMethodStr);
    if (it != errorMethodMap.end())
    {
        return it->second;
    }
    else
    {
        cerr << "Metode error tidak dikenali: " << errorMethodStr << std::endl;
        exit(1);
    }
}

bool isValidThreshold(ErrorMethod method, float threshold)
{
    if (method == Variance)
    {
        return threshold > 0 && threshold <= 65025.0f;
    }
    else if (method == MAD)
    {
        return threshold > 0 && threshold <= 255.0f;
    }
    else if (method == MaxPixelDiff)
    {
        return threshold >= 0 && threshold <= 255.0f;
    }
    else if (method == Entropy)
    {
        return threshold >= 0 && threshold <= 8.0f;
    }
    else
    {
        cerr << "Threshold is not in valid range." << endl;
        return false;
    }
}

string trim(const string& s)
{
    size_t start = s.find_first_not_of(" \t\n\r");
    size_t end = s.find_last_not_of(" \t\n\r");
    return (start == string::npos) ? "" : s.substr(start, end - start + 1);
}

string getNonEmptyLine(const string& prompt)
{
    string input;
    do {
        cout << prompt;
        getline(cin, input);
        input = trim(input);
    } while (input.empty());
    return input;
}

bool processImage(const string& imagePath, vector<vector<RGB>>& image)
{
    int width, height, channels;
    unsigned char* data = stbi_load(imagePath.c_str(), &width, &height, &channels, 3); // force 3 channels (RGB)
    
    if (!data) {
        cerr << "Gagal memuat gambar: " << imagePath << endl;
        return false;
    }

    image.resize(height, vector<RGB>(width));

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = (y * width + x) * 3;
            image[y][x] = {
                data[index],     // R
                data[index + 1], // G
                data[index + 2]  // B
            };
        }
    }

    stbi_image_free(data);
    return true;
}

long long getFileSize(const string& path)
{
    ifstream file(path, ios::binary | ios::ate);
    if (!file) return -1;
    return static_cast<long long>(file.tellg());
}

void inputHandler(string& inputImagePath, vector<vector<RGB>>& image,
                  string& errorMethodStr, ErrorMethod& method, float& threshold,
                  int& minBlockSize, string& outputImagePath)
{
    // Input path
    cout << "Masukkan path gambar input (dengan ekstensi)\n";
    cout << "Ekstensi valid: .png, .jpg, .jpeg, .bmp\n\n";

    while (true) {
        inputImagePath = getNonEmptyLine("Path gambar input: ");
        if (!fileExists(inputImagePath))
        {
            cout << "File tidak ditemukan.\n\n";
        }
        else if (!hasValidExtension(inputImagePath))
        {
            cout << "Ekstensi file tidak valid. Gunakan salah satu dari: .png, .jpg, .jpeg, .bmp\n\n";
        }
        else
        {
            break;
        }
    }
    cout << endl;

    // Output path
    cout << "Masukkan path gambar output (dengan ekstensi)\n";
    cout << "Ekstensi valid: .png, .jpg, .jpeg, .bmp\n";
    cout << "Path output tidak boleh sama dengan path input\n\n";

    while (true) {
        outputImagePath = getNonEmptyLine("Path gambar output: ");

        if (!hasValidExtension(outputImagePath))
        {
            cout << "\nEkstensi tidak valid. Gunakan salah satu dari: .png, .jpg, .jpeg, .bmp\n\n";
            continue;
        }

        if (outputImagePath == inputImagePath)
        {
            cout << "\nPath output tidak boleh sama dengan path input.\n\n";
            continue;
        }

        if (fileExists(outputImagePath))
        {
            cout << "\nFile sudah ada. Timpa? (y/n): ";
            string response;
            getline(cin, response);
            if (!response.empty() && tolower(response[0]) == 'y') break;
        }
        else
        {
            break;
        }
    }

    cout << endl;

    // Metode error
    cout << "Pilih metode perhitungan error\n";
    cout << "(Variance, Mean Absolute Deviation (MAD), Max Pixel Difference, Entropy)\n\n";

    while (true)
    {
        errorMethodStr = getNonEmptyLine("Metode error: ");
        transform(errorMethodStr.begin(), errorMethodStr.end(), errorMethodStr.begin(), ::tolower);
        if (isValidErrorMethod(errorMethodStr))
        {
            method = parseErrorMethod(errorMethodStr);
            break;
        }
        cout << "\nMetode tidak dikenali. Coba lagi.\n";
    }

    cout << endl;

    // Threshold
    while (true)
    {
        cout << "Masukkan nilai threshold: ";
        string line;
        getline(cin, line);
        stringstream ss(line);
        if (ss >> threshold && isValidThreshold(method, threshold)) break;
        cout << "\nInput tidak valid. Masukkan angka desimal yang sesuai untuk threshold.\n\n";
    }

    cout << endl;

    // Ukuran blok minimum
    while (true)
    {
        cout << "Masukkan ukuran blok minimum: ";
        string line;
        getline(cin, line);
        stringstream ss(line);
        if (ss >> minBlockSize && minBlockSize > 1) break;
        cout << "\nInput tidak valid. Masukkan bilangan bulat > 1.\n\n";
    }

    cout << endl;

    // Baca dan proses gambar
    if (!processImage(inputImagePath, image))
    {
        cerr << "Gagal memproses gambar. Program dihentikan.\n";
        exit(EXIT_FAILURE);
    }
}

void saveCompressedImage(const vector<vector<RGB>>& image, const string& outputImagePath)
{
    if (image.empty() || image[0].empty()) {
        std::cerr << "Galat: Data gambar kosong. Tidak dapat menyimpan.\n";
        return;
    }

    const int height = static_cast<int>(image.size());
    const int width = static_cast<int>(image[0].size());
    const int channels = 3; // RGB

    // Siapkan buffer datar untuk menyimpan piksel dalam format RGB
    std::vector<uint8_t> data(height * width * channels);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const RGB& pixel = image[y][x];
            int idx = (y * width + x) * channels;
            data[idx + 0] = static_cast<uint8_t>(pixel.r);
            data[idx + 1] = static_cast<uint8_t>(pixel.g);
            data[idx + 2] = static_cast<uint8_t>(pixel.b);
        }
    }

    // Simpan gambar ke file PNG
    if (!stbi_write_png(outputImagePath.c_str(), width, height, channels, data.data(), width * channels)) {
        std::cerr << "Gagal menyimpan gambar ke: " << outputImagePath << '\n';
    } else {
        std::cout << "Gambar berhasil disimpan ke: " << outputImagePath << '\n';
    }
}

void outputHandler(const string& outputImagePath, const string& inputImagePath,
                   int maxDepth, int nodeCount, chrono::milliseconds duration)
{
    long long inputSizeKB = getFileSize(inputImagePath) / 1024;
    long long outputSizeKB = getFileSize(outputImagePath) / 1024;

    cout << "\n\n========================= HASIL KOMPRESI =========================\n\n";
    cout << "Gambar keluaran berhasil disimpan ke : " << outputImagePath << '\n';
    cout << "Durasi eksekusi kompresi             : " << duration.count() << " ms\n";
    cout << "Ukuran gambar input                  : " << inputSizeKB << " KB\n";
    cout << "Ukuran gambar output                 : " << outputSizeKB << " KB\n";
    cout << "Rasio kompresi                       : " << (1.0 - (float(outputSizeKB) / float(inputSizeKB))) * 100 << "% reduction\n";
    cout << "Kedalaman maksimum Quadtree          : " << maxDepth << '\n';
    cout << "Jumlah total simpul Quadtree         : " << nodeCount << "\n\n";
    cout << "=================================================================\n";
}
