/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */
/** \file
 * A header file that defines various enums
 * and a class that stores all the different parameters used during the execution.
 */
#pragma once
#include <cstddef>
#include <string>

#ifdef WITH_SZ
#undef SZ
#endif

namespace htf {
/** A set of various compression algorithms supported by HTF.*/
enum class CompressionAlgorithm {
  /** No Compression.*/
  None = 0,
  /** Compression using ZSTD (lossless). */
  ZSTD = 1,
  /**Compression using Histogram (lossy). */
  Histogram = 2,
#ifdef WITH_SZ
  /** Compression using SZ (lossy). */
  SZ = 3,
#endif
#ifdef WITH_ZFP
  /**Compression using ZFP (lossy). */
  ZFP = 4
#endif
};

/**
 * @brief Converts a compression algorithm to its string name.
 * @param alg Algorithm to compress.
 * @return String such that it shall be parsed to that algorithm's enum.
 */
inline std::string algorithmToString(CompressionAlgorithm alg) {
  switch (alg) {
  case CompressionAlgorithm::None:
    return "None";
  case CompressionAlgorithm::ZSTD:
    return "ZSTD";
  case CompressionAlgorithm::Histogram:
    return "Histogram";
#ifdef WITH_SZ
  case CompressionAlgorithm::SZ:
    return "SZ";
#endif
#ifdef WITH_ZFP
  case CompressionAlgorithm::ZFP:
    return "ZFP";
#endif
  }
}
/** Returns whether a compression algorithm is lossy or not. */
inline bool isLossy(CompressionAlgorithm alg) {
  return alg != CompressionAlgorithm::None && alg != CompressionAlgorithm::ZSTD;
}
/** A set of various encoding algorithms supported by HTF */
enum class EncodingAlgorithm {
  /** No encoding. */
  None,
  /** Masking encoding: the first byte of an array indicates the size of the rest of the elements in the array.
   * This is done to reduce the number of leading zeroes.*/
  Masking,
  /** LeadingZeroes encoding: the first byte of each element in the array indicates the size of that element.
   * This is done to nullify the number of leading zeroes */
  LeadingZeroes
};

/**
 * @brief Converts a compression algorithm to its string name.
 * @param alg Algorithm to compress.
 * @return String such that it shall be parsed to that algorithm's enum.
 */
inline std::string algorithmToString(EncodingAlgorithm alg) {
  switch (alg) {
  case EncodingAlgorithm::None:
    return "None";
  case EncodingAlgorithm::Masking:
    return "Masking";
  case EncodingAlgorithm::LeadingZeroes:
    return "LeadingZeroes";
  }
}

/** A set of various loop-finding algorithms used by HTF */
enum class LoopFindingAlgorithm {
  /** No loop finding */
  None,
  /** Basic, quadratic loop finding algorithm. */
  Basic,
  /** Basic, quadratic loop finding algorithm.
   * The algorithm doesn't search for any pattern longer than ParameterHandler::maxLoopLength */
  BasicTruncated,
  /** Start by filtering the tokens and only running the loop finding algorithm on the interesting ones.
   * See ThreadWriter::findLoopFilter for more information.
   */
  Filter
};

/**
 * @brief Converts a compression algorithm to its string name.
 * @param alg Algorithm to compress.
 * @return String such that it shall be parsed to that algorithm's enum.
 */
inline std::string algorithmToString(LoopFindingAlgorithm alg) {
  switch (alg) {
  case LoopFindingAlgorithm::None:
    return "None";
  case LoopFindingAlgorithm::Basic:
    return "Basic";
  case LoopFindingAlgorithm::BasicTruncated:
    return "BasicTruncated";
  case LoopFindingAlgorithm::Filter:
    return "Filter";
  }
}

/**
 * @brief A simple data class that contains information on different parameters.
 */
class ParameterHandler {
  /** The compression algorithm used during the execution. */
  CompressionAlgorithm compressionAlgorithm{CompressionAlgorithm::None};
  /** The ZSTD compression level. */
  size_t zstdCompressionLevel{3};
  /** The encoding algorithm used during the execution. */
  EncodingAlgorithm encodingAlgorithm{EncodingAlgorithm::None};
  /** The compression algorithm used during the execution. */
  LoopFindingAlgorithm loopFindingAlgorithm{LoopFindingAlgorithm::BasicTruncated};
  /** The max length the LoopFindingAlgorithm::BasicTruncated will go to.*/
  size_t maxLoopLength{100};

 public:
  /** Getter for #maxLoopLength. Error if you're not supposed to have a maximum loop length.
   * @returns Value of #maxLoopLength. */
  [[nodiscard]] size_t getMaxLoopLength() const;
  /** Getter for #zstdCompressionLevel. Error if you're not using ZSTD.
   * @returns Value of #zstdCompressionLevel. */
  [[nodiscard]] u_int8_t getZstdCompressionLevel() const;
  /** Getter for #compressionAlgorithm.
   * @returns Value of #compressionAlgorithm. */
  [[nodiscard]] CompressionAlgorithm getCompressionAlgorithm() const;
  /**
   * Getter for #encodingAlgorithm.
   * @returns Value of #encodingAlgorithm. If the #compressionAlgorithm is lossy, returns EncodingAlgorithm::None. */
  [[nodiscard]] EncodingAlgorithm getEncodingAlgorithm() const;
  /**
   * Getter for #loopFindingAlgorithm.
   * @returns Value of #loopFindingAlgorithm.
   */
  [[nodiscard]] LoopFindingAlgorithm getLoopFindingAlgorithm() const;
  /** @brief Creates a ParameterHandler from a config file loaded from CONFIG_FILE_PATH or config.json.
   */
  ParameterHandler();
  [[nodiscard]] std::string to_string() const;
};

/** Global ParameterHandler. This is supposed to be the only instance of that class. */
extern const ParameterHandler parameterHandler;
}  // namespace htf
