#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <stdexcept>
#include <cstdint>
#include "BinMetaData.h"
#include "BinData.h"
using namespace std;

ifstream open_file(string fileName){
    ifstream file(fileName);
    if (!file.is_open()) {
        cerr << "Error opening file"<<endl;
    }
    else{
        cout<<"Opened file succesfully"<<endl;
    }

    return file;
}

void save_metadata_line(ifstream& file, BinMetaData& headerData){
    string line1;
    if (std::getline(file, line1)) {
        headerData.metadata = line1;
    }
    else{
        cerr << "Error retrieving first metadata line from csv file"<<endl;
    }
}

void save_name_descriptor(string value_line, string& name, string& descriptor){
    size_t openParen  = value_line.find('(');
    size_t closeParen = value_line.find(')');

    if (openParen != string::npos && closeParen != string::npos && closeParen > openParen) {
        descriptor = value_line.substr(openParen + 1, closeParen - openParen - 1);

        size_t firstSpace = value_line.find(' ');
        if (firstSpace != std::string::npos && firstSpace + 1 < openParen) {
            name = value_line.substr(firstSpace + 1, openParen - firstSpace - 1);

            while (!name.empty() && isspace(name.front())) {
                name.erase(name.begin());
            }
            while (!name.empty() && isspace(name.back())) {
                name.pop_back();
            }
        }
    } else {
        name.clear();
        descriptor.clear();
    }
}


void save_BinData_to_binary(BinData obj, ofstream& outputFile){
    uint32_t ts = obj.TIMESTAMP;
    uint16_t di = obj.id;
    outputFile.write(reinterpret_cast<const char*>(&ts), sizeof(ts));
    outputFile.write(reinterpret_cast<const char*>(&di), sizeof(di));
}

void save_type_of_line(uint8_t type_of_line, ofstream& outputFile){
    outputFile.write(reinterpret_cast<const char*>(&type_of_line), sizeof(type_of_line));
}

void write_string_to_binary(ofstream& outputFile, uint16_t lenght, string text){
    outputFile.write(reinterpret_cast<const char*>(&lenght), sizeof(lenght)); 
    outputFile.write(text.data(), lenght);
}

void save_valueLine_to_binary(uint16_t id, BinValueLine valueLine, ofstream& outputFile){
    save_type_of_line(2, outputFile); //type of output (int, float, long long,...)
    outputFile.write(reinterpret_cast<const char*>(&id), sizeof(id)); //id
    write_string_to_binary(outputFile, static_cast<uint16_t>(valueLine.name.size()), valueLine.name); //name
    write_string_to_binary(outputFile, static_cast<uint16_t>(valueLine.descriptor.size()), valueLine.descriptor); //descriptor
}

void save_headerData_to_binary(BinMetaData headerData, ofstream& outputFile){
    save_type_of_line(1, outputFile); //these two functions save the headredata metadat to the binary file
    write_string_to_binary(outputFile, static_cast<uint16_t>(headerData.metadata.size()), headerData.metadata);    
}

void remove_all_spaces(string& s){
    for (size_t i = 0; i < s.size();) {
        if (s[i] == ' ')
            s.erase(i, 1);
        else
            ++i;
    }
}

