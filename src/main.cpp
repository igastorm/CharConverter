#include "CharConverter.hpp"
#include <cstdlib>
#include <cstring>
#include <iostream>
using std::cout, std::endl;

constexpr std::size_t max_size = 4;

int main(int argc, char **argv) {
  char8_t input[4];
  cout << "一文字入力 (UTF-8 専用): ";
  std::cin.getline(reinterpret_cast<char *>(input), sizeof(input));

  std::size_t consumed = 0;
  char32_t code_point = 0;
  CharConverter::cvtUTF8ToUTF32(input, max_size, &consumed, &code_point);

  std::memset(input + consumed, 0, max_size - consumed);
  cout << "入力した文字 (" << consumed << "バイト): ";
  cout << input << endl;

  cout << std::hex;
  cout << "UTF-32コード: 0x" << code_point << endl;

  return 0;
}
