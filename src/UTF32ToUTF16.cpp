#include "CharConverter.hpp"

CharConverter::Result
CharConverter::cvtUTF32ToUTF16(char32_t code_point, char16_t (&dst)[2],
                               std::size_t *out_utf16_len) {
  std::size_t utf16_len = 0;
  dst[0] = 0;
  dst[1] = 0;
  // サロゲート領域 (0xD800〜0xDFFF) 自体 と 0x10FFFF 超えは不正
  if ((code_point >= 0xD800 && code_point <= 0xDFFF) || code_point > 0x10FFFF) {
    if (out_utf16_len != nullptr) {
      *out_utf16_len = 0;
    }
    return CharConverter::Result::Invalid;
  }

  if (code_point <= 0xFFFF) {
    // サロゲートでない
    dst[0] = static_cast<char16_t>(code_point);
    utf16_len = 1;
  } else if (0x10000 <= code_point && code_point <= 0x10FFFF) {
    // 10000000000000000 ~ 100001111111111111111
    // サロゲート
    char32_t tmp = code_point - 0x10000;
    char32_t high = static_cast<char16_t>(
        (tmp >> 10) + 0xD800); // 0x400 で割って 0xD800 を足す
    char16_t low = static_cast<char16_t>(
        (tmp & 0x3FF) + 0xDC00); // 0x400 で割った余りに 0xDC00 を足す
    dst[0] = high;
    dst[1] = low;
    utf16_len = 2;
  }

  if (out_utf16_len != nullptr) {
    *out_utf16_len = utf16_len;
  }

  return CharConverter::Result::Success;
}
