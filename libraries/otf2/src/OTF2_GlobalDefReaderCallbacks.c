#include <stdio.h>
#include <stdlib.h>

#include "htf/htf.h"
#include "otf2/OTF2_GlobalDefReaderCallbacks.h"
#include "otf2/otf2.h"

/** @brief Allocates a new struct for the global definition callbacks.
 *
 *  @return A newly allocated struct of type @eref{OTF2_GlobalDefReaderCallbacks}.
 */
OTF2_GlobalDefReaderCallbacks* OTF2_GlobalDefReaderCallbacks_New(void) {
  NOT_IMPLEMENTED;
}

/** @brief Deallocates a struct for the global definition callbacks.
 *
 *  @param globalDefReaderCallbacks Handle to a struct previously allocated
 *                                  with @eref{OTF2_GlobalDefReaderCallbacks_New}.
 */
void OTF2_GlobalDefReaderCallbacks_Delete(OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks) {
  NOT_IMPLEMENTED;
}

/** @brief Clears a struct for the global definition callbacks.
 *
 *  @param globalDefReaderCallbacks Handle to a struct previously allocated
 *                                  with @eref{OTF2_GlobalDefReaderCallbacks_New}.
 */
void OTF2_GlobalDefReaderCallbacks_Clear(OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for an unknown definition.
 *
 *  @param globalDefReaderCallbacks Struct for all callbacks.
 *  @param unknownCallback          Function which should be called for all
 *                                  Unknown definitions.
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_GlobalDefReaderCallbacks_SetUnknownCallback(OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks,
                                                                OTF2_GlobalDefReaderCallback_Unknown unknownCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the @eref{ClockProperties} definition.
 *
 *  @param globalDefReaderCallbacks Struct for all callbacks.
 *  @param clockPropertiesCallback  Function which should be called for all
 *                                  @eref{ClockProperties} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_GlobalDefReaderCallbacks_SetClockPropertiesCallback(
  OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks,
  OTF2_GlobalDefReaderCallback_ClockProperties clockPropertiesCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the @eref{Paradigm} definition.
 *
 *  @param globalDefReaderCallbacks Struct for all callbacks.
 *  @param paradigmCallback         Function which should be called for all
 *                                  @eref{Paradigm} definitions.
 *
 *  @since Version 1.5
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_GlobalDefReaderCallbacks_SetParadigmCallback(
  OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks,
  OTF2_GlobalDefReaderCallback_Paradigm paradigmCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the @eref{ParadigmProperty} definition.
 *
 *  @param globalDefReaderCallbacks Struct for all callbacks.
 *  @param paradigmPropertyCallback Function which should be called for all
 *                                  @eref{ParadigmProperty} definitions.
 *
 *  @since Version 1.5
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_GlobalDefReaderCallbacks_SetParadigmPropertyCallback(
  OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks,
  OTF2_GlobalDefReaderCallback_ParadigmProperty paradigmPropertyCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the @eref{IoParadigm} definition.
 *
 *  @param globalDefReaderCallbacks Struct for all callbacks.
 *  @param ioParadigmCallback       Function which should be called for all
 *                                  @eref{IoParadigm} definitions.
 *
 *  @since Version 2.1
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_GlobalDefReaderCallbacks_SetIoParadigmCallback(
  OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks,
  OTF2_GlobalDefReaderCallback_IoParadigm ioParadigmCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the @eref{String} definition.
 *
 *  @param globalDefReaderCallbacks Struct for all callbacks.
 *  @param stringCallback           Function which should be called for all
 *                                  @eref{String} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_GlobalDefReaderCallbacks_SetStringCallback(OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks,
                                                               OTF2_GlobalDefReaderCallback_String stringCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the @eref{Attribute} definition.
 *
 *  @param globalDefReaderCallbacks Struct for all callbacks.
 *  @param attributeCallback        Function which should be called for all
 *                                  @eref{Attribute} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_GlobalDefReaderCallbacks_SetAttributeCallback(
  OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks,
  OTF2_GlobalDefReaderCallback_Attribute attributeCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the @eref{SystemTreeNode} definition.
 *
 *  @param globalDefReaderCallbacks Struct for all callbacks.
 *  @param systemTreeNodeCallback   Function which should be called for all
 *                                  @eref{SystemTreeNode} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_GlobalDefReaderCallbacks_SetSystemTreeNodeCallback(
  OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks,
  OTF2_GlobalDefReaderCallback_SystemTreeNode systemTreeNodeCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the @eref{LocationGroup} definition.
 *
 *  @param globalDefReaderCallbacks Struct for all callbacks.
 *  @param locationGroupCallback    Function which should be called for all
 *                                  @eref{LocationGroup} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_GlobalDefReaderCallbacks_SetLocationGroupCallback(
  OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks,
  OTF2_GlobalDefReaderCallback_LocationGroup locationGroupCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the @eref{Location} definition.
 *
 *  @param globalDefReaderCallbacks Struct for all callbacks.
 *  @param locationCallback         Function which should be called for all
 *                                  @eref{Location} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_GlobalDefReaderCallbacks_SetLocationCallback(
  OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks,
  OTF2_GlobalDefReaderCallback_Location locationCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the @eref{Region} definition.
 *
 *  @param globalDefReaderCallbacks Struct for all callbacks.
 *  @param regionCallback           Function which should be called for all
 *                                  @eref{Region} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_GlobalDefReaderCallbacks_SetRegionCallback(OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks,
                                                               OTF2_GlobalDefReaderCallback_Region regionCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the @eref{Callsite} definition.
 *
 *  @param globalDefReaderCallbacks Struct for all callbacks.
 *  @param callsiteCallback         Function which should be called for all
 *                                  @eref{Callsite} definitions.
 *
 *  @since Version 1.0
 *
 *  @deprecated In version 2.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_GlobalDefReaderCallbacks_SetCallsiteCallback(
  OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks,
  OTF2_GlobalDefReaderCallback_Callsite callsiteCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the @eref{Callpath} definition.
 *
 *  @param globalDefReaderCallbacks Struct for all callbacks.
 *  @param callpathCallback         Function which should be called for all
 *                                  @eref{Callpath} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_GlobalDefReaderCallbacks_SetCallpathCallback(
  OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks,
  OTF2_GlobalDefReaderCallback_Callpath callpathCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the @eref{Group} definition.
 *
 *  @param globalDefReaderCallbacks Struct for all callbacks.
 *  @param groupCallback            Function which should be called for all
 *                                  @eref{Group} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_GlobalDefReaderCallbacks_SetGroupCallback(OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks,
                                                              OTF2_GlobalDefReaderCallback_Group groupCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the @eref{MetricMember} definition.
 *
 *  @param globalDefReaderCallbacks Struct for all callbacks.
 *  @param metricMemberCallback     Function which should be called for all
 *                                  @eref{MetricMember} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_GlobalDefReaderCallbacks_SetMetricMemberCallback(
  OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks,
  OTF2_GlobalDefReaderCallback_MetricMember metricMemberCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the @eref{MetricClass} definition.
 *
 *  @param globalDefReaderCallbacks Struct for all callbacks.
 *  @param metricClassCallback      Function which should be called for all
 *                                  @eref{MetricClass} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_GlobalDefReaderCallbacks_SetMetricClassCallback(
  OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks,
  OTF2_GlobalDefReaderCallback_MetricClass metricClassCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the @eref{MetricInstance} definition.
 *
 *  @param globalDefReaderCallbacks Struct for all callbacks.
 *  @param metricInstanceCallback   Function which should be called for all
 *                                  @eref{MetricInstance} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_GlobalDefReaderCallbacks_SetMetricInstanceCallback(
  OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks,
  OTF2_GlobalDefReaderCallback_MetricInstance metricInstanceCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the @eref{Comm} definition.
 *
 *  @param globalDefReaderCallbacks Struct for all callbacks.
 *  @param commCallback             Function which should be called for all
 *                                  @eref{Comm} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_GlobalDefReaderCallbacks_SetCommCallback(OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks,
                                                             OTF2_GlobalDefReaderCallback_Comm commCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the @eref{Parameter} definition.
 *
 *  @param globalDefReaderCallbacks Struct for all callbacks.
 *  @param parameterCallback        Function which should be called for all
 *                                  @eref{Parameter} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_GlobalDefReaderCallbacks_SetParameterCallback(
  OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks,
  OTF2_GlobalDefReaderCallback_Parameter parameterCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the @eref{RmaWin} definition.
 *
 *  @param globalDefReaderCallbacks Struct for all callbacks.
 *  @param rmaWinCallback           Function which should be called for all
 *                                  @eref{RmaWin} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_GlobalDefReaderCallbacks_SetRmaWinCallback(OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks,
                                                               OTF2_GlobalDefReaderCallback_RmaWin rmaWinCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the @eref{MetricClassRecorder} definition.
 *
 *  @param globalDefReaderCallbacks    Struct for all callbacks.
 *  @param metricClassRecorderCallback Function which should be called for all
 *                                     @eref{MetricClassRecorder}
 *                                     definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_GlobalDefReaderCallbacks_SetMetricClassRecorderCallback(
  OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks,
  OTF2_GlobalDefReaderCallback_MetricClassRecorder metricClassRecorderCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the @eref{SystemTreeNodeProperty} definition.
 *
 *  @param globalDefReaderCallbacks       Struct for all callbacks.
 *  @param systemTreeNodePropertyCallback Function which should be called for all
 *                                        @eref{SystemTreeNodeProperty}
 *                                        definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_GlobalDefReaderCallbacks_SetSystemTreeNodePropertyCallback(
  OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks,
  OTF2_GlobalDefReaderCallback_SystemTreeNodeProperty systemTreeNodePropertyCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the @eref{SystemTreeNodeDomain} definition.
 *
 *  @param globalDefReaderCallbacks     Struct for all callbacks.
 *  @param systemTreeNodeDomainCallback Function which should be called for all
 *                                      @eref{SystemTreeNodeDomain}
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
OTF2_ErrorCode OTF2_GlobalDefReaderCallbacks_SetSystemTreeNodeDomainCallback(
  OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks,
  OTF2_GlobalDefReaderCallback_SystemTreeNodeDomain systemTreeNodeDomainCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the @eref{LocationGroupProperty} definition.
 *
 *  @param globalDefReaderCallbacks      Struct for all callbacks.
 *  @param locationGroupPropertyCallback Function which should be called for all
 *                                       @eref{LocationGroupProperty}
 *                                       definitions.
 *
 *  @since Version 1.3
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_GlobalDefReaderCallbacks_SetLocationGroupPropertyCallback(
  OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks,
  OTF2_GlobalDefReaderCallback_LocationGroupProperty locationGroupPropertyCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the @eref{LocationProperty} definition.
 *
 *  @param globalDefReaderCallbacks Struct for all callbacks.
 *  @param locationPropertyCallback Function which should be called for all
 *                                  @eref{LocationProperty} definitions.
 *
 *  @since Version 1.3
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_GlobalDefReaderCallbacks_SetLocationPropertyCallback(
  OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks,
  OTF2_GlobalDefReaderCallback_LocationProperty locationPropertyCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the @eref{CartDimension} definition.
 *
 *  @param globalDefReaderCallbacks Struct for all callbacks.
 *  @param cartDimensionCallback    Function which should be called for all
 *                                  @eref{CartDimension} definitions.
 *
 *  @since Version 1.3
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_GlobalDefReaderCallbacks_SetCartDimensionCallback(
  OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks,
  OTF2_GlobalDefReaderCallback_CartDimension cartDimensionCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the @eref{CartTopology} definition.
 *
 *  @param globalDefReaderCallbacks Struct for all callbacks.
 *  @param cartTopologyCallback     Function which should be called for all
 *                                  @eref{CartTopology} definitions.
 *
 *  @since Version 1.3
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_GlobalDefReaderCallbacks_SetCartTopologyCallback(
  OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks,
  OTF2_GlobalDefReaderCallback_CartTopology cartTopologyCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the @eref{CartCoordinate} definition.
 *
 *  @param globalDefReaderCallbacks Struct for all callbacks.
 *  @param cartCoordinateCallback   Function which should be called for all
 *                                  @eref{CartCoordinate} definitions.
 *
 *  @since Version 1.3
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_GlobalDefReaderCallbacks_SetCartCoordinateCallback(
  OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks,
  OTF2_GlobalDefReaderCallback_CartCoordinate cartCoordinateCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the @eref{SourceCodeLocation} definition.
 *
 *  @param globalDefReaderCallbacks   Struct for all callbacks.
 *  @param sourceCodeLocationCallback Function which should be called for all
 *                                    @eref{SourceCodeLocation} definitions.
 *
 *  @since Version 1.5
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_GlobalDefReaderCallbacks_SetSourceCodeLocationCallback(
  OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks,
  OTF2_GlobalDefReaderCallback_SourceCodeLocation sourceCodeLocationCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the @eref{CallingContext} definition.
 *
 *  @param globalDefReaderCallbacks Struct for all callbacks.
 *  @param callingContextCallback   Function which should be called for all
 *                                  @eref{CallingContext} definitions.
 *
 *  @since Version 1.5
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_GlobalDefReaderCallbacks_SetCallingContextCallback(
  OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks,
  OTF2_GlobalDefReaderCallback_CallingContext callingContextCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the @eref{CallingContextProperty} definition.
 *
 *  @param globalDefReaderCallbacks       Struct for all callbacks.
 *  @param callingContextPropertyCallback Function which should be called for all
 *                                        @eref{CallingContextProperty}
 *                                        definitions.
 *
 *  @since Version 2.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_GlobalDefReaderCallbacks_SetCallingContextPropertyCallback(
  OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks,
  OTF2_GlobalDefReaderCallback_CallingContextProperty callingContextPropertyCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the @eref{InterruptGenerator} definition.
 *
 *  @param globalDefReaderCallbacks   Struct for all callbacks.
 *  @param interruptGeneratorCallback Function which should be called for all
 *                                    @eref{InterruptGenerator} definitions.
 *
 *  @since Version 1.5
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_GlobalDefReaderCallbacks_SetInterruptGeneratorCallback(
  OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks,
  OTF2_GlobalDefReaderCallback_InterruptGenerator interruptGeneratorCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the @eref{IoFileProperty} definition.
 *
 *  @param globalDefReaderCallbacks Struct for all callbacks.
 *  @param ioFilePropertyCallback   Function which should be called for all
 *                                  @eref{IoFileProperty} definitions.
 *
 *  @since Version 2.1
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_GlobalDefReaderCallbacks_SetIoFilePropertyCallback(
  OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks,
  OTF2_GlobalDefReaderCallback_IoFileProperty ioFilePropertyCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the @eref{IoRegularFile} definition.
 *
 *  @param globalDefReaderCallbacks Struct for all callbacks.
 *  @param ioRegularFileCallback    Function which should be called for all
 *                                  @eref{IoRegularFile} definitions.
 *
 *  @since Version 2.1
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_GlobalDefReaderCallbacks_SetIoRegularFileCallback(
  OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks,
  OTF2_GlobalDefReaderCallback_IoRegularFile ioRegularFileCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the @eref{IoDirectory} definition.
 *
 *  @param globalDefReaderCallbacks Struct for all callbacks.
 *  @param ioDirectoryCallback      Function which should be called for all
 *                                  @eref{IoDirectory} definitions.
 *
 *  @since Version 2.1
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_GlobalDefReaderCallbacks_SetIoDirectoryCallback(
  OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks,
  OTF2_GlobalDefReaderCallback_IoDirectory ioDirectoryCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the @eref{IoHandle} definition.
 *
 *  @param globalDefReaderCallbacks Struct for all callbacks.
 *  @param ioHandleCallback         Function which should be called for all
 *                                  @eref{IoHandle} definitions.
 *
 *  @since Version 2.1
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_GlobalDefReaderCallbacks_SetIoHandleCallback(
  OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks,
  OTF2_GlobalDefReaderCallback_IoHandle ioHandleCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the @eref{IoPreCreatedHandleState} definition.
 *
 *  @param globalDefReaderCallbacks        Struct for all callbacks.
 *  @param ioPreCreatedHandleStateCallback Function which should be called for
 *                                         all @eref{IoPreCreatedHandleState}
 *                                         definitions.
 *
 *  @since Version 2.1
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_GlobalDefReaderCallbacks_SetIoPreCreatedHandleStateCallback(
  OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks,
  OTF2_GlobalDefReaderCallback_IoPreCreatedHandleState ioPreCreatedHandleStateCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the @eref{CallpathParameter} definition.
 *
 *  @param globalDefReaderCallbacks  Struct for all callbacks.
 *  @param callpathParameterCallback Function which should be called for all
 *                                   @eref{CallpathParameter} definitions.
 *
 *  @since Version 2.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_GlobalDefReaderCallbacks_SetCallpathParameterCallback(
  OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks,
  OTF2_GlobalDefReaderCallback_CallpathParameter callpathParameterCallback) {
  NOT_IMPLEMENTED;
}

/** @brief Registers the callback for the @eref{InterComm} definition.
 *
 *  @param globalDefReaderCallbacks Struct for all callbacks.
 *  @param interCommCallback        Function which should be called for all
 *                                  @eref{InterComm} definitions.
 *
 *  @since Version 3.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode OTF2_GlobalDefReaderCallbacks_SetInterCommCallback(
  OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks,
  OTF2_GlobalDefReaderCallback_InterComm interCommCallback) {
  NOT_IMPLEMENTED;
}
