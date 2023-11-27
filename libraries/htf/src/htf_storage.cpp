/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */

#include <libgen.h>
#include <sys/stat.h>
#include <unistd.h>
#include <zstd.h>
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>

#ifdef WITH_ZFP
#include <zfp.h>
#endif
#ifdef WITH_SZ
#include <sz.h>
#endif

#include "ParameterHandler.h"
#include "htf/htf.h"
#include "htf/htf_dbg.h"
#include "htf/htf_hash.h"
#include "htf/htf_read.h"
#include "htf/htf_storage.h"

short STORE_TIMESTAMPS = 1;
static short STORE_HASHING = 0;
void htf_storage_option_init() {
  // Timestamp storage
  char* store_timestamps_str = getenv("STORE_TIMESTAMPS");
  if (store_timestamps_str && strcmp(store_timestamps_str, "TRUE") != 0)
    STORE_TIMESTAMPS = 0;

  // Store hash for sequences
  char* store_hashing_str = getenv("STORE_HASHING");
  if (store_hashing_str && strcmp(store_hashing_str, "FALSE") != 0)
    STORE_HASHING = 1;
}
static void _htf_store_event(const char* base_dirname, htf::Thread* th, htf::EventSummary* e, htf::Token event);
static void _htf_store_sequence(const char* base_dirname, htf::Thread* th, htf::Sequence* s, htf::Token sequence);
static void _htf_store_loop(const char* base_dirname, htf::Thread* th, struct htf_loop* l, htf::Token loop);

static void _htf_store_string(htf::Archive* c, htf::String* l, int string_index);
static void _htf_store_regions(htf::Archive* c);
static void _htf_store_attributes(htf::Archive* c);

static void _htf_store_location_groups(htf::Archive* a);
static void _htf_store_locations(htf::Archive* a);

static void _htf_read_event(const char* base_dirname, htf::Thread* th, htf::EventSummary* e, htf::Token event);
static void _htf_read_sequence(const char* base_dirname, htf::Thread* th, htf::Sequence* s, htf::Token sequence);
static void _htf_read_loop(const char* base_dirname, htf::Thread* th, struct htf_loop* l, htf::Token loop);

static void _htf_read_string(htf::Archive* c, htf::String* l, int string_index);
static void _htf_read_regions(htf::Archive* c);
static void _htf_read_attributes(htf::Archive* c);
static void _htf_read_location_groups(htf::Archive* a);
static void _htf_read_locations(htf::Archive* a);

void htf_read_thread(htf::Archive* archive, htf::ThreadId thread_id);

static htf::Archive* _htf_get_archive(htf::Archive* global_archive, htf::LocationGroupId archive_id);

static void _htf_mkdir(const char* dirname, mode_t mode) {
  if (mkdir(dirname, mode) != 0) {
    if (errno != EEXIST)
      htf_error("mkdir(%s) failed: %s\n", dirname, strerror(errno));
  }
}

static FILE* _htf_file_open(const char* filename, const char* mode) {
  htf_log(htf::DebugLevel::Debug, "Open %s with mode %s\n", filename, mode);
  char* filename_copy = strdup(filename);
  _htf_mkdir(dirname(filename_copy), 0777);
  free(filename_copy);

  FILE* file = fopen(filename, mode);
  if (file == nullptr) {
    htf_error("Cannot open %s: %s\n", filename, strerror(errno));
  }
  return file;
}

#define _htf_fread(ptr, size, nmemb, stream)            \
  do {                                                  \
    size_t ret = fread(ptr, size, nmemb, stream);       \
    if (ret != nmemb)                                   \
      htf_error("fread failed: %s\n", strerror(errno)); \
  } while (0)

#define _htf_fwrite(ptr, size, nmemb, stream)      \
  do {                                             \
    size_t ret = fwrite(ptr, size, nmemb, stream); \
    if (ret != nmemb)                              \
      htf_error("fwrite failed\n");                \
  } while (0)

/******************* Read/Write/Compression function for vectors and arrays *******************/

/** @brief Compresses the content in src using ZSTD and writes it to dest. Returns the amount of data written.
 *  @param src The source array.
 *  @param size Size of the source array.
 *  @param dest A free array in which the compressed data will be written.
 *  @param destSize Size of the destination array
 *  @returns Number of bytes written in the dest array.
 */
inline static size_t _htf_zstd_compress(void* src, size_t size, void* dest, size_t destSize) {
  return ZSTD_compress(dest, destSize, src, size, htf::parameterHandler.getZstdCompressionLevel());
}

/**
 * Decompresses an array that has been compressed by ZSTD. Returns the size of the uncompressed data.
 * @param realSize Size of the uncompressed data.
 * @param compArray The compressed array.
 * @param compSize Size of the compressed array.
 * @returns Uncompressed array.
 */
inline static uint64_t* _htf_zstd_read(size_t& realSize, void* compArray, size_t compSize) {
  realSize = ZSTD_getFrameContentSize(compArray, compSize);
  auto dest = new byte[realSize];
  ZSTD_decompress(dest, realSize, compArray, compSize);
  return reinterpret_cast<uint64_t*>(dest);
}

#ifdef WITH_ZFP
/**
 * @brief Gives a conservative upper bound for the size of the compressed data.
 * @param src The source array.
 * @param n Number of items in the array.
 * @return Upper bound to compressed array size in bytes.
 */
inline static size_t _htf_zfp_bound(uint64_t* src, size_t n) {
  zfp_type type = zfp_type_int64;                 // array scalar type
  zfp_field* field = zfp_field_1d(src, type, n);  // array metadata
  zfp_stream* zfp = zfp_stream_open(nullptr);     // compressed stream and parameters
  zfp_stream_set_accuracy(zfp, .1);               // set tolerance for fixed-accuracy mode, this is absolute error
  size_t bufsize = zfp_stream_maximum_size(zfp, field);
  zfp_stream_close(zfp);
  return bufsize;
}
/**
 * @brief Compresses the content in src using the 1D ZFP Algorithm and writes it to dest.
 * Returns the amounts of data written.
 * @param src The source array.
 * @param n Number of items in the source array.
 * @param dest A free array in which the compressed data will be written.
 * @param destSize Size of the destination array.
 * @return Number of bytes written in the dest array.
 */
