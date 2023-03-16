#include <stdlib.h>
#include <stdio.h>

#include "htf.h"
#include "otf2.h"
#include "OTF2_DefReaderCallbacks.h"


OTF2_DefReaderCallbacks*
OTF2_DefReaderCallbacks_New( void ) {
  NOT_IMPLEMENTED;
}



/** @brief Deallocates a struct for the definition callbacks.
 *
 *  @param defReaderCallbacks Handle to a struct previously allocated
 *                            with @eref{OTF2_DefReaderCallbacks_New}.
 */
void
OTF2_DefReaderCallbacks_Delete( OTF2_DefReaderCallbacks* defReaderCallbacks ) {
  NOT_IMPLEMENTED;
}



/** @brief Clears a struct for the definition callbacks.
 *
 *  @param defReaderCallbacks Handle to a struct previously allocated
 *                            with @eref{OTF2_DefReaderCallbacks_New}.
 */
void
OTF2_DefReaderCallbacks_Clear( OTF2_DefReaderCallbacks* defReaderCallbacks ) {
  NOT_IMPLEMENTED;
}


/** @brief Registers the callback for an unknown definition.
 *
 *  @param defReaderCallbacks Struct for all callbacks.
 *  @param unknownCallback    Function which should be called for all
 *                            unknown definitions.
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetUnknownCallback(
    OTF2_DefReaderCallbacks*       defReaderCallbacks,
    OTF2_DefReaderCallback_Unknown unknownCallback ) {
  NOT_IMPLEMENTED;
}


/** @brief Registers the callback for the @eref{MappingTable} definition.
 *
 *  @param defReaderCallbacks   Struct for all callbacks.
 *  @param mappingTableCallback Function which should be called for all
 *                              @eref{MappingTable} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetMappingTableCallback(
    OTF2_DefReaderCallbacks*            defReaderCallbacks,
    OTF2_DefReaderCallback_MappingTable mappingTableCallback ) {
  NOT_IMPLEMENTED;
}


/** @brief Registers the callback for the @eref{ClockOffset} definition.
 *
 *  @param defReaderCallbacks  Struct for all callbacks.
 *  @param clockOffsetCallback Function which should be called for all
 *                             @eref{ClockOffset} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetClockOffsetCallback(
    OTF2_DefReaderCallbacks*           defReaderCallbacks,
    OTF2_DefReaderCallback_ClockOffset clockOffsetCallback ) {
  NOT_IMPLEMENTED;
}



/** @brief Registers the callback for the @eref{String} definition.
 *
 *  @param defReaderCallbacks Struct for all callbacks.
 *  @param stringCallback     Function which should be called for all
 *                            @eref{String} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetStringCallback(
    OTF2_DefReaderCallbacks*      defReaderCallbacks,
    OTF2_DefReaderCallback_String stringCallback ) {
  NOT_IMPLEMENTED;
}




/** @brief Registers the callback for the @eref{Attribute} definition.
 *
 *  @param defReaderCallbacks Struct for all callbacks.
 *  @param attributeCallback  Function which should be called for all
 *                            @eref{Attribute} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetAttributeCallback(
    OTF2_DefReaderCallbacks*         defReaderCallbacks,
    OTF2_DefReaderCallback_Attribute attributeCallback ) {
  NOT_IMPLEMENTED;
}




/** @brief Registers the callback for the @eref{SystemTreeNode} definition.
 *
 *  @param defReaderCallbacks     Struct for all callbacks.
 *  @param systemTreeNodeCallback Function which should be called for all
 *                                @eref{SystemTreeNode} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetSystemTreeNodeCallback(
    OTF2_DefReaderCallbacks*              defReaderCallbacks,
    OTF2_DefReaderCallback_SystemTreeNode systemTreeNodeCallback ) {
  NOT_IMPLEMENTED;
}




/** @brief Registers the callback for the @eref{LocationGroup} definition.
 *
 *  @param defReaderCallbacks    Struct for all callbacks.
 *  @param locationGroupCallback Function which should be called for all
 *                               @eref{LocationGroup} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetLocationGroupCallback(
    OTF2_DefReaderCallbacks*             defReaderCallbacks,
    OTF2_DefReaderCallback_LocationGroup locationGroupCallback ) {
  NOT_IMPLEMENTED;
}




/** @brief Registers the callback for the @eref{Location} definition.
 *
 *  @param defReaderCallbacks Struct for all callbacks.
 *  @param locationCallback   Function which should be called for all
 *                            @eref{Location} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetLocationCallback(
    OTF2_DefReaderCallbacks*        defReaderCallbacks,
    OTF2_DefReaderCallback_Location locationCallback ) {
  NOT_IMPLEMENTED;
}




/** @brief Registers the callback for the @eref{Region} definition.
 *
 *  @param defReaderCallbacks Struct for all callbacks.
 *  @param regionCallback     Function which should be called for all
 *                            @eref{Region} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetRegionCallback(
    OTF2_DefReaderCallbacks*      defReaderCallbacks,
    OTF2_DefReaderCallback_Region regionCallback ) {
  NOT_IMPLEMENTED;
}




/** @brief Registers the callback for the @eref{Callsite} definition.
 *
 *  @param defReaderCallbacks Struct for all callbacks.
 *  @param callsiteCallback   Function which should be called for all
 *                            @eref{Callsite} definitions.
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
OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetCallsiteCallback(
    OTF2_DefReaderCallbacks*        defReaderCallbacks,
    OTF2_DefReaderCallback_Callsite callsiteCallback ) {
  NOT_IMPLEMENTED;
}




/** @brief Registers the callback for the @eref{Callpath} definition.
 *
 *  @param defReaderCallbacks Struct for all callbacks.
 *  @param callpathCallback   Function which should be called for all
 *                            @eref{Callpath} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetCallpathCallback(
    OTF2_DefReaderCallbacks*        defReaderCallbacks,
    OTF2_DefReaderCallback_Callpath callpathCallback ) {
  NOT_IMPLEMENTED;
}




/** @brief Registers the callback for the @eref{Group} definition.
 *
 *  @param defReaderCallbacks Struct for all callbacks.
 *  @param groupCallback      Function which should be called for all
 *                            @eref{Group} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetGroupCallback(
    OTF2_DefReaderCallbacks*     defReaderCallbacks,
    OTF2_DefReaderCallback_Group groupCallback ) {
  NOT_IMPLEMENTED;
}




/** @brief Registers the callback for the @eref{MetricMember} definition.
 *
 *  @param defReaderCallbacks   Struct for all callbacks.
 *  @param metricMemberCallback Function which should be called for all
 *                              @eref{MetricMember} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetMetricMemberCallback(
    OTF2_DefReaderCallbacks*            defReaderCallbacks,
    OTF2_DefReaderCallback_MetricMember metricMemberCallback ) {
  NOT_IMPLEMENTED;
}




/** @brief Registers the callback for the @eref{MetricClass} definition.
 *
 *  @param defReaderCallbacks  Struct for all callbacks.
 *  @param metricClassCallback Function which should be called for all
 *                             @eref{MetricClass} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetMetricClassCallback(
    OTF2_DefReaderCallbacks*           defReaderCallbacks,
    OTF2_DefReaderCallback_MetricClass metricClassCallback ) {
  NOT_IMPLEMENTED;
}




/** @brief Registers the callback for the @eref{MetricInstance} definition.
 *
 *  @param defReaderCallbacks     Struct for all callbacks.
 *  @param metricInstanceCallback Function which should be called for all
 *                                @eref{MetricInstance} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetMetricInstanceCallback(
    OTF2_DefReaderCallbacks*              defReaderCallbacks,
    OTF2_DefReaderCallback_MetricInstance metricInstanceCallback ) {
  NOT_IMPLEMENTED;
}




/** @brief Registers the callback for the @eref{Comm} definition.
 *
 *  @param defReaderCallbacks Struct for all callbacks.
 *  @param commCallback       Function which should be called for all @eref{Comm}
 *                            definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetCommCallback(
    OTF2_DefReaderCallbacks*    defReaderCallbacks,
    OTF2_DefReaderCallback_Comm commCallback ) {
  NOT_IMPLEMENTED;
}




/** @brief Registers the callback for the @eref{Parameter} definition.
 *
 *  @param defReaderCallbacks Struct for all callbacks.
 *  @param parameterCallback  Function which should be called for all
 *                            @eref{Parameter} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetParameterCallback(
    OTF2_DefReaderCallbacks*         defReaderCallbacks,
    OTF2_DefReaderCallback_Parameter parameterCallback ) {
  NOT_IMPLEMENTED;
}




/** @brief Registers the callback for the @eref{RmaWin} definition.
 *
 *  @param defReaderCallbacks Struct for all callbacks.
 *  @param rmaWinCallback     Function which should be called for all
 *                            @eref{RmaWin} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetRmaWinCallback(
    OTF2_DefReaderCallbacks*      defReaderCallbacks,
    OTF2_DefReaderCallback_RmaWin rmaWinCallback ) {
  NOT_IMPLEMENTED;
}




/** @brief Registers the callback for the @eref{MetricClassRecorder} definition.
 *
 *  @param defReaderCallbacks          Struct for all callbacks.
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
OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetMetricClassRecorderCallback(
    OTF2_DefReaderCallbacks*                   defReaderCallbacks,
    OTF2_DefReaderCallback_MetricClassRecorder metricClassRecorderCallback ) {
  NOT_IMPLEMENTED;
}




/** @brief Registers the callback for the @eref{SystemTreeNodeProperty} definition.
 *
 *  @param defReaderCallbacks             Struct for all callbacks.
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
OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetSystemTreeNodePropertyCallback(
    OTF2_DefReaderCallbacks*                      defReaderCallbacks,
    OTF2_DefReaderCallback_SystemTreeNodeProperty systemTreeNodePropertyCallback ) {
  NOT_IMPLEMENTED;
}




/** @brief Registers the callback for the @eref{SystemTreeNodeDomain} definition.
 *
 *  @param defReaderCallbacks           Struct for all callbacks.
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
OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetSystemTreeNodeDomainCallback(
    OTF2_DefReaderCallbacks*                    defReaderCallbacks,
    OTF2_DefReaderCallback_SystemTreeNodeDomain systemTreeNodeDomainCallback ) {
  NOT_IMPLEMENTED;
}




/** @brief Registers the callback for the @eref{LocationGroupProperty} definition.
 *
 *  @param defReaderCallbacks            Struct for all callbacks.
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
OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetLocationGroupPropertyCallback(
    OTF2_DefReaderCallbacks*                     defReaderCallbacks,
    OTF2_DefReaderCallback_LocationGroupProperty locationGroupPropertyCallback ) {
  NOT_IMPLEMENTED;
}




/** @brief Registers the callback for the @eref{LocationProperty} definition.
 *
 *  @param defReaderCallbacks       Struct for all callbacks.
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
OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetLocationPropertyCallback(
    OTF2_DefReaderCallbacks*                defReaderCallbacks,
    OTF2_DefReaderCallback_LocationProperty locationPropertyCallback ) {
  NOT_IMPLEMENTED;
}




/** @brief Registers the callback for the @eref{CartDimension} definition.
 *
 *  @param defReaderCallbacks    Struct for all callbacks.
 *  @param cartDimensionCallback Function which should be called for all
 *                               @eref{CartDimension} definitions.
 *
 *  @since Version 1.3
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetCartDimensionCallback(
    OTF2_DefReaderCallbacks*             defReaderCallbacks,
    OTF2_DefReaderCallback_CartDimension cartDimensionCallback ) {
  NOT_IMPLEMENTED;
}




/** @brief Registers the callback for the @eref{CartTopology} definition.
 *
 *  @param defReaderCallbacks   Struct for all callbacks.
 *  @param cartTopologyCallback Function which should be called for all
 *                              @eref{CartTopology} definitions.
 *
 *  @since Version 1.3
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetCartTopologyCallback(
    OTF2_DefReaderCallbacks*            defReaderCallbacks,
    OTF2_DefReaderCallback_CartTopology cartTopologyCallback ) {
  NOT_IMPLEMENTED;
}




/** @brief Registers the callback for the @eref{CartCoordinate} definition.
 *
 *  @param defReaderCallbacks     Struct for all callbacks.
 *  @param cartCoordinateCallback Function which should be called for all
 *                                @eref{CartCoordinate} definitions.
 *
 *  @since Version 1.3
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetCartCoordinateCallback(
    OTF2_DefReaderCallbacks*              defReaderCallbacks,
    OTF2_DefReaderCallback_CartCoordinate cartCoordinateCallback ) {
  NOT_IMPLEMENTED;
}




/** @brief Registers the callback for the @eref{SourceCodeLocation} definition.
 *
 *  @param defReaderCallbacks         Struct for all callbacks.
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
OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetSourceCodeLocationCallback(
    OTF2_DefReaderCallbacks*                  defReaderCallbacks,
    OTF2_DefReaderCallback_SourceCodeLocation sourceCodeLocationCallback ) {
  NOT_IMPLEMENTED;
}




/** @brief Registers the callback for the @eref{CallingContext} definition.
 *
 *  @param defReaderCallbacks     Struct for all callbacks.
 *  @param callingContextCallback Function which should be called for all
 *                                @eref{CallingContext} definitions.
 *
 *  @since Version 1.5
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetCallingContextCallback(
    OTF2_DefReaderCallbacks*              defReaderCallbacks,
    OTF2_DefReaderCallback_CallingContext callingContextCallback ) {
  NOT_IMPLEMENTED;
}




/** @brief Registers the callback for the @eref{CallingContextProperty} definition.
 *
 *  @param defReaderCallbacks             Struct for all callbacks.
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
OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetCallingContextPropertyCallback(
    OTF2_DefReaderCallbacks*                      defReaderCallbacks,
    OTF2_DefReaderCallback_CallingContextProperty callingContextPropertyCallback ) {
  NOT_IMPLEMENTED;
}




/** @brief Registers the callback for the @eref{InterruptGenerator} definition.
 *
 *  @param defReaderCallbacks         Struct for all callbacks.
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
OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetInterruptGeneratorCallback(
    OTF2_DefReaderCallbacks*                  defReaderCallbacks,
    OTF2_DefReaderCallback_InterruptGenerator interruptGeneratorCallback ) {
  NOT_IMPLEMENTED;
}




/** @brief Registers the callback for the @eref{IoFileProperty} definition.
 *
 *  @param defReaderCallbacks     Struct for all callbacks.
 *  @param ioFilePropertyCallback Function which should be called for all
 *                                @eref{IoFileProperty} definitions.
 *
 *  @since Version 2.1
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetIoFilePropertyCallback(
    OTF2_DefReaderCallbacks*              defReaderCallbacks,
    OTF2_DefReaderCallback_IoFileProperty ioFilePropertyCallback ) {
  NOT_IMPLEMENTED;
}




/** @brief Registers the callback for the @eref{IoRegularFile} definition.
 *
 *  @param defReaderCallbacks    Struct for all callbacks.
 *  @param ioRegularFileCallback Function which should be called for all
 *                               @eref{IoRegularFile} definitions.
 *
 *  @since Version 2.1
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetIoRegularFileCallback(
    OTF2_DefReaderCallbacks*             defReaderCallbacks,
    OTF2_DefReaderCallback_IoRegularFile ioRegularFileCallback ) {
  NOT_IMPLEMENTED;
}




/** @brief Registers the callback for the @eref{IoDirectory} definition.
 *
 *  @param defReaderCallbacks  Struct for all callbacks.
 *  @param ioDirectoryCallback Function which should be called for all
 *                             @eref{IoDirectory} definitions.
 *
 *  @since Version 2.1
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetIoDirectoryCallback(
    OTF2_DefReaderCallbacks*           defReaderCallbacks,
    OTF2_DefReaderCallback_IoDirectory ioDirectoryCallback ) {
  NOT_IMPLEMENTED;
}




/** @brief Registers the callback for the @eref{IoHandle} definition.
 *
 *  @param defReaderCallbacks Struct for all callbacks.
 *  @param ioHandleCallback   Function which should be called for all
 *                            @eref{IoHandle} definitions.
 *
 *  @since Version 2.1
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetIoHandleCallback(
    OTF2_DefReaderCallbacks*        defReaderCallbacks,
    OTF2_DefReaderCallback_IoHandle ioHandleCallback ) {
  NOT_IMPLEMENTED;
}




/** @brief Registers the callback for the @eref{IoPreCreatedHandleState} definition.
 *
 *  @param defReaderCallbacks              Struct for all callbacks.
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
OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetIoPreCreatedHandleStateCallback(
    OTF2_DefReaderCallbacks*                       defReaderCallbacks,
    OTF2_DefReaderCallback_IoPreCreatedHandleState ioPreCreatedHandleStateCallback ) {
  NOT_IMPLEMENTED;
}




/** @brief Registers the callback for the @eref{CallpathParameter} definition.
 *
 *  @param defReaderCallbacks        Struct for all callbacks.
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
OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetCallpathParameterCallback(
    OTF2_DefReaderCallbacks*                 defReaderCallbacks,
    OTF2_DefReaderCallback_CallpathParameter callpathParameterCallback ) {
  NOT_IMPLEMENTED;
}




/** @brief Regiseters the callback for the @eref{InterComm} definition.
 *
 *  @param defReaderCallbacks Struct for all callbacks.
 *  @param interCommCallback  Function which should be called for all
 *                            @eref{InterComm} definitions.
 *
 *  @since Version 3.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetInterCommCallback(
    OTF2_DefReaderCallbacks*         defReaderCallbacks,
    OTF2_DefReaderCallback_InterComm interCommCallback ) {
  NOT_IMPLEMENTED;
}
