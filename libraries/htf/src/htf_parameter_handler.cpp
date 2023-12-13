/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */

#include <json/json.h>
#include <json/value.h>
#include <fstream>
#include <iostream>
#include "htf/htf_dbg.h"
#include "htf/htf_parameter_handler.h"

std::string loadStringFromConfig(Json::Value &config, std::string fieldName) {
  if(config[fieldName]) {
    if (config[fieldName].isString()) {
      return config[fieldName].asString();
    }
  }
  return "";
}

uint64_t loadUInt64FromConfig(Json::Value &config, std::string fieldName) {
  if(config[fieldName]) {
    if (config[fieldName].isUInt64()) {
      return config[fieldName].asUInt64();
    }
  }
  return UINT64_MAX;
}

std::string loadStringFromEnv(Json::Value &config, std::string envName) {
  const char* env_value = getenv(envName.c_str());
  if(env_value)
    return std::string(env_value);
  return "";
}

uint64_t loadUInt64FromEnv(Json::Value &config, std::string envName) {
  const char* env_value = getenv(envName.c_str());
  if(env_value) {
    return std::stoull(env_value);
  }
  return UINT64_MAX;
}

std::map<htf::CompressionAlgorithm,std::string> CompressionAlgorithmMap = {
  {htf::CompressionAlgorithm::None, "None"},
  {htf::CompressionAlgorithm::ZSTD, "ZSTD"},
  {htf::CompressionAlgorithm::Histogram, "Histogram"},
#ifdef WITH_SZ
  {htf::CompressionAlgorithm::SZ, "SZ"},
#endif
#ifdef WITH_ZFP
  {htf::CompressionAlgorithm::ZFP, "ZFP"},
#endif
  {htf::CompressionAlgorithm::Invalid, "Invalid"},
};

std::string htf::toString(htf::CompressionAlgorithm alg) {
  return CompressionAlgorithmMap[alg];
}

htf::CompressionAlgorithm compressionAlgorithmFromString(std::string str) {
  for(auto &it : CompressionAlgorithmMap) {
    if(it.second.compare(str)==0) {
      return it.first;
    }
  }
  return htf::CompressionAlgorithm::Invalid;
}

htf::CompressionAlgorithm loadCompressionAlgorithmConfig(Json::Value &config) {
  htf::CompressionAlgorithm ret = htf::CompressionAlgorithm::None;

  std::string value = loadStringFromEnv(config, "HTF_COMPRESSION");
  if(value.empty()) {
    value = loadStringFromConfig(config, "compressionAlgorithm");
  }
  ret = compressionAlgorithmFromString(value);

  return ret;
}



std::map<htf::EncodingAlgorithm,std::string> EncodingAlgorithmMap = {
  {htf::EncodingAlgorithm::None, "None"},
  {htf::EncodingAlgorithm::Masking, "Masking"},
  {htf::EncodingAlgorithm::LeadingZeroes, "LeadingZeroes"},
  {htf::EncodingAlgorithm::Invalid, "Invalid"},
};

std::string htf::toString(htf::EncodingAlgorithm alg) {
  return EncodingAlgorithmMap[alg];
}

htf::EncodingAlgorithm encodingAlgorithmFromString(std::string str) {
  for(auto &it : EncodingAlgorithmMap) {
    if(it.second.compare(str)==0) {
      return it.first;
    }
  }
  return htf::EncodingAlgorithm::Invalid;
}

htf::EncodingAlgorithm loadEncodingAlgorithmConfig(Json::Value &config) {
  htf::EncodingAlgorithm ret = htf::EncodingAlgorithm::None;

  std::string value = loadStringFromEnv(config, "HTF_ENCODING");
  if(value.empty()) {
    value = loadStringFromConfig(config, "encodingAlgorithm");
  }  
  ret = encodingAlgorithmFromString(value);
  return ret;
}


std::map<htf::LoopFindingAlgorithm,std::string> LoopFindingAlgorithmMap = {
  {htf::LoopFindingAlgorithm::None, "None"},
  {htf::LoopFindingAlgorithm::Basic, "Basic"},
  {htf::LoopFindingAlgorithm::BasicTruncated, "BasicTruncated"},
  {htf::LoopFindingAlgorithm::Filter, "Filter"},
  {htf::LoopFindingAlgorithm::Invalid, "Invalid"},
};

std::string htf::toString(htf::LoopFindingAlgorithm alg) {
  return LoopFindingAlgorithmMap[alg];
}