inline static size_t _htf_zfp_compress(uint64_t* src, size_t n, void* dest, size_t destSize) {
  zfp_type type = zfp_type_int64;                 // array scalar type
  zfp_field* field = zfp_field_1d(src, type, n);  // array metadata
  zfp_stream* zfp = zfp_stream_open(nullptr);     // compressed stream and parameters
  zfp_stream_set_accuracy(zfp, .1);               // set tolerance for fixed-accuracy mode, this is absolute error
  size_t bufsize = zfp_stream_maximum_size(zfp, field);  // capacity of compressed buffer (conservative)
  htf_assert(bufsize <= destSize);
  bitstream* stream = stream_open(dest, bufsize);  // bit stream to compress to
  zfp_stream_set_bit_stream(zfp, stream);          // associate with compressed stream
  zfp_stream_rewind(zfp);                          // rewind stream to beginning
  size_t outSize = zfp_compress(zfp, field);       // return value is byte size of compressed stream
  zfp_stream_close(zfp);
  stream_close(stream);
  return outSize;
}

/**
 * @brief Decompresses the content in src using the 1D ZFP Algorithm and writes it to dest.
 * Returns the amounts of data written.
 * @param n Number of items that should be decompressed.
 * @param compressedArray The compressed array.
 * @param destSize Size of the compressed array.
 * @returns Uncompressed array of size uint64 * n.
 */
inline static uint64_t* _htf_zfp_decompress(size_t n, void* compressedArray, size_t compressedSize) {
  auto dest = new uint64_t[n];
  zfp_type type = zfp_type_int64;                  // array scalar type
  zfp_field* field = zfp_field_1d(dest, type, n);  // array metadata
  zfp_stream* zfp = zfp_stream_open(nullptr);      // compressed stream and parameters
  zfp_stream_set_accuracy(zfp, .1);                // set tolerance for fixed-accuracy mode, this is absolute error
  bitstream* stream = stream_open(compressedArray, compressedSize);  // bit stream to read from
  zfp_stream_set_bit_stream(zfp, stream);                            // associate with compressed stream
  zfp_stream_rewind(zfp);                                            // rewind stream to beginning
  size_t outSize = zfp_decompress(zfp, field);                       // return value is byte size of compressed stream
  zfp_stream_close(zfp);
  stream_close(stream);
  return dest;
}
#endif
#ifdef WITH_SZ
/**
 * @brief Compresses the content in src using the 1D SZ Algorithm.
 * @param src The source array.
 * @param n Number of items in the source array.
 * @param compressedSize Size of the compressed array. Passed by ref and modified.
 * @return The compressed array.
 */
inline static byte* _htf_sz_compress(uint64_t* src, size_t n, size_t& compressedSize) {
  SZ_Init(nullptr);
  byte* compressedArray = SZ_compress(SZ_UINT64, src, &compressedSize, 0, 0, 0, 0, n);
  SZ_Finalize();
  return compressedArray;
}

inline static uint64_t* _htf_sz_decompress(size_t n, byte* compressedArray, size_t compressedSize) {
  return static_cast<uint64_t*>(SZ_decompress(SZ_UINT64, compressedArray, compressedSize, 0, 0, 0, 0, n));
};

#endif

#define N_BYTES 1
#define N_BITS (N_BYTES * 8)
#define MAX_BIT ((1 << N_BITS) - 1)

/** @brief Compresses the content in src using the Histogram method and writes it to dest.
 * Returns the amount of data written.
 *  @param src The source array.
 *  @param n Number of elements in src.
 *  @param dest A free array in which the compressed data will be written.
 *  @param destSize Size of the destination array
 *  @returns Number of bytes written in the dest array.
 */
inline static size_t _htf_histogram_compress(const uint64_t* src, size_t n, byte* dest, size_t destSize) {
  htf_assert(destSize >= (N_BYTES * n + 2 * sizeof(uint64_t)));
  // Compute the min and max
  uint64_t min = UINT64_MAX, max = 0;
  for (size_t i = 0; i < n; i++) {
    min = (src[i] < min) ? src[i] : min;
    max = (src[i] > max) ? src[i] : max;
  }
  size_t width = max - min;
  size_t stepSize = ((double)width) / MAX_BIT;
  printf("Min: %lu; Max: %lu\n", min, max);
  // TODO Check size is sufficient
  memcpy(dest, &min, sizeof(min));
  dest = &dest[sizeof(min)];
  memcpy(dest, &max, sizeof(max));
  dest = &dest[sizeof(max)];
  for (size_t i = 0; i < n; i++) {
    auto temp = (size_t)std::round((src[i] - min) / stepSize);
    temp = (temp > MAX_BIT) ? MAX_BIT : temp;
    printf("Writing %lu as %lu\n", src[i], temp);
    memcpy(&dest[i * N_BYTES], &temp, N_BYTES);
  }

  return N_BYTES * n + 2 * sizeof(uint64_t);
}

/** @brief Decompresses the content in compArray using the Histogram method and writes it to dest.
 * Returns the amount of data written.
 * @param n Number of elements in the dest array.
 * @param compArray The compressed array.
 * @param compSize Size of the compressed array.
 * @returns Array of uncompressed data of size uint64_t * n.
 */
inline static uint64_t* _htf_histogram_read(size_t n, byte* compArray, size_t compSize) {
  auto dest = new uint64_t[n];
  // Compute the min and max
  uint64_t min, max;
  memcpy(&min, compArray, sizeof(min));
  compArray = &compArray[sizeof(min)];
  memcpy(&max, compArray, sizeof(max));
  compArray = &compArray[sizeof(max)];
  size_t width = max - min;
  size_t stepSize = width / (1 << N_BITS);

  printf("Min: %lu; Max: %lu\n", min, max);

  for (size_t i = 0; i < n; i++) {
    size_t factor = 0;
    memcpy(&factor, &compArray[i * N_BYTES], N_BYTES);
    dest[i] = min + factor * stepSize;
    printf("Reading %lu as %lu\n", factor, dest[i]);
  }
  return dest;
}

/**
 * Encodes the content in src using a Masking technique and writes it to dest.
 * This is done only for 64-bits values.
 * @param src The source array. Contains n elements of 8 bytes (sizeof uint64_t).
 * @param dest The destination array. Same size as src, is an uint8 for convenience (byte counting).
 * @param n Number of elements in source array.
 * @return Number of interesting bytes contained in dest. (0 <= nBytes <= n * sizeof uint64)
 */
