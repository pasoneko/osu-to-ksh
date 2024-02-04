#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <zip.h>

using namespace std;
namespace fs = filesystem;

/*class Header {
    public:
        string title;
        string artist;
        string effect;
        string jacket;
        string illustrator = "";
        string difficulty;
        int level = 1;
        string t;
        string m;
        int mvol = 75;
        int o;
        string bg = "cyber";
        string layer = "arrow";
        int po;
        int plength = 15000;
        int pfiltergain = 50;
        string filtertype = "peak";
        int chokkakuautovol = 0;
        int chokkakuvol = 50;
        string ver = "";

        Header() {

        }

        Header(string title, string artist, string effect, string jacket, string difficulty, string t, string m, int o, int po) {
            this->title = title;
            this->artist = artist;
            this->effect = effect;
            this->jacket = jacket;
            this->difficulty = difficulty;
            this->t = t;
            this->m = m;
            this->o = o;
            this->po = po;
        }
};


void parseOsu() {
    
    ifstream osu(".osu");

    bool hitObjSection = false;
    string s;

    while (getline(osu, s)) {

    }
}

void createKsh() {
    ofstream ksh(".ksh");

}*/

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