htf::LoopFindingAlgorithm loopFindingAlgorithmFromString(std::string str) {
  for(auto &it : LoopFindingAlgorithmMap) {
    if(it.second.compare(str)==0) {
      return it.first;
    }
  }
  return htf::LoopFindingAlgorithm::Invalid;
}

htf::LoopFindingAlgorithm loadLoopFindingAlgorithmConfig(Json::Value &config) {
  htf::LoopFindingAlgorithm ret = htf::LoopFindingAlgorithm::BasicTruncated;

  std::string value = loadStringFromEnv(config, "HTF_LOOP_FINDING");
  if(value.empty()) {
    value = loadStringFromConfig(config, "loopFindingAlgorithm");
  }  
  ret = loopFindingAlgorithmFromString(value);
  return ret;
}

uint64_t loadMaxLoopLength(Json::Value &config) {
  uint64_t ret = 100;

  uint64_t value = loadUInt64FromEnv(config, "HTF_LOOP_LENGTH");
  if(value == UINT64_MAX) {
    value = loadUInt64FromConfig(config, "maxLoopLength");
  }  

  return value;
}


uint64_t loadZSTDCompressionLevel(Json::Value &config) {
  uint64_t ret = 3;

  uint64_t value = loadUInt64FromEnv(config, "HTF_ZSTD_LVL");
  if(value == UINT64_MAX) {
    value = loadUInt64FromConfig(config, "zstdCompressionLevel");
  }

  return value;
}



std::map<htf::TimestampStorage,std::string> TimestampStorageMap = {
  { htf::TimestampStorage::None, "None"},
  { htf::TimestampStorage::Delta, "Delta"},
  { htf::TimestampStorage::Timestamp, "Timestamp"},
  { htf::TimestampStorage::Invalid, "Invalid"}
};

std::string htf::toString(htf::TimestampStorage ts) {
  return TimestampStorageMap[ts];
}

htf::TimestampStorage timestampStorageFromString(std::string str) {
  for(auto &it : TimestampStorageMap) {
    if(it.second.compare(str)==0) {
      return it.first;
    }
  }
  return htf::TimestampStorage::Invalid;
}

htf::TimestampStorage loadTimestampStorageConfig(Json::Value &config) {
  htf::TimestampStorage ret = htf::TimestampStorage::Delta;

  std::string value = loadStringFromEnv(config, "HTF_TIMESTAMP_STORAGE");
  if(value.empty()) {
    value = loadStringFromConfig(config, "timestampStorage");
  }  
  ret = timestampStorageFromString(value);
  return ret;
}

namespace htf {
const char* defaultPath = "config.json";
const ParameterHandler parameterHandler = ParameterHandler();

ParameterHandler::ParameterHandler() {
  htf_debug_level_init();

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
    htf_warn("Using the default one: %s\n", to_string().c_str());
    return;
  }

  Json::Value config;
  configFile >> config;
  configFile.close();
  /* Load from file */

  compressionAlgorithm = loadCompressionAlgorithmConfig(config);
  encodingAlgorithm = loadEncodingAlgorithmConfig(config);
  loopFindingAlgorithm = loadLoopFindingAlgorithmConfig(config);
  maxLoopLength = loadMaxLoopLength(config);
  zstdCompressionLevel = loadZSTDCompressionLevel(config);
  timestampStorage = loadTimestampStorageConfig(config);

  htf_log(htf::DebugLevel::Verbose, "%s\n", to_string().c_str());
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

TimestampStorage ParameterHandler::getTimestampStorage() const {
  return timestampStorage;
}

std::string ParameterHandler::to_string() const {
  std::stringstream stream("");
  stream << "{\n";
  stream << '\t' << R"("compressionAlgorithm": ")" << toString(compressionAlgorithm) << "\",\n";
  stream << '\t' << R"("encodingAlgorithm": ")" << toString(encodingAlgorithm) << "\",\n";
  stream << '\t' << R"("loopFindingAlgorithm": ")" << toString(loopFindingAlgorithm) << "\",\n";
  stream << '\t' << R"("maxLoopLength": )" << maxLoopLength << ",\n";
  stream << '\t' << R"("zstdCompressionLevel": )" << zstdCompressionLevel << ",\n";
  stream << '\t' << R"("timestampStorage": ")" << toString(timestampStorage) << "\",\n";
  stream << "}";
  return stream.str();
}

}  // namespace htf