int main() {
    BinMetaData headerData; //headreData contains the first line "PER CSV Modbus Log 05/25/23 09:39:05 PM", and all the other 'Value' line
    string inputFileName = "Task1.csv";
    ifstream csvfile = open_file(inputFileName); //creates the file object

    string outputFileName = "Task1.bin";
    ofstream outputFile(outputFileName, ios::binary | ios::out);

    //metadata
    save_metadata_line(csvfile, headerData); //saves the first line to the headerData object
    save_headerData_to_binary(headerData, outputFile);//these function saves the headerdata metadats to the binary file
    
    cout<<headerData.metadata<<endl;

    // Value lines
    string line;
    int count_V = 0;
    int count_D = 0;

    //cout<<"List of IDs:"<<endl;

    while (getline(csvfile, line)) {

        size_t check = line.find("Value");
        if (check != string::npos){
            BinValueLine valueLine;
            size_t colonPos = line.find(':'); //find where the ':' is at
            if (colonPos != std::string::npos) {
                string value_line_string = line.substr(0, colonPos);  // "Value Pcm OK (pcm.signals.pcmOk)"
                save_name_descriptor(value_line_string, valueLine.name, valueLine.descriptor);
                string idPart = line.substr(colonPos + 1);  // e.g. " 8192"
                headerData.ValueLines[static_cast<uint16_t>(stoi(idPart))] = valueLine; //saves to map
                //cout<<static_cast<uint16_t>(stoi(idPart))<<"\t"; //print to terminal to check all values are read
            }
            count_V++;
        }
        else{
            /*for (const auto& [key, valueLine] : headerData.ValueLines) { //saves all BinValueLine(s)
                save_valueLine_to_binary(key, valueLine, outputFile);
            }*/

            stringstream ss(line);
            string timest, id_, data;
            
            if (getline(ss, timest, ',') && getline(ss, id_, ',') && getline(ss, data, ',')) {
                if (count_D == (16447118-1)){
                    cout<<"line"<<line<<endl;
                }
                
                BinData data_obj;
                
                data_obj.TIMESTAMP = static_cast<uint32_t>(stoi(timest));
                data_obj.id = static_cast<uint16_t>(stoi(id_));
                BinValueLine* line_by_id = headerData.getValueLine(data_obj.id);
                save_type_of_line(3, outputFile);
                uint8_t type_ = 0;

                if(data == "0" || data == "1"){ //bool
                    type_ = 3;
                    outputFile.write(reinterpret_cast<const char*>(&type_), sizeof(type_));
                    int x = stoi(data);
                    outputFile.write(reinterpret_cast<const char*>(&x), sizeof(x));
                }
                else if(data == "inf"){
                    type_ = 5;
                    outputFile.write(reinterpret_cast<const char*>(&type_), sizeof(type_));
                }
                else if(data == "NaN"){
                    type_ = 0;
                    outputFile.write(reinterpret_cast<const char*>(&type_), sizeof(type_));
                }
                else if(data == "-inf"){
                    type_ = 6;
                    outputFile.write(reinterpret_cast<const char*>(&type_), sizeof(type_));
                }
                else if(data.find(".") != string::npos){
                    try{
                        type_ = 1; //float
                        float x = stof(data);
                        outputFile.write(reinterpret_cast<const char*>(&type_), sizeof(type_));
                        outputFile.write(reinterpret_cast<const char*>(&x), sizeof(x));
                    }
                    catch(const out_of_range& e){
                        type_ = 2; //double
                        double x = stod(data);
                        outputFile.write(reinterpret_cast<const char*>(&type_), sizeof(type_));
                        outputFile.write(reinterpret_cast<const char*>(&x), sizeof(x));
                    }
                    catch(const exception& e){ //fails for overflow error
                        throw e;
                    }
                }
                else{
                    try{ //32 bit int
                        type_ = 3;
                        int x = stoi(data);
                        outputFile.write(reinterpret_cast<const char*>(&type_), sizeof(type_));
                        outputFile.write(reinterpret_cast<const char*>(&x), sizeof(x));
                    }
                    catch(const out_of_range& e){ //64 bit int, when 32 bit int fails for overflow error
                        type_ = 4;
                        long long x = stoll(data);
                        outputFile.write(reinterpret_cast<const char*>(&type_), sizeof(type_));
                        outputFile.write(reinterpret_cast<const char*>(&x), sizeof(x));
                    }
                    catch(const exception& e){ //fails for overflow error
                        throw e; //probably a numbr to big even for double
                    }
                }
                /*if (type_ ==4){
                    cout<<"type of line"<<type_<<endl;
                }*/

                save_BinData_to_binary(data_obj, outputFile);
            }
            //break;
        count_D++;
        }
    }
    cout<<endl;
    cout<<"number of Value lines: "<<count_V<<endl;
    cout<<"number of Data entries: "<<count_D<<endl;

    
    outputFile.close();
    csvfile.close();
    cout<<"Data written with success"<<endl;
    return 0;
}