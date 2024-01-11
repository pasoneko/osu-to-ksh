#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

/**
 * @brief Method for splitting a string at a string regex
 * 
 * @param str String to be split
 * @param regex String where str is to be split
 * @return vector<string> Vector containing the split string
 * Could be an array but eh too lazy
 */
vector<string> split(string str, string regex) {
    vector<string> res;
    int prev = -1;
    int regLen = regex.length();
    for (int i = 0; i < str.length(); i++) {
        if (str.substr(i, regLen) == regex) {
            res.push_back(str.substr(prev + regLen, i - prev - regLen));

            prev = i;
        }
    }
    if (prev == str.length() - regLen) {
        return res;
    } else {
        res.push_back(str.substr(prev + regLen, str.length() - prev - regLen));
    }

    return res;
}

class HitObject {
    /* x,y,time,type,hitSound,hitSample
        ex: 64,192,33421,1,0,0:0:0:0:
    x,y,time,type,hitSound,endTime:hitSample
        ex: 448,192,37821,128,0,38121:0:0:0:70:cymbal.wav
    */
    public:
        int x;
        int y;
        int time;
        int type;
        int hitSound;
        string hitSample;

        // Constructs a HitObject object, with 6 parameters given by a [HitObject] section line in the .osu file
        HitObject(int x, int y, int time, int type, int hitSound, string hitSample) {
            this->x = x;
            this->y = y;
            this->time = time;
            this->type = type;
            this->hitSound = hitSound;
            this->hitSample = hitSample;
        }

    
};

void parseOsu(int x) {
    vector<HitObject> hitObjects;

    ofstream newOsu("delta_new.osu");
    ifstream osu("delta.osu");

    bool obj = false;
    string s;
    while (getline(osu, s)) {
        if (obj) {
            vector<string> temp = split(s, ",");
            hitObjects.push_back(HitObject(stoi(temp[0]), stoi(temp[1]), stoi(temp[2]), stoi(temp[3]), stoi(temp[4]), temp[5]));
        } else {
            newOsu << s + "\n";
        }
        if (s.find("[HitObjects]") != string::npos) {
            obj = true;
        }
    }


    osu.close();
    newOsu.close();
}

int main() {

}