inline static size_t _htf_masking_encode(const uint64_t* src, byte* dest, size_t n) {
  uint64_t mask = 0;
  for (int i = 0; i < n; i++) {
    mask |= src[i];
  }
  short maskSize = 0;
  while (mask != 0) {
    mask >>= 8;
    maskSize += 1;
  }
  // maskSize is the number of bytes needed to write the mask
  // ie the most amount of byte any number in src will need to be written
  if (maskSize && maskSize != sizeof(uint64_t)) {
    for (int i = 0; i < n; i++) {
      // FIXME This works because our LSB is in front (Small-endian)
      memcpy(&dest[maskSize * i], &src[i], maskSize);
    }
    return maskSize * n;
  } else {
    memcpy(dest, src, n * sizeof(uint64_t));
    return n * sizeof(uint64_t);
  }
}

/** De-encodes an array that has been compressed by the Masking technique. Returns the size of the unencoded data.
 * @param n Number of elements in the dest array.
 * @param encodedArray The encoded array.
 * @param encodedSize Size of the encoded array.
 * @returns Decoded array.
 */
inline static uint64_t* _htf_masking_read(size_t n, byte* encodedArray, size_t encodedSize) {
  auto dest = new uint64_t[n];
  size_t size = n * sizeof(uint64_t);
  if (encodedSize == size) {
    memcpy(dest, encodedArray, size);
    return dest;
  }
  size_t width = encodedSize / n;
  // width is the number of bytes needed to write an element in the encoded array.
  memset(dest, 0, size);
  for (int i = 0; i < n; i++) {
    // FIXME Still only works with Little-Endian architecture.
    memcpy(&dest[i], &encodedArray[width * i], width);
  }
  return dest;
}

size_t numberRawBytes = 0;
size_t numberCompressedBytes = 0;

/**
 * Writes the array to the given file, but encodes and compresses it before
 * according to the value of parameterHandler::EncodingAlgorithm and parameterHandler::CompressingAlgorithm.
 * @param src The source array. Contains n elements of 8 bytes (sizeof uint64_t).
 * @param n Number of elements in src.
 * @param file File to write in.
 */
inline static void _htf_compress_write(uint64_t* src, size_t n, FILE* file) {
  size_t size = n * sizeof(uint64_t);
  uint64_t* encodedArray = nullptr;
  size_t encodedSize;
  // First we do the encoding
  switch (htf::parameterHandler.getEncodingAlgorithm()) {
  case htf::EncodingAlgorithm::None:
    break;
  case htf::EncodingAlgorithm::Masking: {
    encodedArray = new uint64_t[n];
    encodedSize = _htf_masking_encode(src, (uint8_t*)encodedArray, n);
    break;
  }
  case htf::EncodingAlgorithm::LeadingZeroes: {
    htf_error("Not yet implemented\n");
    break;
  }
  }

  byte* compressedArray = nullptr;
  size_t compressedSize;
  switch (htf::parameterHandler.getCompressionAlgorithm()) {
  case htf::CompressionAlgorithm::None:
    break;
  case htf::CompressionAlgorithm::ZSTD: {
    compressedSize = ZSTD_compressBound(encodedArray ? encodedSize : size);
    compressedArray = new byte[compressedSize];
    if (encodedArray) {
      compressedSize = _htf_zstd_compress(encodedArray, encodedSize, compressedArray, compressedSize);
    } else {
      compressedSize = _htf_zstd_compress(src, size, compressedArray, compressedSize);
    }
    break;
  }
  case htf::CompressionAlgorithm::Histogram: {
    compressedSize = (n + 2) * sizeof(uint64_t);  // Take into account that we add the min and the max.
    compressedArray = new uint8_t[compressedSize];
    compressedSize = _htf_histogram_compress(src, n, compressedArray, compressedSize);
    break;
  }
#ifdef WITH_ZFP
  case htf::CompressionAlgorithm::ZFP:
    compressedSize = _htf_zfp_bound(src, n);
    compressedArray = new byte[compressedSize];
    compressedSize = _htf_zfp_compress(src, n, compressedArray, compressedSize);
    break;
#endif
#ifdef WITH_SZ
  case htf::CompressionAlgorithm::SZ:
    compressedArray = _htf_sz_compress(src, n, compressedSize);
    break;
#endif
  }

  if (htf::parameterHandler.getCompressionAlgorithm() != htf::CompressionAlgorithm::None) {
    htf_log(htf::DebugLevel::Normal, "Compressing %lu bytes as %lu bytes\n", size, compressedSize);
    _htf_fwrite(&compressedSize, sizeof(compressedSize), 1, file);
    _htf_fwrite(compressedArray, compressedSize, 1, file);
    numberRawBytes += size;
    numberCompressedBytes += compressedSize;
  } else if (htf::parameterHandler.getEncodingAlgorithm() != htf::EncodingAlgorithm::None) {
    htf_log(htf::DebugLevel::Normal, "Encoding %lu bytes as %lu bytes\n", size, encodedSize);
    _htf_fwrite(&encodedSize, sizeof(encodedSize), 1, file);
    _htf_fwrite(encodedArray, encodedSize, 1, file);
  } else {
    htf_log(htf::DebugLevel::Normal, "Writing %lu bytes as is.\n", size);
    _htf_fwrite(&size, sizeof(size), 1, file);
    _htf_fwrite(src, size, 1, file);
  }
  if (htf::parameterHandler.getCompressionAlgorithm() != htf::CompressionAlgorithm::None)
    delete[] compressedArray;
  if (htf::parameterHandler.getEncodingAlgorithm() != htf::EncodingAlgorithm::None)
    delete[] encodedArray;
}

/**
 * Reads, de-encodes and decompresses an array from the given file,
 * according to the values of parameterHandler::EncodingAlgorithm and parameterHandler::CompressingAlgorithm.
 * @param n Number of elements of 8 bytes dest is supposed to have.
 * @param file File to read from
 * @returns Array of uncompressed data of size uint64_t * n.
 */
