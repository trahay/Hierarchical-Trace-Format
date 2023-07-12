#include <stdio.h>
#include <stdlib.h>

#include "OTF2_MarkerReaderCallbacks.h"
#include "htf.h"
#include "otf2.h"

/** @brief Allocates a new struct for the marker callbacks.
 *
 *  @since Version 1.2
 *
 *  @return A newly allocated struct of type @eref{OTF2_MarkerReaderCallbacks}.
 */
OTF2_MarkerReaderCallbacks* OTF2_MarkerReaderCallbacks_New(void) {
  NOT_IMPLEMENTED;
}

/** @brief Deallocates a struct for the marker callbacks.
 *
 *  @since Version 1.2
 *
 *  @param markerReaderCallbacks Handle to a struct previously allocated
 *                               with @eref{OTF2_MarkerReaderCallbacks_New}.
 */
void OTF2_MarkerReaderCallbacks_Delete(OTF2_MarkerReaderCallbacks* markerReaderCallbacks) {
  NOT_IMPLEMENTED;
}

/** @brief Clears a struct for the marker callbacks.
 *
 *  @since Version 1.2
 *
 *  @param markerReaderCallbacks Handle to a struct previously allocated
 *                               with @eref{OTF2_MarkerReaderCallbacks_New}.
 */
void OTF2_MarkerReaderCallbacks_Clear(OTF2_MarkerReaderCallbacks* markerReaderCallbacks) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for an unknown marker.
 *
 *  @param markerReaderCallbacks Struct for all callbacks.
 *  @param unknownCallback       Function which should be called for all
 *                               unknown definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_MarkerReaderCallbacks_SetUnknownCallback(OTF2_MarkerReaderCallbacks* markerReaderCallbacks,
                                                             OTF2_MarkerReaderCallback_Unknown unknownCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the @eref{DefMarker} definition.
 *
 *  @param markerReaderCallbacks    Struct for all callbacks.
 *  @param defMarkerCallback        Function which should be called for all
 *                                  @eref{DefMarker} definitions.
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_MarkerReaderCallbacks_SetDefMarkerCallback(OTF2_MarkerReaderCallbacks* markerReaderCallbacks,
                                                               OTF2_MarkerReaderCallback_DefMarker defMarkerCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the @eref{Marker} record.
 *
 *  @param markerReaderCallbacks    Struct for all callbacks.
 *  @param markerCallback           Function which should be called for all
 *                                  @eref{Marker} records.
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_MarkerReaderCallbacks_SetMarkerCallback(OTF2_MarkerReaderCallbacks* markerReaderCallbacks,
                                                            OTF2_MarkerReaderCallback_Marker markerCallback) {
  NOT_IMPLEMENTED;
}
