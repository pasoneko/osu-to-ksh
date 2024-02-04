#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <numeric>
#include <cmath>
#include <set>
#include <unordered_set>

using namespace std;

double timeSignature;
double quarternoteLength;
int offset;
double BPM;
double measureLength;

class HitObject {
    public:
        int col; // column, 64 = 0, 192 = 1, 320 = 3, 448 = 4
        int time; // time in milliseconds
        // deal with LNs later
        HitObject(int c, int t) {
            col = c;
            time = t;
        }
};

class Measure {
    public:
        vector<HitObject> notes;
};

double calculateBPM(double tp) {
    // tp is the value next to the offset in the TimingPoints section, it is the length of 1 quarternote at BPM
    double bpm = 1.0 / tp * 1000 * 60;
    return bpm;
}

vector<string> splitHitObject(string s){
    vector<string> res;
    int pos = 0;
    while(pos < s.size()){
        pos = s.find(",");
        res.push_back(s.substr(0,pos));
        s.erase(0,pos+1); // 1 is the length of the delimiter, ","
    }
    return res;
}

int determineOptimalNoteLines(Measure measure) {
    vector<HitObject> hitObjects = measure.notes;
    if (hitObjects.size() == 0) return 1;

    vector<int> timeIntervals;
    for (size_t i = 1; i < hitObjects.size(); i++) {
        int k = hitObjects[i].time - hitObjects[i - 1].time;
        if (k != 0) {
            timeIntervals.push_back(k);
        }
    }

    if (timeIntervals.size() == 0) return 1;

    int minT = timeIntervals[0];
    for (size_t i = 1; i < timeIntervals.size(); i++) {
        if (timeIntervals[i] < minT) {
            minT = timeIntervals[i];
        }
    }

    return (int) round(measureLength / minT);
}

int determineNoteLine(int optimalNoteLines, HitObject hitObject) {
    return round(optimalNoteLines * BPM * (fmod(hitObject.time - offset, measureLength)) / (240000 * timeSignature)) + 1;
}

int determineColumn(int x) {
    // column, 64 = 0, 192 = 1, 320 = 3, 448 = 4
    switch(x) {
        case 64:
            return 0;
        case 192:
            return 1;
        case 320:
            return 2;
        case 448:
            return 3;
    }
    return -1;
}

string writeNoteLine(string parsekey) {
    string res = "";
    if (parsekey.find("0") != string::npos) {
        res += "1";
    } else res += "0";
    if (parsekey.find("1") != string::npos) {
        res += "1";
    } else res += "0";
    if (parsekey.find("2") != string::npos) {
        res += "1";
    } else res += "0";
    if (parsekey.find("3") != string::npos) {
        res += "1";
    } else res += "0";

    return res += "|00|--";
}

int main() {
    ifstream osu("hardcore.osu");
    ofstream ksh("hardcore.ksh");

    unordered_map<string, string> header;
    vector<HitObject> hitObjects;

    string s;
    bool timingPoints = false;
    bool skip = false;
    bool metadata = true;
    while(getline(osu, s)) {
        if (metadata) {
            string firstTiming;

            if (s.find("Title:") != string::npos) header["title"] = s.substr(s.find_last_of(":") + 1);
            if (s.find("Artist:") != string::npos) header["artist"] = s.substr(s.find_last_of(":") + 1);
            if (s.find("Creator:") != string::npos) header["effect"] = s.substr(s.find_last_of(":") + 1);
            if (s.find("Version:") != string::npos) header["difficulty"] = s.substr(s.find_last_of(":") + 1);
            if (timingPoints & !skip) {
                firstTiming = s;
                skip = true; 
                
                int pos = firstTiming.find(",");
                int pos1 = firstTiming.find(",", pos+1);
                header["o"] = firstTiming.substr(0, pos);
                offset = stod(header["o"]);
                header["t"] = firstTiming.substr(pos+1, pos1-pos-1);
                quarternoteLength = stod(header["t"]);
            }
            if (s.find("[TimingPoints]") != string::npos) timingPoints = true;

            if (s.find("AudioFilename:") != string::npos) header["m"] = s.substr(s.find_last_of(":") + 2);
            if (s.find("PreviewTime:") != string::npos) header["po"] = s.substr(s.find_last_of(":") + 2);

            if (s.find("BeatDivisor:") != string::npos) header["beat"] = s.substr(s.find_last_of(":") + 2);

            if (s.find("HitObjects") != string::npos) metadata = false;
        } else {
            // convert HitObject to noteline
            if (s.find(",") == string::npos) break;
            vector<string> temp = splitHitObject(s);
            hitObjects.push_back(HitObject(stoi(temp[0]), stoi(temp[2])));
        }
    }

    ksh <<  string("title=") + header["title"] + "\n" +
            "artist=" + header["artist"] + "\n" +
            "effect=" + header["effect"] + "\n" +
            "jacket=" + "\n" +
            "illustrator=" + "\n" +
            "difficulty=" + header["difficulty"] + "\n" +
            "level=" + "1\n" +
            "t=" + to_string(calculateBPM(stod(header["t"]))) + "\n" +
            "m=" + header["m"] + "\n" +
            "mvol=75\n" +
            "o=" + header["o"] + "\n" +
            "bg=cyber\n" +
            "layer=arrow\n" +
            "po=" + header["po"] + "\n" +
            "plength=15000\n" +
            "pfiltergain=50\n" +
            "filtertype=peak\n" +
            "chokkakuautovol=0\n" +
            "chokkakuvol=50\n" +
            "ver=\n" +
            "--" + "\n" +
            "beat=" + header["beat"] + "/4\n" +
            "t=" + to_string(calculateBPM(stod(header["t"]))) + "\n";

    timeSignature = stod(header["beat"]) / 4.0;
    BPM = calculateBPM(stod(header["t"]));
    measureLength = stod(header["t"]) * stod(header["beat"]);
    
    int totalMeasures = hitObjects[hitObjects.size()-1].time / measureLength + 1;
    
    Measure measures[totalMeasures];

    for (HitObject h : hitObjects) {
        int measureNumber = (h.time - offset) / measureLength;
        measures[measureNumber].notes.push_back(h);
    }


    for (Measure measure : measures) {
        if (measure.notes.size() == 0) {
            ksh << "0000|00|--" << "\n";
        } else {
            int optimalNoteLines = determineOptimalNoteLines(measure);
            string parsekey = to_string(determineColumn(measure.notes[0].col));
            for (size_t i = 1; i < measure.notes.size(); i++) {
                if (measure.notes[i].time == measure.notes[i-1].time) {
                    parsekey += to_string(determineColumn(measure.notes[i].col));
                } else {
                    parsekey += "," + to_string(determineColumn(measure.notes[i].col));
                }
            }
            // should look something like "1,3,0,21"
            int k = 0;
            vector<string> linekeys = splitHitObject(parsekey);
            // index = noteposition - 1
            unordered_set<int> notepositions;

            for (size_t j = 0; j < measure.notes.size(); j++) {
                notepositions.insert(determineNoteLine(optimalNoteLines, measure.notes[j]));
            }

            for (size_t m = 1; m <= optimalNoteLines; m++) {
                if (notepositions.count(m) > 0) {
                    ksh << writeNoteLine(linekeys[k]) << "\n";
                    k++;
                } else {
                    ksh << "0000|00|--" << "\n";
                }
            }
            
        }

        ksh << "--\n";
    }
    

    osu.close();
    ksh.close();

    return 0;
}