inline static uint64_t* _htf_compress_read(size_t n, FILE* file) {
  size_t expectedSize = n * sizeof(uint64_t);
  uint64_t* uncompressedArray;

  size_t compressedSize;
  byte* compressedArray = nullptr;

  size_t encodedSize;
  byte* encodedArray = nullptr;

  auto compressionAlgorithm = htf::parameterHandler.getCompressionAlgorithm();
  auto encodingAlgorithm = htf::parameterHandler.getEncodingAlgorithm();
  if (compressionAlgorithm != htf::CompressionAlgorithm::None) {
    _htf_fread(&compressedSize, sizeof(compressedSize), 1, file);
    compressedArray = new byte[compressedSize];
    _htf_fread(compressedArray, compressedSize, 1, file);
  }

  switch (compressionAlgorithm) {
  case htf::CompressionAlgorithm::None:
    break;
  case htf::CompressionAlgorithm::ZSTD: {
    if (htf::parameterHandler.getEncodingAlgorithm() == htf::EncodingAlgorithm::None) {
      size_t uncompressedSize;
      uncompressedArray = _htf_zstd_read(uncompressedSize, compressedArray, compressedSize);
      htf_assert(uncompressedSize == expectedSize);
    } else {
      encodedArray = reinterpret_cast<byte*>(_htf_zstd_read(encodedSize, compressedArray, compressedSize));
      htf_assert(encodedSize <= expectedSize);
    }
    delete[] compressedArray;
    break;
  }
  case htf::CompressionAlgorithm::Histogram: {
    uncompressedArray = _htf_histogram_read(n, compressedArray, compressedSize);
    break;
  }
#ifdef WITH_ZFP
  case htf::CompressionAlgorithm::ZFP: {
    uncompressedArray = _htf_zfp_decompress(n, compressedArray, compressedSize);
    break;
  }
#endif
#ifdef WITH_SZ
  case htf::CompressionAlgorithm::SZ:
    uncompressedArray = _htf_sz_decompress(n, compressedArray, compressedSize);
    break;
#endif
  }

  switch (encodingAlgorithm) {
  case htf::EncodingAlgorithm::None:
    break;
  case htf::EncodingAlgorithm::Masking: {
    if (htf::parameterHandler.getCompressionAlgorithm() == htf::CompressionAlgorithm::None) {
      _htf_fread(&encodedSize, sizeof(encodedSize), 1, file);
      encodedArray = new byte[encodedSize];  // Too big but don't care
      _htf_fread(encodedArray, encodedSize, 1, file);
    }
    uncompressedArray = _htf_masking_read(n, encodedArray, encodedSize);
    delete[] encodedArray;
    break;
  }
  case htf::EncodingAlgorithm::LeadingZeroes: {
    htf_error("Not yet implemented\n");
    break;
  }
  }

  if (compressionAlgorithm == htf::CompressionAlgorithm::None && encodingAlgorithm == htf::EncodingAlgorithm::None) {
    size_t realSize;
    _htf_fread(&realSize, sizeof(realSize), 1, file);
    uncompressedArray = new uint64_t[n];
    _htf_fread(uncompressedArray, realSize, 1, file);
    htf_assert(realSize == n * sizeof(uint64_t));
  }
  return uncompressedArray;
}

void htf::LinkedVector::writeToFile(FILE* file, bool writeSize = true) const {
  if (writeSize) {
    _htf_fwrite(&size, sizeof(size), 1, file);
  }
  if (size == 0) {
    return;
  }
  auto* buffer = new uint64_t[size];
  uint cur_index = 0;
  SubVector* sub_vec = first;
  while (sub_vec) {
    sub_vec->copyToArray(&buffer[sub_vec->starting_index]);
    cur_index += sub_vec->size;
    sub_vec = sub_vec->next;
  }
  htf_assert(cur_index == size);
  _htf_compress_write(buffer, size, file);
  delete[] buffer;
}

htf::LinkedVector::LinkedVector(FILE* file, size_t givenSize) {
  size = givenSize;
  if (size) {
    auto temp = _htf_compress_read(size, file);
    last = new SubVector(size, temp);
    first = last;
    last->size = size;
  }
}

htf::LinkedVector::LinkedVector(FILE* file) {
  _htf_fread(&size, sizeof(size), 1, file);
  if (size) {
    auto temp = _htf_compress_read(size, file);
    last = new SubVector(size, temp);
    first = last;
    last->size = size;
  }
}
// TODO Find a way to delegate this ?

/**************** Storage Functions ****************/

void htf_storage_init(htf::Archive* archive) {
  _htf_mkdir(archive->dir_name, 0777);
  htf_storage_option_init();
}

static const char* base_dirname(htf::Archive* a) {
  return a->dir_name;
}

static FILE* _htf_get_event_file(const char* base_dirname, htf::Thread* th, htf::Token event, const char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/thread_%u/event_%d", base_dirname, th->id, event.id);
  return _htf_file_open(filename, mode);
}

static void _htf_store_attribute_values(htf::EventSummary* e, FILE* file) {
  _htf_fwrite(&e->attribute_pos, sizeof(e->attribute_pos), 1, file);
  if (e->attribute_pos > 0) {
    htf_log(htf::DebugLevel::Normal, "\t\tStore %lu attributes\n", e->attribute_pos);
    if (htf::parameterHandler.getCompressionAlgorithm() != htf::CompressionAlgorithm::None) {
      size_t compressedSize = ZSTD_compressBound(e->attribute_pos);
      byte* compressedArray = new byte[compressedSize];
      compressedSize = _htf_zstd_compress(e->attribute_buffer, e->attribute_pos, compressedArray, compressedSize);
      _htf_fwrite(&compressedSize, sizeof(compressedSize), 1, file);
      _htf_fwrite(compressedArray, compressedSize, 1, file);
      delete[] compressedArray;
    } else {
      _htf_fwrite(e->attribute_buffer, e->attribute_pos, 1, file);
    }
  }
}

static void _htf_read_attribute_values(htf::EventSummary* e, FILE* file) {
  _htf_fread(&e->attribute_pos, sizeof(e->attribute_pos), 1, file);
  e->attribute_buffer_size = e->attribute_pos;
  e->attribute_pos = 0;
  e->attribute_buffer = nullptr;

  if (e->attribute_buffer_size > 0) {
    e->attribute_buffer = new uint8_t[e->attribute_buffer_size];
    if (e->attribute_buffer == nullptr) {
      htf_error("Cannot allocate memory\n");
    }
    if (htf::parameterHandler.getCompressionAlgorithm() != htf::CompressionAlgorithm::None) {
      size_t compressedSize;
      _htf_fread(&compressedSize, sizeof(compressedSize), 1, file);
      byte* compressedArray = new byte[compressedSize];
      _htf_fread(e->attribute_buffer, compressedSize, 1, file);
      e->attribute_buffer =
        reinterpret_cast<uint8_t*>(_htf_zstd_read(e->attribute_buffer_size, compressedArray, compressedSize));
      delete[] compressedArray;
    } else {
      _htf_fread(e->attribute_buffer, e->attribute_buffer_size, 1, file);
    }
  }
}

static void _htf_store_event(const char* base_dirname, htf::Thread* th, htf::EventSummary* e, htf::Token event) {
  FILE* file = _htf_get_event_file(base_dirname, th, event, "w");
  htf_log(htf::DebugLevel::Debug, "\tStore event %x {.nb_events=%zu}\n", event.id, e->nb_occurences);

  _htf_fwrite(&e->event, sizeof(htf::Event), 1, file);
  _htf_fwrite(&e->nb_occurences, sizeof(e->nb_occurences), 1, file);
  _htf_store_attribute_values(e, file);
  if (STORE_TIMESTAMPS) {
    e->durations->writeToFile(file, false);
  }
  fclose(file);
}

