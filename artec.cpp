#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>

struct SampleData {
    int32_t int_field;
    bool bool_field;
    std::string string_field;
};

void serialize(const SampleData& data, const std::string& filename) {
    std::ofstream outfile(filename, std::ios::out | std::ios::binary);
    if (!outfile.is_open()) {
        std::cerr << "Error: could not open file " << filename << std::endl;
        return;
    }

    // Encode integer field
    std::vector<uint8_t> int_field_encoded;
    uint32_t int_field = data.int_field;
    while (int_field > 127) {
        uint8_t byte = (int_field & 127) | 128;
        int_field_encoded.push_back(byte);
        int_field >>= 7;
    }
    int_field_encoded.push_back(static_cast<uint8_t>(int_field));
    outfile.write(reinterpret_cast<const char*>(&int_field_encoded[0]), int_field_encoded.size());

    // Encode boolean field
    uint8_t bool_field = data.bool_field ? 1 : 0;
    outfile.write(reinterpret_cast<const char*>(&bool_field), 1);

    // Encode string field
    std::vector<uint8_t> string_field_encoded;
    uint32_t string_field_length = static_cast<uint32_t>(data.string_field.size());
    while (string_field_length > 127) {
        uint8_t byte = (string_field_length & 127) | 128;
        string_field_encoded.push_back(byte);
        string_field_length >>= 7;
    }
    string_field_encoded.push_back(static_cast<uint8_t>(string_field_length));
    outfile.write(reinterpret_cast<const char*>(&string_field_encoded[0]), string_field_encoded.size());
    outfile.write(data.string_field.c_str(), data.string_field.size());

    outfile.close();
}

SampleData deserialize(const std::string& filename) {
    std::ifstream infile(filename, std::ios::in | std::ios::binary);
    if (!infile.is_open()) {
        std::cerr << "Error: could not open file " << filename << std::endl;
        return SampleData();
    }

    SampleData data;

    // Decode integer field
    uint8_t byte;
    uint32_t shift = 0;
    uint32_t int_field = 0;
    do {
        infile.read(reinterpret_cast<char*>(&byte), 1);
        int_field |= static_cast<uint32_t>(byte & 127) << shift;
        shift += 7;
    } while (byte & 128);
    data.int_field = static_cast<int32_t>(int_field);

    // Decode boolean field
    infile.read(reinterpret_cast<char*>(&byte), 1);
    data.bool_field = (byte != 0);

    // Decode string field
    shift = 0;
    uint32_t string_field_length = 0;
    do {
        infile.read(reinterpret_cast<char*>(&byte), 1);
        string_field_length |= static_cast<uint32_t>(byte & 127) << shift;
        shift += 7;
    } while (byte & 128);
    std::vector<char> string_field_data(string_field_length);
    infile.read(&string_field_data[0], string_field_length);
    data.string_field.assign(string_field_data.begin(), string_field_data.end());

    infile.close();

    return data;
}


int main(int argc, char** argv) {
    // Parse command line arguments here
    // ...

    // Define sample data
    SampleData data = {21, false, "Hello!"};

    // Serialize sample data to file
    std::string filename = "sample_data.bin";
    serialize(data, filename);

    // Deserialize sample data from file
    SampleData deserialized_data = deserialize(filename);

    // Print deserialized data to console
    std::cout << "Deserialized data:" << std::endl;
    std::cout << "int_field: " << deserialized_data.int_field << std::endl;
    std::cout << "bool_field: " << std::boolalpha << deserialized_data.bool_field << std::endl;
    std::cout << "string_field: " << deserialized_data.string_field << std::endl;

    return 0;
}

