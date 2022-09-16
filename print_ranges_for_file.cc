/*******************************************************************************
 The MIT License (MIT)

 Copyright (c) 2015 Alexander Zolotarev <me@alex.bio> from Minsk, Belarus

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 *******************************************************************************/

// Prints character ranges from text file, with a char code on each line.

#include <string.h>

#include <algorithm>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct UnicodeRange {
  uint32_t first_;
  uint32_t last_;
  string name_;
};

void PrintRanges(const vector<UnicodeRange> & ranges, vector<uint32_t> charcodes) {
  if (charcodes.empty()) {
    return;
  }
  // Display font ranges.
  for (const auto & range : ranges) {
    size_t count = 0;
    for (auto it = charcodes.begin(); it != charcodes.end();) {
      if (*it >= range.first_ && *it <= range.last_) {
        ++count;
        it = charcodes.erase(it);
      } else {
        ++it;
      }
    }
    if (count) {
      const bool stress = (double)count / (range.last_ - range.first_ + 1) >= 0.5;
      if (stress) cout << "\033[0;36m";
      cout << range.name_ << ' ' << count << '/' << range.last_ - range.first_ + 1 << endl;
      if (stress) cout << "\033[0m";
    }
  }
  if (!charcodes.empty()) {
    cout << "Number of characters from unspecified range: " << charcodes.size() << endl;
  }
  cout << endl;
}

int main(int argc, char ** argv) {
  if (argc < 2) {
    cout << "Usage: " << argv[0] << " <file with hex char on each line> [full path to Blocks.txt]" << endl;
    cout << "  If path to Blocks.txt is not specified, it is searched in the current directory." << endl;
    return -1;
  }

  // Load unicode ranges.
  vector<UnicodeRange> ranges;
  {
    const string path = argc < 3 ? "Blocks.txt" : argv[2];
    ifstream file(path);
    string line;
    while (file) {
      getline(file, line);
      if (line.empty() || line[0] == '#') {
        continue;
      }
      const size_t dots = line.find("..");
      const size_t semicolon = line.find("; ");
      if (dots == string::npos || semicolon == string::npos) {
        continue;
      }
      ranges.push_back({static_cast<uint32_t>(stol(line.substr(0, dots), 0, 16)),
                        static_cast<uint32_t>(stol(line.substr(dots + 2, semicolon - dots - 2), 0, 16)),
                        line.substr(semicolon + 2)});
    }
    if (ranges.empty()) {
      cout << "Can't load unicode ranges from " << path << '.' << endl;
      return -1;
    }
  }

  ifstream file(argv[1]);
  string line;
  vector<uint32_t> charcodes;
  while (file) {
    getline(file, line);
    if (line.empty()) {
      continue;
    }
    char * token = strtok(&line[0], ",");
    while (token) {
      charcodes.push_back(static_cast<uint32_t>(stoi(string(token), 0, 16)));
      token = strtok(nullptr, ",");
    }
  }
  PrintRanges(ranges, charcodes);
  return 0;
}
