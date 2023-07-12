#include <stdio.h>
#include <stdlib.h>

#include "OTF2_SnapReaderCallbacks.h"
#include "htf.h"
#include "otf2.h"

/** @brief Allocates a new struct for the snap event callbacks.
 *
 *  @since Version 1.2
 *
 *  @return A newly allocated struct of type @eref{OTF2_SnapReaderCallbacks}.
 */
OTF2_SnapReaderCallbacks* OTF2_SnapReaderCallbacks_New(void) {
  NOT_IMPLEMENTED;
}

/** @brief Deallocates a struct for the snap event callbacks.
 *
 *  @param snapReaderCallbacks Handle to a struct previously allocated
 *                            with @eref{OTF2_SnapReaderCallbacks_New}.
 *
 *  @since Version 1.2
 */
void OTF2_SnapReaderCallbacks_Delete(OTF2_SnapReaderCallbacks* snapReaderCallbacks) {
  NOT_IMPLEMENTED;
}

/** @brief Clears a struct for the snapshot event callbacks.
 *
 *  @param snapReaderCallbacks Handle to a struct previously allocated
 *                            with @eref{OTF2_SnapReaderCallbacks_New}.
 *
 *  @since Version 1.2
 */
void OTF2_SnapReaderCallbacks_Clear(OTF2_SnapReaderCallbacks* snapReaderCallbacks) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the Unknown snap event.
 *
 *  @param snapReaderCallbacks Struct for all callbacks.
 *  @param unknownCallback    Function which should be called for all
 *                            unknown snap events.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_SnapReaderCallbacks_SetUnknownCallback(OTF2_SnapReaderCallbacks* snapReaderCallbacks,
                                                           OTF2_SnapReaderCallback_Unknown unknownCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the SnapshotStart snap event.
 *
 *  @param snapReaderCallbacks   Struct for all callbacks.
 *  @param snapshotStartCallback Function which should be called for all
 *                               @eref{SnapshotStart} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_SnapReaderCallbacks_SetSnapshotStartCallback(
  OTF2_SnapReaderCallbacks* snapReaderCallbacks,
  OTF2_SnapReaderCallback_SnapshotStart snapshotStartCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the SnapshotEnd snap event.
 *
 *  @param snapReaderCallbacks Struct for all callbacks.
 *  @param snapshotEndCallback Function which should be called for all
 *                             @eref{SnapshotEnd} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_SnapReaderCallbacks_SetSnapshotEndCallback(
  OTF2_SnapReaderCallbacks* snapReaderCallbacks,
  OTF2_SnapReaderCallback_SnapshotEnd snapshotEndCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the MeasurementOnOff snap event.
 *
 *  @param snapReaderCallbacks      Struct for all callbacks.
 *  @param measurementOnOffCallback Function which should be called for all
 *                                  @eref{MeasurementOnOff} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_SnapReaderCallbacks_SetMeasurementOnOffCallback(
  OTF2_SnapReaderCallbacks* snapReaderCallbacks,
  OTF2_SnapReaderCallback_MeasurementOnOff measurementOnOffCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the Enter snap event.
 *
 *  @param snapReaderCallbacks Struct for all callbacks.
 *  @param enterCallback       Function which should be called for all
 *                             @eref{Enter} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_SnapReaderCallbacks_SetEnterCallback(OTF2_SnapReaderCallbacks* snapReaderCallbacks,
                                                         OTF2_SnapReaderCallback_Enter enterCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the MpiSend snap event.
 *
 *  @param snapReaderCallbacks Struct for all callbacks.
 *  @param mpiSendCallback     Function which should be called for all
 *                             @eref{MpiSend} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_SnapReaderCallbacks_SetMpiSendCallback(OTF2_SnapReaderCallbacks* snapReaderCallbacks,
                                                           OTF2_SnapReaderCallback_MpiSend mpiSendCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the MpiIsend snap event.
 *
 *  @param snapReaderCallbacks Struct for all callbacks.
 *  @param mpiIsendCallback    Function which should be called for all
 *                             @eref{MpiIsend} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_SnapReaderCallbacks_SetMpiIsendCallback(OTF2_SnapReaderCallbacks* snapReaderCallbacks,
                                                            OTF2_SnapReaderCallback_MpiIsend mpiIsendCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the MpiIsendComplete snap event.
 *
 *  @param snapReaderCallbacks      Struct for all callbacks.
 *  @param mpiIsendCompleteCallback Function which should be called for all
 *                                  @eref{MpiIsendComplete} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_SnapReaderCallbacks_SetMpiIsendCompleteCallback(
  OTF2_SnapReaderCallbacks* snapReaderCallbacks,
  OTF2_SnapReaderCallback_MpiIsendComplete mpiIsendCompleteCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the MpiRecv snap event.
 *
 *  @param snapReaderCallbacks Struct for all callbacks.
 *  @param mpiRecvCallback     Function which should be called for all
 *                             @eref{MpiRecv} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_SnapReaderCallbacks_SetMpiRecvCallback(OTF2_SnapReaderCallbacks* snapReaderCallbacks,
                                                           OTF2_SnapReaderCallback_MpiRecv mpiRecvCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the MpiIrecvRequest snap event.
 *
 *  @param snapReaderCallbacks     Struct for all callbacks.
 *  @param mpiIrecvRequestCallback Function which should be called for all
 *                                 @eref{MpiIrecvRequest} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_SnapReaderCallbacks_SetMpiIrecvRequestCallback(
  OTF2_SnapReaderCallbacks* snapReaderCallbacks,
  OTF2_SnapReaderCallback_MpiIrecvRequest mpiIrecvRequestCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the MpiIrecv snap event.
 *
 *  @param snapReaderCallbacks Struct for all callbacks.
 *  @param mpiIrecvCallback    Function which should be called for all
 *                             @eref{MpiIrecv} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_SnapReaderCallbacks_SetMpiIrecvCallback(OTF2_SnapReaderCallbacks* snapReaderCallbacks,
                                                            OTF2_SnapReaderCallback_MpiIrecv mpiIrecvCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the MpiCollectiveBegin snap event.
 *
 *  @param snapReaderCallbacks        Struct for all callbacks.
 *  @param mpiCollectiveBeginCallback Function which should be called for all
 *                                    @eref{MpiCollectiveBegin} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_SnapReaderCallbacks_SetMpiCollectiveBeginCallback(
  OTF2_SnapReaderCallbacks* snapReaderCallbacks,
  OTF2_SnapReaderCallback_MpiCollectiveBegin mpiCollectiveBeginCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the MpiCollectiveEnd snap event.
 *
 *  @param snapReaderCallbacks      Struct for all callbacks.
 *  @param mpiCollectiveEndCallback Function which should be called for all
 *                                  @eref{MpiCollectiveEnd} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_SnapReaderCallbacks_SetMpiCollectiveEndCallback(
  OTF2_SnapReaderCallbacks* snapReaderCallbacks,
  OTF2_SnapReaderCallback_MpiCollectiveEnd mpiCollectiveEndCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the OmpFork snap event.
 *
 *  @param snapReaderCallbacks Struct for all callbacks.
 *  @param ompForkCallback     Function which should be called for all
 *                             @eref{OmpFork} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_SnapReaderCallbacks_SetOmpForkCallback(OTF2_SnapReaderCallbacks* snapReaderCallbacks,
                                                           OTF2_SnapReaderCallback_OmpFork ompForkCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the OmpAcquireLock snap event.
 *
 *  @param snapReaderCallbacks    Struct for all callbacks.
 *  @param ompAcquireLockCallback Function which should be called for all
 *                                @eref{OmpAcquireLock} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_SnapReaderCallbacks_SetOmpAcquireLockCallback(
  OTF2_SnapReaderCallbacks* snapReaderCallbacks,
  OTF2_SnapReaderCallback_OmpAcquireLock ompAcquireLockCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the OmpTaskCreate snap event.
 *
 *  @param snapReaderCallbacks   Struct for all callbacks.
 *  @param ompTaskCreateCallback Function which should be called for all
 *                               @eref{OmpTaskCreate} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_SnapReaderCallbacks_SetOmpTaskCreateCallback(
  OTF2_SnapReaderCallbacks* snapReaderCallbacks,
  OTF2_SnapReaderCallback_OmpTaskCreate ompTaskCreateCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the OmpTaskSwitch snap event.
 *
 *  @param snapReaderCallbacks   Struct for all callbacks.
 *  @param ompTaskSwitchCallback Function which should be called for all
 *                               @eref{OmpTaskSwitch} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_SnapReaderCallbacks_SetOmpTaskSwitchCallback(
  OTF2_SnapReaderCallbacks* snapReaderCallbacks,
  OTF2_SnapReaderCallback_OmpTaskSwitch ompTaskSwitchCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the Metric snap event.
 *
 *  @param snapReaderCallbacks Struct for all callbacks.
 *  @param metricCallback      Function which should be called for all
 *                             @eref{Metric} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_SnapReaderCallbacks_SetMetricCallback(OTF2_SnapReaderCallbacks* snapReaderCallbacks,
                                                          OTF2_SnapReaderCallback_Metric metricCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the ParameterString snap event.
 *
 *  @param snapReaderCallbacks     Struct for all callbacks.
 *  @param parameterStringCallback Function which should be called for all
 *                                 @eref{ParameterString} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_SnapReaderCallbacks_SetParameterStringCallback(
  OTF2_SnapReaderCallbacks* snapReaderCallbacks,
  OTF2_SnapReaderCallback_ParameterString parameterStringCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the ParameterInt snap event.
 *
 *  @param snapReaderCallbacks  Struct for all callbacks.
 *  @param parameterIntCallback Function which should be called for all
 *                              @eref{ParameterInt} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_SnapReaderCallbacks_SetParameterIntCallback(
  OTF2_SnapReaderCallbacks* snapReaderCallbacks,
  OTF2_SnapReaderCallback_ParameterInt parameterIntCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the ParameterUnsignedInt snap event.
 *
 *  @param snapReaderCallbacks          Struct for all callbacks.
 *  @param parameterUnsignedIntCallback Function which should be called for all
 *                                      @eref{ParameterUnsignedInt}
 *                                      definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_SnapReaderCallbacks_SetParameterUnsignedIntCallback(
  OTF2_SnapReaderCallbacks* snapReaderCallbacks,
  OTF2_SnapReaderCallback_ParameterUnsignedInt parameterUnsignedIntCallback) {
  NOT_IMPLEMENTED;
}