static void _htf_read_event(const char* base_dirname, htf::Thread* th, htf::EventSummary* e, htf::Token event) {
  FILE* file = _htf_get_event_file(base_dirname, th, event, "r");

  _htf_fread(&e->event, sizeof(htf::Event), 1, file);
  _htf_fread(&e->nb_occurences, sizeof(e->nb_occurences), 1, file);
  htf_log(htf::DebugLevel::Debug, "\tLoad event %x {.nb_events=%zu}\n", event.id, e->nb_occurences);
  _htf_read_attribute_values(e, file);
  if (STORE_TIMESTAMPS) {
    e->durations = new htf::LinkedVector(file, e->nb_occurences);
  } else {
    e->durations->size = 0;
  }
  fclose(file);
}

static FILE* _htf_get_sequence_file(const char* base_dirname, htf::Thread* th, htf::Token sequence, const char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/thread_%u/sequence_%d", base_dirname, th->id, sequence.id);
  return _htf_file_open(filename, mode);
}

static void _htf_store_sequence(const char* base_dirname, htf::Thread* th, htf::Sequence* s, htf::Token sequence) {
  FILE* file = _htf_get_sequence_file(base_dirname, th, sequence, "w");
  htf_log(htf::DebugLevel::Debug, "\tStore sequence %x {.size=%zu, .nb_ts=%zu}\n", sequence.id, s->size(),
          s->durations->size);
  if (htf::debugLevel >= htf::DebugLevel::Debug) {
    htf_print_sequence(th, sequence);
  }
  size_t size = s->size();
  _htf_fwrite(&size, sizeof(size), 1, file);
  _htf_fwrite(s->tokens.data(), sizeof(s->tokens[0]), s->size(), file);
  if (STORE_HASHING) {
    if (!s->hash) {
      hash32(s->tokens.data(), s->size(), SEED, &s->hash);
    }
    _htf_fwrite(&s->hash, sizeof(s->hash), 1, file);
  }
  if (STORE_TIMESTAMPS) {
    s->durations->writeToFile(file);
  }
  fclose(file);
}

static void _htf_read_sequence(const char* base_dirname, htf::Thread* th, htf::Sequence* s, htf::Token sequence) {
  FILE* file = _htf_get_sequence_file(base_dirname, th, sequence, "r");
  size_t size;
  _htf_fread(&size, sizeof(size), 1, file);
  s->tokens.resize(size);
  _htf_fread(s->tokens.data(), sizeof(htf::Token), size, file);
  if (STORE_HASHING) {
    uint32_t stored_hash;
    _htf_fread(&stored_hash, sizeof(stored_hash), 1, file);
    hash32(s->tokens.data(), size, SEED, &s->hash);
    htf_assert(stored_hash == s->hash);
  }
  if (STORE_TIMESTAMPS) {
    s->durations = new htf::LinkedVector(file);
  }
  fclose(file);

  htf_log(htf::DebugLevel::Debug, "\tLoad sequence %x {.size=%zu, .nb_ts=%zu}\n", sequence.id, s->size(),
          s->durations->size);

  if (htf::debugLevel >= htf::DebugLevel::Debug) {
    htf_print_sequence(th, sequence);
  }
}

static FILE* _htf_get_loop_file(const char* base_dirname, htf::Thread* th, htf::Token loop, const char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/thread_%u/loop_%d", base_dirname, th->id, loop.id);
  return _htf_file_open(filename, mode);
}

static void _htf_store_loop(const char* base_dirname, htf::Thread* th, htf::Loop* l, htf::Token loop) {
  FILE* file = _htf_get_loop_file(base_dirname, th, loop, "w");
  if (htf::debugLevel >= htf::DebugLevel::Debug) {
    htf_log(htf::DebugLevel::Debug, "\tStore loops %x {.nb_loops=%zu, .repeated_token=%x.%x, .nb_iterations:", loop.id,
            l->nb_iterations.size(), l->repeated_token.type, l->repeated_token.id);
    std::cout << "[";
    for (const auto& i : l->nb_iterations) {
      std::cout << i << ((&i != &l->nb_iterations.back()) ? ", " : "]");
    }
    std::cout << "}" << std::endl;
  }
  _htf_fwrite(&l->repeated_token, sizeof(l->repeated_token), 1, file);
  size_t size = l->nb_iterations.size();
  _htf_fwrite(&size, sizeof(size), 1, file);
  _htf_fwrite(l->nb_iterations.data(), sizeof(uint), l->nb_iterations.size(), file);
  fclose(file);
}

static void _htf_read_loop(const char* base_dirname, htf::Thread* th, htf::Loop* l, htf::Token loop) {
  FILE* file = _htf_get_loop_file(base_dirname, th, loop, "r");
  l->self_id = loop;
  _htf_fread(&l->repeated_token, sizeof(l->repeated_token), 1, file);
  size_t size;
  _htf_fread(&size, sizeof(size), 1, file);
  l->nb_iterations.resize(size);
  _htf_fread(l->nb_iterations.data(), sizeof(uint), size, file);
  fclose(file);
  if (htf::debugLevel >= htf::DebugLevel::Debug) {
    htf_log(htf::DebugLevel::Debug, "\tLoad loops %x {.nb_loops=%zu, .repeated_token=%x.%x, .nb_iterations: ", loop.id,
            l->nb_iterations.size(), l->repeated_token.type, l->repeated_token.id);
    std::cout << "[";
    for (const auto& i : l->nb_iterations) {
      std::cout << i << ((&i != &l->nb_iterations.back()) ? ", " : "]");
    }
    std::cout << "}" << std::endl;
  }
}

static FILE* _htf_get_string_file(htf::Archive* a, int string_index, const char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/archive_%u/string_%d", base_dirname(a), a->id, string_index);
  return _htf_file_open(filename, mode);
}

static void _htf_store_string_generic(FILE* file, htf::String* s, int string_index) {
  htf_log(htf::DebugLevel::Debug, "\tStore String %d {.ref=%d, .length=%d, .str='%s'}\n", string_index, s->string_ref,
          s->length, s->str);

  _htf_fwrite(&s->string_ref, sizeof(s->string_ref), 1, file);
  _htf_fwrite(&s->length, sizeof(s->length), 1, file);
  _htf_fwrite(s->str, sizeof(char), s->length, file);
}

