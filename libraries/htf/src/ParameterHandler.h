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

/**
 * @brief A simple data class that contains information on different parameters.
 */
class ParameterHandler {
  /** The compression algorithm used during the execution. */
  CompressionAlgorithm compressionAlgorithm{CompressionNone};
  /** The ZSTD compression level. */
  size_t zstdCompressionLevel{1};
  /** The encoding algorithm used during the execution. */
  EncodingAlgorithm encodingAlgorithm{EncodingNone};
  /** The compression algorithm used during the execution. */
  LoopFindingAlgorithm loopFindingAlgorithm{LoopFindingBasicTruncated};
  /** The max length the ::LoopFindingBasicTruncated will go to.*/
  size_t maxLoopLength{100};

 public:
  /** Getter for maxLoopLength. Error if you're not supposed to have a maxLoopLength.
   * @returns Value of maxLoopLength. */
  [[nodiscard]] size_t getMaxLoopLength() const;
  /** Getter for zstdCompressionLevel. Error if you're not using ZSTD.
   * @returns Value of zstdCompressionLevel. */
  [[nodiscard]] u_int8_t getZstdCompressionLevel() const;
  /** Getter for compressionAlgorithm.
   * @returns Value of compressionAlgorithm. */
  [[nodiscard]] CompressionAlgorithm getCompressionAlgorithm() const;
  /**
   * Getter for encodingAlgorithm.
   * @returns Value of encodingAlgorithm. If the compressionAlgorithm is lossy, returns EncodingNone. */
  [[nodiscard]] EncodingAlgorithm getEncodingAlgorithm() const;
  /**
   * Getter for loopFindingAlgorithm.
   * @returns Value of loopFindingAlgorithm.
   */
  [[nodiscard]] LoopFindingAlgorithm getLoopFindingAlgorithm() const;
  /**
   * Creates a parameterHandler from a config file.
   * @param configFileName Path to a valid JSON config file.
   */
  explicit ParameterHandler(const std::string& configFileName);
  /**
   * Default constructor.
   */
  ParameterHandler() = default;
};

/** Global ParameterHandler. This is supposed to be the only instance of that class. */
extern const ParameterHandler parameterHandler;
}  // namespace htf
