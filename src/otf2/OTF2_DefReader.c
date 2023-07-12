#include <stdio.h>
#include <stdlib.h>

#include "OTF2_DefReader.h"
#include "htf.h"
#include "otf2.h"

/** @brief Get the location ID of this reader object.
 *
 *  @param reader        This given reader object will be deleted.
 *  @param location      Pointer to the variable where the location ID is
 *                       returned in.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode OTF2_DefReader_GetLocationID(const OTF2_DefReader* reader, OTF2_LocationRef* location) {
  NOT_IMPLEMENTED;
}

/** @brief Sets the callback functions for the given reader object. Every time
 *         when OTF2 reads a record, a callback function is called and the records
 *         data is passed to this function. Therefore the programmer needs to set
 *         function pointers at the "callbacks" struct for the record type he wants
 *         to read.
 *
 *  @param reader          This given reader object will be set up with new
 *                         callback functions.
 *  @param callbacks       Struct which holds a function pointer for each
 *                         record type. @eref{OTF2_DefReaderCallbacks_New}.
 *  @param userData        Data passed as argument @a userData to the record
 *                         callbacks.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode OTF2_DefReader_SetCallbacks(OTF2_DefReader* reader,
                                           const OTF2_DefReaderCallbacks* callbacks,
                                           void* userData) {
  NOT_IMPLEMENTED;
}

/** @brief Reads the given number of records from the definition reader.
 *
 *  @param reader           The records of this reader will be read when the function is issued.
 *  @param recordsToRead    This variable tells the reader how much records it has to read.
 *  @param[out] recordsRead This is a pointer to variable where the amount of actually read
 *                          records is returned. This may differ to the given recordsToRead if
 *                          there are no more records left in the trace. In this case the programmer
 *                          can easily check that the reader has finished his job by checking
 *                          recordsRead < recordsToRead.
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INTERRUPTED_BY_CALLBACK,
 *             if an user supplied callback returned OTF2_CALLBACK_INTERRUPT}
 *    @retcode{OTF2_ERROR_DUPLICATE_MAPPING_TABLE,
 *             if a duplicate mapping table definition was read}
 *    @retelse{the error code}
 *  @retend
 */
OTF2_ErrorCode OTF2_DefReader_ReadDefinitions(OTF2_DefReader* reader, uint64_t recordsToRead, uint64_t* recordsRead) {
  NOT_IMPLEMENTED;
}