static void _htf_store_string(htf::Archive* a, htf::String* s, int string_index) {
  FILE* file = _htf_get_string_file(a, string_index, "w");
  _htf_store_string_generic(file, s, string_index);
  fclose(file);
}

static void _htf_read_string_generic(FILE* file, htf::String* s, int string_index) {
  _htf_fread(&s->string_ref, sizeof(s->string_ref), 1, file);
  _htf_fread(&s->length, sizeof(s->length), 1, file);
  s->str = new char[s->length];
  htf_assert(s->str);
  _htf_fread(s->str, sizeof(char), s->length, file);
  htf_log(htf::DebugLevel::Debug, "\tLoad String %d {.ref=%d, .length=%d, .str='%s'}\n", string_index, s->string_ref,
          s->length, s->str);
}

static void _htf_read_string(htf::Archive* a, htf::String* s, int string_index) {
  FILE* file = _htf_get_string_file(a, string_index, "r");
  _htf_read_string_generic(file, s, string_index);
  fclose(file);
}

static FILE* _htf_get_regions_file(htf::Archive* a, const char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/archive_%u/regions.dat", base_dirname(a), a->id);
  return _htf_file_open(filename, mode);
}

static void _htf_store_regions_generic(FILE* file, htf::Definition* d) {
  if (d->regions.empty())
    return;

  htf_log(htf::DebugLevel::Debug, "\tStore %zu Regions\n", d->regions.size());
  _htf_fwrite(d->regions.data(), sizeof(htf::Region), d->regions.size(), file);
}

static void _htf_store_regions(htf::Archive* a) {
  if (a->definitions.regions.empty())
    return;

  FILE* file = _htf_get_regions_file(a, "w");
  _htf_store_regions_generic(file, &a->definitions);
  fclose(file);
}

static void _htf_read_regions_generic(FILE* file, htf::Definition* d) {
  if (d->regions.empty())
    return;

  _htf_fread(d->regions.data(), sizeof(htf::Region), d->regions.size(), file);

  htf_log(htf::DebugLevel::Debug, "\tLoad %zu regions\n", d->regions.size());
}

static void _htf_read_regions(htf::Archive* a) {
  if (a->definitions.regions.empty())
    return;

  FILE* file = _htf_get_regions_file(a, "r");
  _htf_read_regions_generic(file, &a->definitions);
  fclose(file);
}

static FILE* _htf_get_attributes_file(htf::Archive* a, const char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/archive_%u/attributes.dat", base_dirname(a), a->id);
  return _htf_file_open(filename, mode);
}

static void _htf_store_attributes_generic(FILE* file, htf::Definition* d) {
  if (d->attributes.empty())
    return;

  htf_log(htf::DebugLevel::Debug, "\tStore %zu Attributes\n", d->attributes.size());
  for (int i = 0; i < d->attributes.size(); i++) {
    htf_log(htf::DebugLevel::Debug, "\t\t[%d] {ref=%x, name=%x, type=%x}\n", i, d->attributes[i].attribute_ref,
            d->attributes[i].name, d->attributes[i].type);
  }

  _htf_fwrite(d->attributes.data(), sizeof(htf::Attribute), d->attributes.size(), file);
}

static void _htf_store_attributes(htf::Archive* a) {
  if (a->definitions.attributes.empty())
    return;

  FILE* file = _htf_get_attributes_file(a, "w");
  _htf_store_attributes_generic(file, &a->definitions);
  fclose(file);
}

static void _htf_read_attributes_generic(FILE* file, htf::Definition* d) {
  if (d->attributes.empty())
    return;
  _htf_fread(d->attributes.data(), sizeof(htf::Attribute), d->attributes.size(), file);

  htf_log(htf::DebugLevel::Debug, "\tLoad %zu attributes\n", d->attributes.size());
}

static void _htf_read_attributes(htf::Archive* a) {
  if (a->definitions.attributes.empty())
    return;

  FILE* file = _htf_get_attributes_file(a, "r");
  _htf_read_attributes_generic(file, &a->definitions);
  fclose(file);
}

static FILE* _htf_get_location_groups_file(htf::Archive* a, const char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/archive_%u/location_groups.dat", base_dirname(a), a->id);
  return _htf_file_open(filename, mode);
}

static void _htf_store_location_groups(htf::Archive* a) {
  if (a->location_groups.empty())
    return;

  FILE* file = _htf_get_location_groups_file(a, "w");
  htf_log(htf::DebugLevel::Debug, "\tStore %zu location groupds\n", a->location_groups.size());

  _htf_fwrite(a->location_groups.data(), sizeof(htf::LocationGroup), a->location_groups.size(), file);
  fclose(file);
}

static void _htf_read_location_groups(htf::Archive* a) {
  if (a->location_groups.empty())
    return;

  FILE* file = _htf_get_location_groups_file(a, "r");

  _htf_fread(a->location_groups.data(), sizeof(htf::LocationGroup), a->location_groups.size(), file);
  fclose(file);

  htf_log(htf::DebugLevel::Debug, "\tLoad %zu location_groups\n", a->location_groups.size());
}

static FILE* _htf_get_locations_file(htf::Archive* a, const char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/archive_%u/locations.dat", base_dirname(a), a->id);
  return _htf_file_open(filename, mode);
}

static void _htf_store_locations(htf::Archive* a) {
  if (a->locations.empty())
    return;

  FILE* file = _htf_get_locations_file(a, "w");
  htf_log(htf::DebugLevel::Debug, "\tStore %zu locations\n", a->locations.size());

  for (auto& l : a->locations) {
    htf_assert(l.id != HTF_THREAD_ID_INVALID);
  }

  _htf_fwrite(a->locations.data(), sizeof(htf::Location), a->locations.size(), file);
  fclose(file);
}

static void _htf_read_locations(htf::Archive* a) {
  if (a->locations.empty())
    return;

  FILE* file = _htf_get_locations_file(a, "r");

  _htf_fread(a->locations.data(), sizeof(htf::Location), a->locations.size(), file);
  fclose(file);

  htf_log(htf::DebugLevel::Debug, "\tLoad %zu locations\n", a->locations.size());
}

static FILE* _htf_get_thread(const char* dir_name, htf::ThreadId thread_id, const char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/thread_%u.dat", dir_name, thread_id);
  return _htf_file_open(filename, mode);
}

