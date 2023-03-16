#include <stdlib.h>
#include <stdio.h>

#include "htf.h"
#include "otf2.h"
#include "OTF2_GlobalEvtReader.h"


/** @brief Sets the callback functions for the given reader object. Every time
 *         when OTF2 reads a record, a callback function is called and the records
 *         data is passed to this function. Therefore the programmer needs to set
 *         function pointers at the "callbacks" struct for the record type he wants
 *         to read.
 *
 *  @param reader          Reader object which reads the events from its
 *                         buffer.
 *  @param callbacks       Struct which holds a function pointer for each
 *                         record type. @eref{OTF2_GlobalEvtReaderCallbacks_New}.
 *  @param userData        Data passed as argument @a userData to the record
 *                         callbacks.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_GlobalEvtReader_SetCallbacks( OTF2_GlobalEvtReader*                reader,
                                   const OTF2_GlobalEvtReaderCallbacks* callbacks,
                                   void*                                userData ) {
  NOT_IMPLEMENTED;
}


/** @brief Triggers the callback for the next event record.
 *
 *  @param reader          Reader object which reads the events from its
 *                         buffer.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_GlobalEvtReader_ReadEvent( OTF2_GlobalEvtReader* reader ) {
  NOT_IMPLEMENTED;
}


/** @brief Has more events.
 *
 *  @param reader     Global event reader handle.
 *  @param[out] flag  In case of success, the flag will be set to 1
 *                    when there is at least more more event to read.
 *                    To 0 if not. Otherwise the value is undefined.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_GlobalEvtReader_HasEvent( OTF2_GlobalEvtReader* reader,
                               int*                  flag ) {
  NOT_IMPLEMENTED;
}

/** @brief Reads the given number of records from the global event reader.
 *
 *  @param reader           The records of this reader will be read when the function is issued.
 *  @param recordsToRead    This variable tells the reader how much records it has to read.
 *  @param[out] recordsRead This is a pointer to variable where the amount of actually read
 *                          records is returned. This may differ to the given recordsToRead if
 *                          there are no more records left in the trace. In this case the programmer
 *                          can easily check that the reader has finished his job by checking
 *                          recordsRead < recordsToRead.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_GlobalEvtReader_ReadEvents( OTF2_GlobalEvtReader* reader,
                                 uint64_t              recordsToRead,
                                 uint64_t*             recordsRead ) {
  NOT_IMPLEMENTED;
}
