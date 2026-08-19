// base64_encoder.cpp
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <cctype>
#include <getopt.h>

using namespace std;

static const string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const string base64_url_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

string base64_encode(const unsigned char* bytes, size_t len, bool urlSafe) {
    const string& chars = urlSafe ? base64_url_chars : base64_chars;
    string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    while (len--) {
        char_array_3[i++] = *(bytes++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;
            for (i = 0; i < 4; i++)
                ret += chars[char_array_4[i]];
            i = 0;
        }
    }
    if (i) {
        for (j = i; j < 3; j++)
            char_array_3[j] = '\0';
        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;
        for (j = 0; j < (i + 1); j++)
            ret += chars[char_array_4[j]];
        while ((i++ < 3))
            ret += '=';
    }
    return ret;
}

string base64_decode(const string& encoded, bool urlSafe) {
    const string& chars = urlSafe ? base64_url_chars : base64_chars;
    int in_len = encoded.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    string ret;

    while (in_len-- && (encoded[in_] != '=') && isalnum(encoded[in_]) || (encoded[in_] == '-') || (encoded[in_] == '_') || (encoded[in_] == '/') || (encoded[in_] == '+')) {
        char_array_4[i++] = encoded[in_];
        in_++;
        if (i == 4) {
            for (i = 0; i < 4; i++)
                char_array_4[i] = chars.find(char_array_4[i]);
            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
            for (i = 0; i < 3; i++)
                ret += char_array_3[i];
            i = 0;
        }
    }
    if (i) {
        for (j = i; j < 4; j++)
            char_array_4[j] = 0;
        for (j = 0; j < 4; j++)
            char_array_4[j] = chars.find(char_array_4[j]);
        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
        for (j = 0; j < i - 1; j++)
            ret += char_array_3[j];
    }
    return ret;
}

void progressBar(size_t current, size_t total, int width = 40) {
    if (total == 0) return;
    double percent = (double)current / total;
    int filled = (int)(width * percent);
    cerr << "\rProgress: [";
    for (int i = 0; i < filled; i++) cerr << '█';
    for (int i = filled; i < width; i++) cerr << '░';
    cerr << "] " << (percent * 100) << "%";
    if (current >= total) cerr << endl;
}

int encodeFile(const string& input, const string& output, bool urlSafe) {
    ifstream in(input, ios::binary);
    if (!in.is_open()) {
        cerr << "Error: input file '" << input << "' not found.\n";
        return 1;
    }
    in.seekg(0, ios::end);
    size_t fileSize = in.tellg();
    in.seekg(0, ios::beg);
    ofstream out;
    ostream* outStream = &cout;
    if (!output.empty()) {
        out.open(output);
        if (!out.is_open()) {
            cerr << "Error: cannot create output file.\n";
            return 1;
        }
        outStream = &out;
    }
    const size_t chunkSize = 1024 * 1024;
    vector<char> buffer(chunkSize);
    size_t processed = 0;
    while (in.read(buffer.data(), chunkSize) || in.gcount() > 0) {
        size_t read = in.gcount();
        string encoded = base64_encode((const unsigned char*)buffer.data(), read, urlSafe);
        outStream->write(encoded.c_str(), encoded.size());
        processed += read;
        progressBar(processed, fileSize);
    }
    if (!output.empty()) out.close();
    cout << "\n✅ Encoded '" << input << "' to '" << (output.empty() ? "stdout" : output) << "'\n";
    return 0;
}

int decodeFile(const string& input, const string& output, bool urlSafe) {
    ifstream in(input);
    if (!in.is_open()) {
        cerr << "Error: input file '" << input << "' not found.\n";
        return 1;
    }
    string data((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
    in.close();
    // Remove whitespace
    data.erase(remove_if(data.begin(), data.end(), ::isspace), data.end());
    // Add padding
    while (data.size() % 4) data += '=';
    string decoded = base64_decode(data, urlSafe);
    ofstream out;
    ostream* outStream = &cout;
    if (!output.empty()) {
        out.open(output, ios::binary);
        if (!out.is_open()) {
            cerr << "Error: cannot create output file.\n";
            return 1;
        }
        outStream = &out;
    }
    outStream->write(decoded.c_str(), decoded.size());
    if (!output.empty()) out.close();
    cout << "\n✅ Decoded '" << input << "' to '" << (output.empty() ? "stdout" : output) << "'\n";
    return 0;
}

int main(int argc, char* argv[]) {
    static struct option long_options[] = {
        {"encode", required_argument, 0, 'e'},
        {"decode", required_argument, 0, 'd'},
        {"output", required_argument, 0, 'o'},
        {"url-safe", no_argument, 0, 'u'},
        {"help", no_argument, 0, 'h'},
        {0,0,0,0}
    };
    string encode, decode, output;
    bool urlSafe = false;
    int opt;
    while ((opt = getopt_long(argc, argv, "e:d:o:uh", long_options, nullptr)) != -1) {
        switch (opt) {
            case 'e': encode = optarg; break;
            case 'd': decode = optarg; break;
            case 'o': output = optarg; break;
            case 'u': urlSafe = true; break;
            case 'h':
            default:
                cout << "Usage: base64_encoder -e <file> [-o <out>] [-u]\n"
                     << "       base64_encoder -d <file> [-o <out>] [-u]\n";
                return 0;
        }
    }
    if (encode.empty() && decode.empty()) {
        cerr << "Error: either -e or -d required\n";
        return 1;
    }
    int ret = 0;
    if (!encode.empty()) ret = encodeFile(encode, output, urlSafe);
    else ret = decodeFile(decode, output, urlSafe);
    return ret;
}
