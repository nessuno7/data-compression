My data format is a binary file of the csv format.
In order to convert the binary file I created 3 Struct that contained all the information to store the data.
BinValueLine contains all the info needed to encode and decode the "Value" lines:
string name for the variable name,
string descriptor for the variable's description
uint16_t (unsigned 16 bit integer) for the ID (this saves space compared to normal declaration of 32-bit integer)

BinMetaData contained the string of the first line of the csv file as well as an unordered map of all the BinValueLines. Unorder map was chosen instead of a vector because it allows to find elements instantly, which in my initial plan was quite important for the encoding of the actual data, however the code changed and such speed was not needed.

BinData: A struct to contain the actual data. a uint32_t timestamp and a unsigned 16 bit integer for the ID. It also included a variant data, that allows to store different types of data (float, int, ...) decide after build.

To turn the struct into binary I followed this system:
MetaData: 1 byte for the type of line identifier (1), 2 bytes (uint16_t) for the size of the string, string saved as 1 char per byte.
BinValueLine: 1 byte for the type of line identifier (2), 2 bytes ((uint16_t)) for the id, 2 bytes for the size of the name string, name string, 2 bytes for the descriptor string size, descriptor string. 
BinData: 1 byte for the type of line identifier (3), 1 byte(uint8_t) for the type of data (eg int->3, float->1, ...), n bytes depending of type of data, 4 bytes for timestamp, 2 bytes for id.

Looselessness: for all the data no info is lost, some information is also deduced, such as the type of data (int, float, double, ...).
Space efficiency: running test the program is able to free around a third of the file (from 300MB to 200MB).
Extensibility: it is able to convert standard types autonomously, but some info a manual exception (if-else) is needed, for example for the following strings "inf", "-inf", "NaN".
Maintainability: The binary file can be opened in hexdump and for BinMetaData and BinValueLine it is possible to read most of the contents.
Streaming inputs: yes it will be, because it uses no information from previous pieces of data to store it (it deduces the type based on the data it is given just for that line).
Corruption: the file did get corrupted a few times due to memory overflow while debugging on my laptop. It is very easily noticeable because random characters will appear if the file is opened in cvs editor.

The encoder and decoder definitely do work as the decode file contains the same information as the original and the size of the binary log file is smaller than that of the csv file. 
To compile and test the code copy the "Task1.csv" file in the same folder as all the source files. Run "encoder.cpp", which encoed the data into "Task1.bin". "decoder.cpp" decodes the data in "Task1.bin" to "Task1_decoded.csv"

I used llm to write parts of code that had to do with managing strings, but also to ask about standard in c++ that could help me in the code, such as the variant type.

Read ideas.txt for the thought process behind the binary conversion of the csv file.