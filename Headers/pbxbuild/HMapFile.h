//===--- HeaderMap.cpp - A file that acts like dir of symlinks ------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the HeaderMap interface.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Data Structures and Manifest Constants
//===----------------------------------------------------------------------===//

enum {
  HMAP_HeaderMagicNumber = ('h' << 24) | ('m' << 16) | ('a' << 8) | 'p',
  HMAP_HeaderVersion = 1,

  HMAP_EmptyBucketKey = 0
};

struct HMapBucket {
  uint32_t Key;          // Offset (into strings) of key.

  uint32_t Prefix;     // Offset (into strings) of value prefix.
  uint32_t Suffix;     // Offset (into strings) of value suffix.
};

struct HMapHeader {
  uint32_t Magic;           // Magic word, also indicates byte order.
  uint16_t Version;         // Version number -- currently 1.
  uint16_t Reserved;        // Reserved for future use - zero for now.
  uint32_t StringsOffset;   // Offset to start of string pool.
  uint32_t NumEntries;      // Number of entries in the string table.
  uint32_t NumBuckets;      // Number of buckets (always a power of 2).
  uint32_t MaxValueLength;  // Length of longest result path (excluding nul).
  // An array of 'NumBuckets' HMapBucket objects follows this header.
  // Strings follow the buckets, at StringsOffset.
};