static void _htf_store_thread(const char* dir_name, htf::Thread* th) {
  if (th->nb_events == 0) {
    htf_log(htf::DebugLevel::Verbose, "\tSkipping Thread %u {.nb_events=%d, .nb_sequences=%d, .nb_loops=%d}\n", th->id,
            th->nb_events, th->nb_sequences, th->nb_loops);
    abort();
  }

  FILE* token_file = _htf_get_thread(dir_name, th->id, "w");

  htf_log(htf::DebugLevel::Verbose, "\tThread %u {.nb_events=%d, .nb_sequences=%d, .nb_loops=%d}\n", th->id,
          th->nb_events, th->nb_sequences, th->nb_loops);

  _htf_fwrite(&th->id, sizeof(th->id), 1, token_file);
  _htf_fwrite(&th->archive->id, sizeof(th->archive->id), 1, token_file);

  _htf_fwrite(&th->nb_events, sizeof(th->nb_events), 1, token_file);
  _htf_fwrite(&th->nb_sequences, sizeof(th->nb_sequences), 1, token_file);
  _htf_fwrite(&th->nb_loops, sizeof(th->nb_loops), 1, token_file);

  fclose(token_file);
  htf_finish_timestamp();

  for (int i = 0; i < th->nb_events; i++)
    _htf_store_event(dir_name, th, &th->events[i], HTF_EVENT_ID(i));

  for (int i = 0; i < th->nb_sequences; i++)
    _htf_store_sequence(dir_name, th, th->sequences[i], HTF_SEQUENCE_ID(i));

  for (int i = 0; i < th->nb_loops; i++)
    _htf_store_loop(dir_name, th, &th->loops[i], HTF_LOOP_ID(i));
  htf_log(htf::DebugLevel::Normal, "Average compression ratio: %.2f\n", (numberRawBytes + .0) / numberCompressedBytes);
}

void htf::Thread::finalizeThread() {
  _htf_store_thread(archive->dir_name, this);
}

static void _htf_read_thread(htf::Archive* global_archive, htf::Thread* th, htf::ThreadId thread_id) {
  FILE* token_file = _htf_get_thread(global_archive->dir_name, thread_id, "r");
  _htf_fread(&th->id, sizeof(th->id), 1, token_file);
  htf::LocationGroupId archive_id;
  _htf_fread(&archive_id, sizeof(archive_id), 1, token_file);
  th->archive = _htf_get_archive(global_archive, archive_id);

  _htf_fread(&th->nb_events, sizeof(th->nb_events), 1, token_file);
  th->nb_allocated_events = th->nb_events;
  th->events = new htf::EventSummary[th->nb_allocated_events];

  _htf_fread(&th->nb_sequences, sizeof(th->nb_sequences), 1, token_file);
  th->nb_allocated_sequences = th->nb_sequences;
  th->sequences = new htf::Sequence*[th->nb_allocated_sequences];
  for (int i = 0; i < th->nb_sequences; i++) {
    th->sequences[i] = new htf::Sequence;
  }

  _htf_fread(&th->nb_loops, sizeof(th->nb_loops), 1, token_file);
  th->nb_allocated_loops = th->nb_loops;
  th->loops = new htf::Loop[th->nb_allocated_loops];

  htf_log(htf::DebugLevel::Verbose, "Reading %d events\n", th->nb_events);
  for (int i = 0; i < th->nb_events; i++)
    _htf_read_event(global_archive->dir_name, th, &th->events[i], HTF_EVENT_ID(i));

  htf_log(htf::DebugLevel::Verbose, "Reading %d sequences\n", th->nb_sequences);
  for (int i = 0; i < th->nb_sequences; i++)
    _htf_read_sequence(global_archive->dir_name, th, th->sequences[i], HTF_SEQUENCE_ID(i));

  htf_log(htf::DebugLevel::Verbose, "Reading %d loops\n", th->nb_loops);
  for (int i = 0; i < th->nb_loops; i++)
    _htf_read_loop(global_archive->dir_name, th, &th->loops[i], HTF_LOOP_ID(i));

  fclose(token_file);

  htf_log(htf::DebugLevel::Verbose, "\tThread %u: {.nb_events=%d, .nb_sequences=%d, .nb_loops=%d}\n", th->id,
          th->nb_events, th->nb_sequences, th->nb_loops);
}

void htf_storage_finalize_thread(htf::Thread* thread) {
  if (!thread)
    return;
  _htf_store_thread(thread->archive->dir_name, thread);
}

void htf_storage_finalize(htf::Archive* archive) {
  if (!archive)
    return;

  int fullpath_len = strlen(archive->dir_name) + strlen(archive->trace_name) + 20;
  char* fullpath = new char[fullpath_len];
  if (archive->id == HTF_MAIN_LOCATION_GROUP_ID)
    snprintf(fullpath, fullpath_len, "%s/%s.htf", archive->dir_name, archive->trace_name);
  else
    snprintf(fullpath, fullpath_len, "%s/%s_%u.htf", archive->dir_name, archive->trace_name, archive->id);

  FILE* f = _htf_file_open(fullpath, "w");
  delete[] fullpath;
  _htf_fwrite(&archive->id, sizeof(htf::LocationGroupId), 1, f);
  size_t size = archive->definitions.strings.size();
  _htf_fwrite(&size, sizeof(size), 1, f);
  size = archive->definitions.regions.size();
  _htf_fwrite(&size, sizeof(size), 1, f);
  size = archive->definitions.attributes.size();
  _htf_fwrite(&size, sizeof(size), 1, f);
  size = archive->location_groups.size();
  _htf_fwrite(&size, sizeof(size), 1, f);
  size = archive->locations.size();
  _htf_fwrite(&size, sizeof(size), 1, f);
  _htf_fwrite(&archive->nb_threads, sizeof(int), 1, f);
  //  _htf_fwrite(&COMPRESSION_OPTIONS, sizeof(COMPRESSION_OPTIONS), 1, f);
  _htf_fwrite(&STORE_HASHING, sizeof(STORE_HASHING), 1, f);
  _htf_fwrite(&STORE_TIMESTAMPS, sizeof(STORE_TIMESTAMPS), 1, f);

  for (int i = 0; i < archive->definitions.strings.size(); i++) {
    _htf_store_string(archive, &archive->definitions.strings[i], i);
  }

  _htf_store_regions(archive);
  _htf_store_attributes(archive);

  _htf_store_location_groups(archive);
  _htf_store_locations(archive);

  fclose(f);
}

