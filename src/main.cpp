#include "CharConverter.hpp"
#include <cstdlib>
#include <cstring>
#include <iostream>
using std::cout, std::endl;

constexpr std::size_t max_size = 4;

int main(int argc, char **argv) {
  char8_t input[5] = "😊";

  std::size_t consumed = 0;
  char32_t utf32 = 0;
  CharConverter::cvtUTF8ToUTF32(input, max_size, &consumed, &utf32);

  std::memset(input + consumed, 0, max_size - consumed);
  cout << "入力した文字 (" << consumed << "バイト): ";
  cout << input << endl;

  cout << std::hex;
  cout << "UTF-32コード: 0x" << utf32 << endl;

  char16_t utf16[2] = {};
  std::size_t utf16_size = 0;
  CharConverter::cvtUTF32ToUTF16(utf32, utf16, &utf16_size);

  cout << std::hex;
  cout << "UTF-16コード: 0x" << utf16[0];
  if (utf16_size == 2) {
    cout << ", " << utf16[1] << " (サロゲート)";
  }
  cout << endl;

  return 0;
}
