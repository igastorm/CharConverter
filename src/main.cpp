#include "CharConverter.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <ostream>
#include <string>

constexpr std::size_t max_size = 4;

int main() {
  std::cout << "UTF-8文字を1文字入力してください: ";

  std::string line;
  if (!std::getline(std::cin, line) || line.empty()) {
    std::cerr << "入力がありません\n";
    return 1;
  }

  // UTF-8の最大1文字分だけをchar8_t配列へコピー
  const std::size_t copy_size = line.size() > max_size ? max_size : line.size();

  char8_t input[max_size + 1] = {};
  std::memcpy(input, line.data(), copy_size);
  input[copy_size] = '\0';

  std::size_t consumed = 0;
  char32_t utf32 = 0;

  CharConverter::cvtUTF8ToUTF32(input, copy_size, &consumed, &utf32);

  if (consumed == 0 || consumed > max_size) {
    std::cerr << "不正なUTF-8文字です\n";
    return 1;
  }

  std::cout << "入力した文字 (" << consumed << "バイト): ";

  std::cout << input << std::endl;

  std::cout << std::hex;
  std::cout << "UTF-32コード: 0x" << static_cast<std::uint32_t>(utf32) << '\n';

  char16_t utf16[2] = {};
  std::size_t utf16_size = 0;

  CharConverter::cvtUTF32ToUTF16(utf32, utf16, &utf16_size);

  std::cout << "UTF-16コード: 0x" << static_cast<std::uint16_t>(utf16[0]);

  if (utf16_size == 2) {
    std::cout << ", 0x" << static_cast<std::uint16_t>(utf16[1])
              << " (サロゲート)";
  }

  std::cout << std::endl;

  return 0;
}
