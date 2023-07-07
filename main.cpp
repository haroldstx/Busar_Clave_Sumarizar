#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct Field {
    string name;
    int length;
};
vector<Field> readFieldsFromCSV(const string& csvFile, const string& lengthFile) {
    vector<Field> fields;
    ifstream length(lengthFile);
    if (length.is_open()) {
        string line;
        while (getline(length, line)) {
            string fieldName = line.substr(0, 14);
            string lengthStr = line.substr(14, line.find_last_of(' ') - 14);
            int fieldLength = stoi(lengthStr);
            fields.push_back({ fieldName, fieldLength });
        }
        length.close();
    }
    return fields;
}


vector<string> splitLine(const string& line, char delimiter) {
    vector<string> tokens;
    stringstream ss(line);
    string token;
    while (getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

string padField(const string& field, int length) {
    string paddedField = field;
    if (paddedField.length() > static_cast<size_t>(length)) {
        paddedField = paddedField.substr(0, length);
    } else if (paddedField.length() < static_cast<size_t>(length)) {
        paddedField.resize(length, ' ');
    }
    return paddedField;
}

void convertToFixedSizeRecords(const string& inputFile, const string& lengthFile) {
    vector<Field> fields = readFieldsFromCSV(inputFile, lengthFile);
    string outputFile = inputFile.substr(0, inputFile.find_last_of('.')) + ".fixed";
    ifstream inFile(inputFile);
    ofstream outFile(outputFile);
    if (inFile.is_open() && outFile.is_open()) {
        string line;
        getline(inFile, line);  

  
        for (const Field& field : fields) {
            outFile << padField(field.name, field.length) << ",";
        }
        outFile.seekp(-1, ios_base::cur); // Remove the trailing comma
        outFile << endl;

        while (getline(inFile, line)) {
            vector<string> tokens = splitLine(line, ',');
            string record;
            for (size_t i = 0; i < fields.size(); ++i) {
                string paddedField = padField(tokens[i], fields[i].length);
                record += paddedField;
            }
            outFile << record << endl;
        }
        inFile.close();
        outFile.close();
    } else {
        cout << "Error opening files." << endl;
    }
}
void retrieveRecordByRRN(const string& inputFile, int rrn, const string& lengthFile) {
    ifstream file(inputFile);
    if (file.is_open()) {
        string line;
        int currentRRN = 0;
        vector<Field> fields = readFieldsFromCSV(inputFile, lengthFile);
        getline(file, line); // Skip the header line

     
        getline(file, line);
        vector<string> titles = splitLine(line, ',');

       
        for (size_t i = 0; i < fields.size(); ++i) {
            cout << fields[i].name << "\t\t" << titles[i] << endl;
        }

        while (getline(file, line)) {
            ++currentRRN;
            if (currentRRN == rrn) {
                vector<string> tokens = splitLine(line, ',');
                if (tokens.size() == fields.size()) {
                    for (size_t i = 0; i < fields.size(); ++i) {
                        cout << fields[i].name << "\t\t" << tokens[i] << endl;
                    }
                } else {
                    cout << "Invalid record." << endl;
                }
                break;
            }
        }
        file.close();
    } else {
        cout << "Error opening file." << endl;
    }
}



int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <csvFile> [-convert <lengthFile>]" << endl;
        return 1;
    }

    string inputFile = argv[1];

    if (argc == 2) {
        int rrn;
        cout << "Enter RRN: ";
        cin >> rrn;
        retrieveRecordByRRN(inputFile, rrn, "length-file.txt");
    } else if (argc == 4 && string(argv[2]) == "-convert") {
        // Convert to fixed-size records
        string lengthFile = argv[3];
        convertToFixedSizeRecords(inputFile, lengthFile);
        cout << "Conversion completed." << endl;
    } else {
        cout << "Invalid arguments." << endl;
        cout << "Usage: " << argv[0] << " <csvFile> [-convert <lengthFile>]" << endl;
        return 1;
    }

    return 0;
}