#include "CharConverter.hpp"
// 以下をもとに実装
// https://ja.wikipedia.org/wiki/UTF-8
// https://ja.wikipedia.org/wiki/UTF-16
// https://ja.wikipedia.org/wiki/Unicode#サロゲートペア
// 一文字分専用

CharConverter::Result
CharConverter::cvtUTF8ToUTF32(const char8_t *src, std::size_t src_len,
                              std::size_t *consumed_src_bytes,
                              char32_t *out_code_point) {
  // 継続バイトかの判定
  // 文字の先頭ではなく, 前のバイトの続きであることを示す値
  // 2バイト目以降の下限から上限の範囲内か
  auto isContinuationByte = [](char8_t b) -> bool {
    // 10000000 ~ 10111111
    return (0x80 <= b && b <= 0xBF);
  };

  if (src == nullptr || src_len == 0 || out_code_point == nullptr ||
      consumed_src_bytes == nullptr) {
    return CharConverter::Result::Error;
  }

  std::size_t &i = *consumed_src_bytes;
  i = 0;

  char32_t &code_point = *out_code_point;
  code_point = 0;

  while (i < src_len) {
    // 各文字の先頭バイト
    char8_t b0 = src[i];

    // ASCII はそのまま
    // 0 ~ 01111111
    if (b0 <= 0x7F) {
      code_point = src[i];
      i++;
    } else if (0xC2 <= b0 && b0 <= 0xDF) {
      // 2バイトの UTF-8 (1バイト目はすでに b0 に入ってる)
      // 11000010 ~ 11011111
      char8_t b1 = 0;
      if (i + 1 >= src_len) {
        // 続きのデータがないならエラー
        // ただし後から続きを取得できるかも
        i = 0;
        return CharConverter::Result::Incomplete;
      }

      b1 = src[i + 1];
      if (!isContinuationByte(b1)) {
        // 2バイト目なのに前のバイトの続きじゃなかったらおかしい
        // 2バイト目に入るべき値の範囲外
        i++;
        return CharConverter::Result::Invalid;
      }
      // 識別ビットを削除して繋げる
      code_point = static_cast<char32_t>(b0 & 0x1F) << 6 |
                   static_cast<char32_t>(b1 & 0x3F);
      i += 2;
    } else if (0xE0 <= b0 && b0 <= 0xEF) {
      // 3バイトの UTF-8 (1バイト目はすでに b0 に入ってる)
      // 11100000 ~ 11101111
      char8_t b1 = 0, b2 = 0;
      if (i + 2 >= src_len) {
        // 続きのデータがないならエラー
        // ただし後から続きを取得できるかも
        i = 0;
        return CharConverter::Result::Incomplete;
      }

      b1 = src[i + 1];
      b2 = src[i + 2];
      if (!isContinuationByte(b1) || !isContinuationByte(b2)) {
        // 2バイト目以降なのに前のバイトの続きじゃなかったらおかしい
        // 2バイト目以降に入るべき値の範囲外
        i++;
        return CharConverter::Result::Invalid;
      }

      // UTF-16 で16ビットをはみ出す (サロゲートというらしい) 部分
      // UTF-32 への変換だとしてもそのまま UTF-8 にみられる場合は不正らしい
      // 0x10000 ~ 0x10FFFF の範囲である必要がある
      // 0xED 0xA0 ~
      if (b0 == 0xED && 0xA0 <= b1) {
        i++;
        return CharConverter::Result::Invalid;
      }

      // 0xE0 0x80 ~ 0x9F は本来1バイトの文字を3バイトで表してるから不正らしい
      if (b0 == 0xE0 && 0x80 <= b1 && b1 <= 0x9F) {
        i++;
        return CharConverter::Result::Invalid;
      }

      // 識別ビットを削除して繋げる
      code_point = static_cast<char32_t>(b0 & 0x0F) << 12 |
                   static_cast<char32_t>(b1 & 0x3F) << 6 |
                   static_cast<char32_t>(b2 & 0x3F);
      i += 3;
    } else if (b0 >= 0xF0 && b0 <= 0xF4) {
      // 4バイトの UTF-8 (1バイト目はすでに b0 に入ってる)
      // 11110000 ~ 11110100
      char32_t b1 = 0, b2 = 0, b3 = 0;
      if (i + 3 >= src_len) {
        // 続きのデータがないならエラー
        // ただし後から続きを取得できるかも
        i = 0;
        return CharConverter::Result::Incomplete;
      }

      b1 = src[i + 1];
      b2 = src[i + 2];
      b3 = src[i + 3];
      if (!isContinuationByte(b1) || !isContinuationByte(b2) ||
          !isContinuationByte(b3)) {
        // 2バイト目以降なのに前のバイトの続きじゃなかったらおかしい
        // 2バイト目以降に入るべき値の範囲外
        i++;
        return CharConverter::Result::Invalid;
      }

      // 0xF0 0x80 ~ 0x8F は不正らしい
      if (b0 == 0xF0 && 0x80 <= b1 && b1 <= 0x8F) {
        i++;
        return CharConverter::Result::Invalid;
      }

      // 0xF4 0x90 ~ は不正らしい
      if (b0 == 0xF4 && 0x90 <= b1) {
        i++;
        return CharConverter::Result::Invalid;
      }

      // 識別ビットを削除して繋げる
      code_point = static_cast<char32_t>(b0 & 0x07) << 18 |
                   static_cast<char32_t>(b1 & 0x3F) << 12 |
                   static_cast<char32_t>(b2 & 0x3F) << 6 |
                   static_cast<char32_t>(b3 & 0x3F);
      i += 4;
    } else {
      // その他は不正
      i++;
      return CharConverter::Result::Invalid;
    }

    // 一文字分の処理が完了したら即時抜ける
    break;
  }
  if (out_code_point != nullptr) {
    *out_code_point = code_point;
  }
  if (consumed_src_bytes != nullptr) {
    *consumed_src_bytes = i;
  }
  return CharConverter::Result::Success;
}
