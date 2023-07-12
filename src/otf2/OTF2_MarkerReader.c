#include <stdio.h>
#include <stdlib.h>

#include "OTF2_MarkerReader.h"
#include "htf.h"
#include "otf2.h"

/** @brief After callback registration, the markers could be
 *         read with the following function. The user of this
 *         function tells the system how many markers it is
 *         able to handle (recordsToRead) and the function returns
 *         how many markers where in the stream (recordsRead).
 *         It should usually be the case that both values are the
 *         same. If this is not the case, then there where less
 *         records than requested in the stream.
 *
 *  @param reader         Reader Object.
 *  @param recordsToRead  How many records have to be read next.
 *  @param recordsRead    How many records where read?
 *
 *  @since Version 1.2
 *
 *  @return OTF2_ErrorCode with !=OTF2_SUCCESS if there was an error.
 */
OTF2_ErrorCode OTF2_MarkerReader_ReadMarkers(OTF2_MarkerReader* reader, uint64_t recordsToRead, uint64_t* recordsRead) {
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
 *                         record type. @eref{OTF2_MarkerReaderCallbacks_New}.
 *  @param userData        Data passed as argument @a userData to the record
 *                         callbacks.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode OTF2_MarkerReader_SetCallbacks(OTF2_MarkerReader* reader,
                                              const OTF2_MarkerReaderCallbacks* callbacks,
                                              void* userData) {
  NOT_IMPLEMENTED;
}
