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

#include <ft2build.h>
#include FT_FREETYPE_H

#include <algorithm>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct ScopeGuard {
  typedef function<void()> TLambda;
  TLambda lambda_;
  ScopeGuard(TLambda lambda) : lambda_(lambda) {}
  ~ScopeGuard() { lambda_(); }
};

struct UnicodeRange {
  FT_ULong first_;
  FT_ULong last_;
  string name_;
};

void PrintRanges(const vector<UnicodeRange> & ranges, vector<FT_ULong> charcodes) {
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
}

int main(int argc, char ** argv) {
  if (argc < 2) {
    cout << "Usage: " << argv[0] << " <ttf font file> [full path to Blocks.txt]" << endl;
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
      ranges.push_back({static_cast<FT_ULong>(stol(line.substr(0, dots), 0, 16)),
                        static_cast<FT_ULong>(stol(line.substr(dots + 2, semicolon - dots - 2), 0, 16)),
                        line.substr(semicolon + 2)});
    }
    if (ranges.empty()) {
      cout << "Can't load unicode ranges from " << path << '.' << endl;
      return -1;
    }
  }

  FT_Library library;
  FT_Error error = FT_Init_FreeType(&library);
  if (error) {
    cout << "Error " << error << " while initializing FreeType library." << endl;
    return error;
  }
  const ScopeGuard lib_guard([&library]() { FT_Done_FreeType(library); });

  cout << "File " << argv[1] << endl;
  // Font file (e.g. ttc) can contain more than one face. Process them all!
  int num_faces = 1;
  for (int i = 0; i < num_faces; ++i) {
    FT_Face face;
    error = FT_New_Face(library, argv[1], i, &face);
    if (error) {
      cout << "Error " << error << " while opening ttf file " << argv[1] << '.' << endl;
      return error;
    }
    const ScopeGuard face_guard([&face]() { FT_Done_Face(face); });

    num_faces = face->num_faces;
    cout << string(80, '*') << endl;
    cout << "Face index: " << face->face_index << endl;
    string style;
    if (face->style_flags & FT_STYLE_FLAG_BOLD) {
      style += "Bold ";
    }
    if (face->style_flags & FT_STYLE_FLAG_ITALIC) {
      style += "Italic ";
    }
    if (!style.empty()) {
      cout << "Style: " << style << endl;
    }
    if (face->family_name) {
      cout << "Family name: " << face->family_name << endl;
    }
    if (face->style_name) {
      cout << "Style name: " << face->style_name << endl;
    }
    // TODO(AlexZ): Also get charmap info.
    vector<FT_ULong> charcodes;
    {
      charcodes.reserve(face->num_glyphs);
      FT_UInt glyph_index;
      FT_ULong charcode = FT_Get_First_Char(face, &glyph_index);
      while (glyph_index != 0) {
        charcodes.push_back(charcode);
        charcode = FT_Get_Next_Char(face, charcode, &glyph_index);
      }
      sort(charcodes.begin(), charcodes.end());
      charcodes.erase(unique(charcodes.begin(), charcodes.end()), charcodes.end());
    }
    PrintRanges(ranges, charcodes);
  }
  cout << endl;
  return 0;
}
