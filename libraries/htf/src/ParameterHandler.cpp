//
// Created by khatharsis on 24/10/23.
//

#include "ParameterHandler.h"
#include <json/json.h>
#include <json/value.h>
#include <fstream>
#include <iostream>
#include "htf/htf_dbg.h"

/** Given a parameter and an enumValue, will assign the parameter's value to enumValue if the config file says so.*/
#define MATCH_ENUM(parameterName, enumName, enumValue) \
  if (config[#parameterName] == #enumValue) {          \
    parameterName = enumName::enumValue;               \
    goto parameterName##LoadingIfStatement;            \
  }
/** Small macro to load a field that is supposed to be an enum, have some code that does the matching,
 * and then throw a warning if nothing has been matched. */
#define LOAD_FIELD_ENUM(parameterName, loadingCode)                                                              \
  if (config[#parameterName]) {                                                                                  \
    if (config[#parameterName].isString()) {                                                                     \
      loadingCode;                                                                                               \
    }                                                                                                            \
    htf_warn("Parameter in \"" #parameterName "\" field was invalid: %s\n", config[#parameterName].asCString()); \
    parameterName##LoadingIfStatement :;                                                                         \
  }
/** Small macro to load a field that's supposed to be a UINT64, and throw a warning in case something goes wrong. */
#define LOAD_FIELD_UINT64(parameterName)                                                                           \
  if (config[#parameterName]) {                                                                                    \
    if (config[#parameterName].isUInt64()) {                                                                       \
      parameterName = config[#parameterName].asUInt64();                                                           \
    } else {                                                                                                       \
      htf_warn("Parameter in \"" #parameterName "\" field was invalid: %s\n", config[#parameterName].asCString()); \
    }                                                                                                              \
  }

/** Small macro that is used when getting parameters from environment variables.*/
#define GET_ENV_FIELD(parameterName, enumName, enumSpecific) \
  if (parameterName##String == #enumSpecific)                \
  parameterName##Algorithm = enumName##Algorithm::enumSpecific

namespace htf {
const char* defaultPath = "config.json";
const ParameterHandler parameterHandler = ParameterHandler();

ParameterHandler::ParameterHandler() {
  std::ifstream configFile;
  const char* possibleConfigFileName = getenv("CONFIG_FILE_PATH");
  if (!possibleConfigFileName) {
    htf_warn("No config file provided, using default: %s\n", defaultPath);
    possibleConfigFileName = defaultPath;
  }
  htf_log(DebugLevel::Debug, "Loading configuration file from %s\n", possibleConfigFileName);
  configFile.open(possibleConfigFileName);
  if (!configFile.good()) {
    htf_warn("Config file didn't exist: %s.\n", possibleConfigFileName);
    return;
  }
  Json::Value config;
  configFile >> config;
  configFile.close();
  std::cout << config << std::endl;
  /* Load from file */
#define MATCH_COMPRESSION_ENUM(value) MATCH_ENUM(compressionAlgorithm, CompressionAlgorithm, value)
  LOAD_FIELD_ENUM(compressionAlgorithm, {
    MATCH_COMPRESSION_ENUM(None);
    MATCH_COMPRESSION_ENUM(ZSTD);
#ifdef WITH_SZ
    MATCH_COMPRESSION_ENUM(SZ);
#endif
#ifdef WITH_ZFP
    MATCH_COMPRESSION_ENUM(ZFP);
#endif
    MATCH_COMPRESSION_ENUM(Histogram);
  });

#define MATCH_ENCODING_ENUM(value) MATCH_ENUM(encodingAlgorithm, EncodingAlgorithm, value)
  LOAD_FIELD_ENUM(encodingAlgorithm, {
    MATCH_ENCODING_ENUM(None);
    MATCH_ENCODING_ENUM(Masking);
    MATCH_ENCODING_ENUM(LeadingZeroes);
  });

#define MATCH_LOOP_FINDING_ENUM(value) MATCH_ENUM(loopFindingAlgorithm, LoopFindingAlgorithm, value)
  LOAD_FIELD_ENUM(loopFindingAlgorithm, {
    MATCH_LOOP_FINDING_ENUM(None);
    MATCH_LOOP_FINDING_ENUM(Basic);
    MATCH_LOOP_FINDING_ENUM(BasicTruncated);
    MATCH_LOOP_FINDING_ENUM(Filter);
  });
  LOAD_FIELD_UINT64(maxLoopLength);
  LOAD_FIELD_UINT64(zstdCompressionLevel);

  /* Override from Environment Variables */

#define GET_COMP_FIELD(value) GET_ENV_FIELD(compression, Compression, value)
  char* compressionChar = std::getenv("HTF_COMPRESSION");
  if (compressionChar) {
    std::string compressionString = compressionChar;
    GET_COMP_FIELD(None);
    GET_COMP_FIELD(ZSTD);
#ifdef WITH_SZ
    GET_COMP_FIELD(SZ);
#endif
#ifdef WITH_ZFP
    GET_COMP_FIELD(ZFP);
#endif
    GET_COMP_FIELD(Histogram);
  }

#define GET_ENCO_FIELD(value) GET_ENV_FIELD(encoding, Encoding, value)
  char* encodingChar = std::getenv("HTF_ENCODING");
  if (encodingChar) {
    std::string encodingString = encodingChar;
    GET_ENCO_FIELD(None);
    GET_ENCO_FIELD(Masking);
    GET_ENCO_FIELD(LeadingZeroes);
  }

#define GET_LOOP_FIELD(value) GET_ENV_FIELD(loopFinding, LoopFinding, value)
  char* loopFindingChar = std::getenv("HTF_LOOP_FINDING");
  if (loopFindingChar) {
    std::string loopFindingString = loopFindingChar;
    GET_LOOP_FIELD(None);
    GET_LOOP_FIELD(Basic);
    GET_LOOP_FIELD(BasicTruncated);
  }

  char* zstdLevelChar = std::getenv("HTF_ZSTD_LVL");
  if (zstdLevelChar) {
    zstdCompressionLevel = std::stoull(zstdLevelChar);
  }

  char* loopLengthChar = std::getenv("HTF_LOOP_LENGTH");
  if (loopLengthChar) {
    maxLoopLength = std::stoull(loopLengthChar);
  }
}

size_t ParameterHandler::getMaxLoopLength() const {
  if (loopFindingAlgorithm == LoopFindingAlgorithm::BasicTruncated)
    return maxLoopLength;
  htf_error("Asked for the max loop length but wasn't using a LoopFindingBasicTruncated algorithm.\n");
}
u_int8_t ParameterHandler::getZstdCompressionLevel() const {
  if (compressionAlgorithm == CompressionAlgorithm::ZSTD) {
    return zstdCompressionLevel;
  }
  htf_error("Asked for ZSTD Compression Level but wasn't using a CompressionZSTD algorithm.\n");
}
CompressionAlgorithm ParameterHandler::getCompressionAlgorithm() const {
  return compressionAlgorithm;
}
EncodingAlgorithm ParameterHandler::getEncodingAlgorithm() const {
  if (isLossy(compressionAlgorithm) && encodingAlgorithm != EncodingAlgorithm::None) {
    htf_warn("Encoding algorithm isn't None even though the compression algorithm is lossy.\n");
    return EncodingAlgorithm::None;
  }
  return encodingAlgorithm;
}
LoopFindingAlgorithm ParameterHandler::getLoopFindingAlgorithm() const {
  return loopFindingAlgorithm;
}

}  // namespace htf