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

template <typename T>
bool read_nbits(T& x, ifstream& file){
    file.read(reinterpret_cast<char*>(&x), sizeof(x));
    return file.good();
}

void read_string(std::ifstream& file, std::string& s) {
    uint16_t length;
    if (!read_nbits(length, file)) {
        s.clear();
        return; // failed to read length
    }

    s.resize(length);
    if (!file.read(&s[0], length)) {
        s.clear();
        return; // failed to read string data
    }
}


int main(){
    string inputFileName = "Task1.bin";
    ifstream binFile(inputFileName, ios::binary); //creates the file object

    string outputFileName = "Task1_decoded.csv";
    ofstream outputFile(outputFileName);
    string last_ts, last_id;
    uint8_t line_type;

    cout<<"starting to decode "<<inputFileName<<" into "<<outputFileName<<endl;
    int count_D = 0;
    int count_V = 0;

    bool metadata_b = false;

    while (binFile.read(reinterpret_cast<char*>(&line_type), 1)) {
        if(line_type == 3){ //decode BinData
            count_D++;
            uint8_t type;
            string x_;
            //variant<bool, float, double, int, long long, string> x_;
            
            if(!read_nbits(type, binFile)){
                cout<<"non good read"<<endl;
            }

            bool read = true;
            switch(type){
                case 0:
                    {
                    x_ = "NaN";
                    break;}
                case 1:
                    {float x;
                    read = read_nbits(x, binFile);
                    x_ = to_string(x);
                    break;}
                case 2:
                    {double x;
                    read = read_nbits(x, binFile);
                    x_ = to_string(x);
                    break;}
                case 3:
                    {int x;
                    read = read_nbits(x, binFile);
                    x_ = to_string(x);
                    break;}
                case 4:
                    {long long x;
                    read = read_nbits(x, binFile);
                    x_ = to_string(x);
                    break;}
                case 5:{
                    x_ = "inf";
                    break;}
                case 6:{
                    x_ = "-inf";
                    break;}
                default:
                    cout<<"Error, unrecognized data uint8_t type at data entry number: "<<count_D<<endl;
                    cout<<"Type :"<<type<<endl;
            }
            
            if(read == false){
                cout<<"non good read"<<endl;
            }
            uint32_t timestamp;
            if(!read_nbits(timestamp, binFile)){
                cout<<"non good read"<<endl;
            }
            uint16_t id;
            if(!read_nbits(id, binFile)){
                cout<<"non good read"<<endl;
            }
            
            outputFile <<to_string(timestamp)<<","<<to_string(id)<<","<<x_<<"\n";   // prints whichever type is inside
            
        }
        else if(line_type == 2){ //decode ValueLine
            count_V++;
            uint16_t id;
            string name, descriptor;
            read_nbits(id, binFile);
            read_string(binFile, name);
            read_string(binFile, descriptor);
            outputFile <<"Value "<< name <<"("<<descriptor<<")"<<":"<<id<<"\n";
        }
        else{ //BinMetadata
            if (metadata_b == true){
                cout<<"Unrecognized line type "<<line_type<<endl;
                cout<<"line number: "<<count_D<<endl;
            }
            else{
                string metadata_string;
                read_string(binFile, metadata_string);
                outputFile <<metadata_string<<"\n"; //print to csv
            }
        }
    }

    cout<<"number of Values: "<<count_V<<endl;
    cout<<"number of data entries: "<<count_D<<endl;

    binFile.close();
    outputFile.close();
    cout<<"file decoded succefully"<<endl;

    return 0;
}