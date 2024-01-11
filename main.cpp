#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

string maptitle;
string kshtitle;
vector<Header> header;
//vector<HitObject> hitObjects;

class Header {
    public:
        string title;
        string artist;
        string effect;
        string jacket;
        string difficulty;
        string t;
        string m;
        int o;
        int po;
        int plength;

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
            this->plength = 15000;
        }
};


void parseOsu() {
    ifstream osu(maptitle + ".osu");

    bool hitObjSection = false;
    string s;

    
}

void createKsh() {
    ofstream ksh(kshtitle + ".ksh");

}

int main() {
    
}