static char* _archive_filename(htf::Archive* global_archive, htf::LocationGroupId id) {
  if (id == HTF_MAIN_LOCATION_GROUP_ID)
    return strdup(global_archive->trace_name);

  int tracename_len = strlen(global_archive->trace_name) + 1;
  int extension_index = tracename_len - 5;
  htf_assert(strcmp(&global_archive->trace_name[extension_index], ".htf") == 0);

  char trace_basename[tracename_len];
  strncpy(trace_basename, global_archive->trace_name, extension_index);
  trace_basename[extension_index] = '\0';

  int len = strlen(trace_basename) + 20;
  char* result = new char[len];
  snprintf(result, len, "%s_%d.htf", trace_basename, id);
  return result;
}

char* htf_archive_fullpath(char* dir_name, char* trace_name) {
  int len = strlen(dir_name) + strlen(trace_name) + 2;
  char* fullpath = new char[len];
  snprintf(fullpath, len, "%s/%s", dir_name, trace_name);
  return fullpath;
}

static void _htf_read_archive(htf::Archive* global_archive, htf::Archive* archive, char* dir_name, char* trace_name) {
  archive->fullpath = htf_archive_fullpath(dir_name, trace_name);
  archive->dir_name = strdup(dir_name);
  archive->trace_name = strdup(trace_name);
  archive->global_archive = global_archive;
  archive->nb_archives = 0;
  archive->nb_allocated_archives = 1;
  archive->archive_list = new htf::Archive*();
  archive->definitions = htf::Definition();
  if (archive->archive_list == nullptr) {
    htf_error("Failed to allocate memory\n");
  }

  htf_log(htf::DebugLevel::Debug, "Reading archive {.dir_name='%s', .trace='%s'}\n", archive->dir_name,
          archive->trace_name);

  FILE* f = _htf_file_open(archive->fullpath, "r");

  _htf_fread(&archive->id, sizeof(htf::LocationGroupId), 1, f);
  size_t size;

  _htf_fread(&size, sizeof(size), 1, f);
  archive->definitions.strings.resize(size);
  _htf_fread(&size, sizeof(size), 1, f);
  archive->definitions.regions.resize(size);
  _htf_fread(&size, sizeof(size), 1, f);
  archive->definitions.attributes.resize(size);
  _htf_fread(&size, sizeof(size), 1, f);
  archive->location_groups.resize(size);
  _htf_fread(&size, sizeof(size), 1, f);
  archive->locations.resize(size);

  _htf_fread(&archive->nb_threads, sizeof(int), 1, f);

  archive->threads = new htf::Thread*[archive->nb_threads];
  archive->nb_allocated_threads = archive->nb_threads;

  //  _htf_fread(&COMPRESSION_OPTIONS, sizeof(COMPRESSION_OPTIONS), 1, f);
  _htf_fread(&STORE_HASHING, sizeof(STORE_HASHING), 1, f);
  _htf_fread(&STORE_TIMESTAMPS, sizeof(STORE_TIMESTAMPS), 1, f);

  char* store_timestamps_str = getenv("STORE_TIMESTAMPS");
  if (store_timestamps_str && strcmp(store_timestamps_str, "FALSE") == 0) {
    STORE_TIMESTAMPS = 0;
  }
  archive->store_timestamps = STORE_TIMESTAMPS;

  for (int i = 0; i < archive->definitions.strings.size(); i++) {
    htf_assert(strcmp(archive->dir_name, dir_name) == 0);
    _htf_read_string(archive, &archive->definitions.strings[i], i);
  }

  _htf_read_regions(archive);
  _htf_read_attributes(archive);

  if (!archive->location_groups.empty()) {
    _htf_read_location_groups(archive);
  }

  if (!archive->locations.empty()) {
    _htf_read_locations(archive);
  }

  if (archive->id == HTF_MAIN_LOCATION_GROUP_ID) {
    global_archive = archive;
  }

  for (auto& location : archive->locations) {
    htf_assert(location.id != HTF_THREAD_ID_INVALID);
    htf_read_thread(global_archive, location.id);
  }
  fclose(f);
}

static htf::Archive* _htf_get_archive(htf::Archive* global_archive, htf::LocationGroupId archive_id) {
  /* check if archive_id is already known */
  for (int i = 0; i < global_archive->nb_archives; i++) {
    if (global_archive->archive_list[i]->id == archive_id) {
      return global_archive->archive_list[i];
    }
  }

  /* not found. we need to read the archive */
  auto* arch = new htf::Archive();
  char* filename = _archive_filename(global_archive, archive_id);
  char* fullpath = htf_archive_fullpath(global_archive->dir_name, filename);
  if (access(fullpath, R_OK) < 0) {
    printf("I can't read %s: %s\n", fullpath, strerror(errno));
    free(fullpath);
    return nullptr;
  }
  printf("Reading archive %s\n", fullpath);
  free(fullpath);

  while (global_archive->nb_archives >= global_archive->nb_allocated_archives) {
    INCREMENT_MEMORY_SPACE(global_archive->archive_list, global_archive->nb_allocated_archives, htf::Archive*);
  }

  _htf_read_archive(global_archive, arch, global_archive->dir_name, filename);

  int index = global_archive->nb_archives++;
  global_archive->archive_list[index] = arch;

  return arch;
}

void htf_read_thread(htf::Archive* archive, htf::ThreadId thread_id) {
  for (int i = 0; i < archive->nb_threads; i++) {
    if (archive->threads[i]->id == thread_id) {
      /* thread_id is already loaded */
      return;
    }
  }

  while (archive->nb_threads >= archive->nb_allocated_threads) {
    INCREMENT_MEMORY_SPACE(archive->threads, archive->nb_allocated_threads, htf::Thread*);
  }

  int index = archive->nb_threads++;
  archive->threads[index] = new htf::Thread();
  _htf_read_thread(archive, archive->threads[index], thread_id);
  htf_assert(archive->threads[index]->nb_events > 0);
}

void htf_read_archive(htf::Archive* archive, char* main_filename) {
  char* dir_name = dirname(strdup(main_filename));
  char* trace_name = basename(strdup(main_filename));

  _htf_read_archive(nullptr, archive, dir_name, trace_name);

  htf::Archive* global_archive = archive->global_archive;

  if (archive->id == HTF_MAIN_LOCATION_GROUP_ID) {
    global_archive = archive;
  }

  for (auto& location : archive->locations) {
    htf_read_thread(global_archive, location.id);
  }

  for (auto& location_group : archive->location_groups) {
    _htf_get_archive(global_archive, location_group.id);
  }
}

/* -*-
   mode: c;
   c-file-style: "k&r";
   c-basic-offset 2;
   tab-width 2 ;
   indent-tabs-mode nil
   -*- */
