//
// Created by khatharsis on 24/10/23.
//

#pragma once

#include <cstddef>
#include <string>
namespace htf {
/** A set of various compression algorithms supported by HTF.*/
enum CompressionAlgorithm {
  /** No Compression.*/
  CompressionNone,
  /** Compression using ZSTD (lossless). */
  CompressionZSTD,
  /** Compression using SZ (lossy). */
  CompressionSZ,
  /**Compression using ZFP (lossy). */
  CompressionZFP
};
/** Returns whether a compression algorithm is lossy or not. */
inline bool isLossy(CompressionAlgorithm alg) {
  return alg == CompressionSZ || alg == CompressionZFP;
}
/** A set of various encoding algorithms supported by HTF */
enum EncodingAlgorithm {
  /** No encoding. */
  EncodingNone,
  /** Masking encoding: the first byte of an array indicates the size of the rest of the elements in the array.
   * This is done to reduce the number of leading zeroes.*/
  EncodingMasking,
  /** LeadingZeroes encoding: the first byte of each element in the array indicates the size of that element.
   * This is done to nullify the number of leading zeroes */
  EncodingLeadingZeroes
};

/** A set of various loop-finding algorithms used by HTF */
enum LoopFindingAlgorithm {
  /** No loop finding */
  LoopFindingNone,
  /** Basic, quadratic loop finding algorithm. */
  LoopFindingBasic,
  /** Basic, quadratic loop finding algorithm.
   * The algorithm doesn't search for any pattern longer than parameterHandler::maxLoopLength */
  LoopFindingBasicTruncated
};

class ParameterHandler {
  CompressionAlgorithm compressionAlgorithm{CompressionNone};
  size_t zstdCompressionLevel{1};
  EncodingAlgorithm encodingAlgorithm{EncodingNone};
  LoopFindingAlgorithm loopFindingAlgorithm{LoopFindingBasicTruncated};
  size_t maxLoopLength{100};

 public:
  [[nodiscard]] size_t getMaxLoopLength() const;
  [[nodiscard]] u_int8_t getZstdCompressionLevel() const;
  [[nodiscard]] CompressionAlgorithm getCompressionAlgorithm() const;
  [[nodiscard]] EncodingAlgorithm getEncodingAlgorithm() const;
  [[nodiscard]] LoopFindingAlgorithm getLoopFindingAlgorithm() const;
  explicit ParameterHandler(const std::string& configFileName);
  ParameterHandler() = default;
};

extern const ParameterHandler parameterHandler;
}  // namespace htf
