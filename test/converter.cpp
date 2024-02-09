#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <unordered_set>
#include <algorithm>

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
        bool isFirstNote;
        // deal with LNs later
        HitObject(int c, int t) {
            col = c;
            time = t;
            isFirstNote = false;
        }
        HitObject(int c, int t, bool f) {
            col = c;
            time = t;
            isFirstNote = f;
        }
};

class TimingPoint {
    public:
        int time;
        int beatLength;
        int meter;

        TimingPoint(int t, int b, int m) {
            time = t;
            beatLength = b;
            meter = m;
        }
};

class Measure {
    public:
        vector<HitObject> notes;
};

// Unused
class UnsupportedKeymode : public exception {
    public:
        char* what() {
            return "Keymode not currently supported qwq";
        }
};

double calculateBPM(double tp) {
    // tp is the value next to the offset in the TimingPoints section, it is the length of 1 quarternote at X BPM
    return 1.0 / tp * 1000 * 60;
}

vector<string> splitAtComma(string str){
    vector<string> res;
    stringstream ss(str);
    string seg;

    while (getline(ss, seg, ',')) {
        res.push_back(seg);
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
    if (hitObject.isFirstNote) return 1;
    return round(optimalNoteLines * BPM * (fmod(hitObject.time - offset, measureLength)) / (240000 * timeSignature)) + 1;
}

// only works for 4K at the moment
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
        default:
            cout << "This osu file is either for an unsupported keymode or has irregular x positions. The converter will attempt to auto-compensante." << endl;
            cout << "Value: " << x << endl;
            int arr[] = {abs(64-x), abs(192-x), abs(320-x), abs(448-x)};
            int min = arr[0], i = 0;
            for (size_t j = 1; j < 4; j++) {
                if (arr[j] < min) {
                    min = arr[j];
                    i = j;
                }
            }
            return i;
    }
}

string writeNoteLine(string parsekey) {
    if (parsekey.length() == 0) {
        return "0000|00|--";
    }

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
    string title;
    cout << "Enter file name without .osu:" << endl;
    cin >> title;
    ifstream osu(title + ".osu");
    ofstream ksh(title + ".ksh");

    unordered_map<string, string> header;
    vector<HitObject> hitObjects;
    vector<TimingPoint> timingPoints; // not implemented yet

    string s;
    bool timings = false;
    bool skip = false;
    bool metadata = true;
    while(getline(osu, s)) {
        if (metadata) {
            string firstTiming;

            if (s.find("Title:") != string::npos) header["title"] = s.substr(s.find_last_of(":") + 1);
            if (s.find("Artist:") != string::npos) header["artist"] = s.substr(s.find_last_of(":") + 1);
            if (s.find("Creator:") != string::npos) header["effect"] = s.substr(s.find_last_of(":") + 1);
            if (s.find("Version:") != string::npos) header["difficulty"] = s.substr(s.find_last_of(":") + 1);
            if (timings && !skip) {
                vector<string> timing = splitAtComma(s);

                skip = true; 
                
                header["o"] = timing[0];
                offset = stod(header["o"]);
                header["t"] = timing[1];
                quarternoteLength = stod(header["t"]);
                header["beat"] = timing[2];
            }
            if (s.find("[TimingPoints]") != string::npos) timings = true;

            if (s.find("AudioFilename:") != string::npos) header["m"] = s.substr(s.find_last_of(":") + 2);
            if (s.find("PreviewTime:") != string::npos) header["po"] = s.substr(s.find_last_of(":") + 2);

            if (s.find("HitObjects") != string::npos) metadata = false;

            if (s.find("jpg") != string::npos || s.find("png") != string::npos) {
                vector<string> event1 = splitAtComma(s);
                header["jacket"] = event1[2].substr(1, event1[2].find_last_of("\"")-1);
            }
        } else {
            if (s.find(",") == string::npos) break;
            vector<string> temp = splitAtComma(s);
            hitObjects.push_back(HitObject(stoi(temp[0]), stoi(temp[2])));
        }
    }

    ksh <<  string("title=") + header["title"] + "\n" +
            "artist=" + header["artist"] + "\n" +
            "effect=" + header["effect"] + "\n" +
            "jacket=" + header["jacket"] + "\n" +
            "illustrator=" + "\n" +
            "difficulty=infinite" + "\n" + // header["difficulty"] grabs difficulty but USC defaults to novice, infinite = maximum
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

    // this can probably just be handled when creating notes but whatever
    for (HitObject h : hitObjects) {
        double q1 = ((double) h.time - offset) / measureLength;
        double q2 = round(q1);
        int measureNumber = (int) q1;

        double dq = abs(q2-q1);
        // h.time != offset handles first note which will always fall on offset i think ?
        if (h.time != offset && dq < 0.0005) {
            measureNumber++;
            h.isFirstNote = true;
        }

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
            vector<string> linekeys = splitAtComma(parsekey);
            unordered_set<int> notepositions;

            for (size_t j = 0; j < measure.notes.size(); j++) {
                notepositions.insert(determineNoteLine(optimalNoteLines, measure.notes[j]));
            }

            

            int k = 0;
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