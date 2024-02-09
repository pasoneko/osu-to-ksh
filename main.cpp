#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <zip.h>

using namespace std;
namespace fs = filesystem;

void unzip(const char* zipFileName, const char* outputFolder) {
    int error;
    zip* zipFile = zip_open(zipFileName, 0, &error);
    if (zipFile == nullptr) {
        cerr << "Error opening the zip file: " << zip_streeror(zipFile) << endl;
    }
}

int main() {
    string curPath = __FILE__;
    string path = curPath.substr(0, curPath.find_last_of("\\/")) + "\\Input";

    // Iterate through Input folder
    for (const auto& entry : fs::directory_iterator(path)) {
        // Check if .osz
        if (entry.path().string().find(".osz") != string::npos) {
            // Full path of .osz file
            string inPath = entry.path().string();
            // i.e. Kisaragi.osz versus C:\Meow\Bark\Woof\Kisaragi.osz
            string shortname = inPath.substr(inPath.find_last_of("\\") + 1);
            // Path to Output folder using name of osz
            string outPath = curPath.substr(0, curPath.find_last_of("\\/")) + "\\Output" + "\\" + shortname.replace(shortname.find_last_of("."), 4, ".zip");
            
            fs::copy(inPath, outPath);
            
            const char* zipFileName = outPath.substr(outPath.find_last_of("\\") + 1);
            un
        }
    }

    return 0;
}