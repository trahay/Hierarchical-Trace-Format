#include <stdio.h>
#include <stdlib.h>

#include "OTF2_Thumbnail.h"
#include "htf.h"
#include "otf2.h"

/** @brief Reads a thumbnail header.
 *
 *  A thumbnail header contains some meta information for a thumbnail.
 *
 *  @param reader                Reader object.
 *  @param[out] name             Name of thumbnail. Allocated with @a malloc.
 *  @param[out] description      Description of thumbnail. Allocated with @a malloc.
 *  @param[out] type             Type of thumbnail.
 *  @param[out] numberOfSamples  Number of samples.
 *  @param[out] numberOfMetrics  Number of metrics.
 *  @param[out] refsToDefs       The sorted set of references to definitions
 *                               used in a thumbnail sample.
 *                               Allocated with @a malloc.
 *                               Contains numberOfMetrics entries
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode OTF2_ThumbReader_GetHeader(OTF2_ThumbReader* reader,
                                          char** const name,
                                          char** const description,
                                          OTF2_ThumbnailType* type,
                                          uint32_t* numberOfSamples,
                                          uint32_t* numberOfMetrics,
                                          uint64_t** refsToDefs) {
  NOT_IMPLEMENTED;
}

/** @brief Reads a thumbnail sample.
 *
 *  @param reader                Reader object.
 *  @param[out] baseline         Baseline for this sample. If zero, the baseline
 *                               is the sum of all metric values in this sample.
 *  @param numberOfMetrics       Number of metric sample values.
 *  @param[out] metricSamples    Metric sample values.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode OTF2_ThumbReader_ReadSample(OTF2_ThumbReader* reader,
                                           uint64_t* baseline,
                                           uint32_t numberOfMetrics,
                                           uint64_t* metricSamples) {
  NOT_IMPLEMENTED;
}

/** @brief Writes a thumbnail sample.
 *
 *  @param writer           Writer object.
 *  @param baseline         Baseline for this sample. If zero, the baseline is
 *                          the sum of all metric values in this sample.
 *  @param numberOfMetrics  Number of metric sample values.
 *  @param metricSamples    Metric sample values.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode OTF2_ThumbWriter_WriteSample(OTF2_ThumbWriter* writer,
                                            uint64_t baseline,
                                            uint32_t numberOfMetrics,
                                            const uint64_t* metricSamples) {
  NOT_IMPLEMENTED;
}
