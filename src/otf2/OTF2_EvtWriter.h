/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2021,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


#ifndef OTF2_EVT_WRITER_H
#define OTF2_EVT_WRITER_H


/**
 *  @file
 *  @source     templates/OTF2_EvtWriter.tmpl.h
 *
 *  @brief      This lowest user-visible layer provides write routines to
 *              write event data of a single location.
 */



#include <stdint.h>


#include "otf2_compiler.h"


#include <otf2/OTF2_ErrorCodes.h>


#include <otf2/OTF2_Events.h>
#include <otf2/OTF2_AttributeList.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @brief Keeps all necessary information about the event writer.
 *  See OTF2_EvtWriter_struct for detailed information. */
typedef struct OTF2_EvtWriter_struct OTF2_EvtWriter;


/** @brief
 *  Function to get the location ID of a writer object.
 *
 *  @param writer       Writer object which has to be deleted
 *  @param locationID   Pointer to a variable where the ID is returned in
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_GetLocationID( const OTF2_EvtWriter* writer,
                              OTF2_LocationRef*     locationID );


/** @brief
 *  Function to set user defined data to a writer object.
 *
 *  @param writer   Writer object.
 *  @param userData User provided data. Can be queried with
 *                  @eref{OTF2_EvtWriter_GetUserData}.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_SetUserData( OTF2_EvtWriter* writer,
                            void*           userData );


/** @brief
 *  Function to get the location of a writer object.
 *
 *  @param writer        Writer object.
 *  @param[out] userData Pointer to a variable where the pointer to the location is returned in.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_GetUserData( const OTF2_EvtWriter* writer,
                            void**                userData );


/** @brief
 *  The location ID is not always known on measurement start, and only needed
 *  on the first buffer flush to generate the file name. This function enables
 *  setting of the location ID after generating the buffer object.
 *
 *  @param writer      Writer object.
 *  @param location    Location ID.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_SetLocationID( OTF2_EvtWriter*  writer,
                              OTF2_LocationRef location );


/** @brief Get the number of events.
 *
 *  Get the number of events written with this event writer. You should call
 *  this function right before closing the event writer to get the correct
 *  number of stored event records.
 *
 *  @param writer              Writer object.
 *  @param[out] numberOfEvents Return pointer to the number of events.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_GetNumberOfEvents( OTF2_EvtWriter* writer,
                                  uint64_t*       numberOfEvents );


/** @brief Records a BufferFlush event.
 *
 *  This event signals that the internal buffer was flushed at the given
 *  time.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param stopTime      The time the buffer flush finished.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_BufferFlush( OTF2_EvtWriter*     writer,
                            OTF2_AttributeList* attributeList,
                            OTF2_TimeStamp      time,
                            OTF2_TimeStamp      stopTime );


/** @brief Records a MeasurementOnOff event.
 *
 *  This event signals where the measurement system turned measurement on
 *  or off.
 *
 *  @param writer          Writer object.
 *  @param attributeList   Generic attributes for the event.
 *  @param time            The time for this event.
 *  @param measurementMode Is the measurement turned on
 *                         (@eref{OTF2_MEASUREMENT_ON}) or off
 *                         (@eref{OTF2_MEASUREMENT_OFF})?
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_MeasurementOnOff( OTF2_EvtWriter*      writer,
                                 OTF2_AttributeList*  attributeList,
                                 OTF2_TimeStamp       time,
                                 OTF2_MeasurementMode measurementMode );


/** @brief Records a Enter event.
 *
 *  An @e Enter record indicates that the program enters a code region.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param region        Needs to be defined in a definition record References a
 *                       @eref{Region} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_REGION} is available.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_Enter( OTF2_EvtWriter*     writer,
                      OTF2_AttributeList* attributeList,
                      OTF2_TimeStamp      time,
                      OTF2_RegionRef      region );


/** @brief Records a Leave event.
 *
 *  A @e Leave record indicates that the program leaves a code region.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param region        Needs to be defined in a definition record References a
 *                       @eref{Region} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_REGION} is available.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_Leave( OTF2_EvtWriter*     writer,
                      OTF2_AttributeList* attributeList,
                      OTF2_TimeStamp      time,
                      OTF2_RegionRef      region );


/** @brief Records a MpiSend event.
 *
 *  An @e MpiSend record indicates that an MPI send operation was
 *  initiated (MPI_SEND). It keeps the necessary information for this
 *  event: receiver of the message, communicator, and the message tag.
 *  You can optionally add further information like the message length
 *  (size of the send buffer).
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param receiver      MPI rank of receiver in @p communicator.
 *  @param communicator  Communicator ID. References a @eref{Comm}, or a
 *                       @eref{InterComm} definition and will be mapped to
 *                       the global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_COMM} is available.
 *  @param msgTag        Message tag
 *  @param msgLength     Message length
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_MpiSend( OTF2_EvtWriter*     writer,
                        OTF2_AttributeList* attributeList,
                        OTF2_TimeStamp      time,
                        uint32_t            receiver,
                        OTF2_CommRef        communicator,
                        uint32_t            msgTag,
                        uint64_t            msgLength );


/** @brief Records a MpiIsend event.
 *
 *  An @e MpiIsend record indicates that a non-blocking MPI send operation
 *  was initiated (MPI_ISEND). It keeps the necessary information for
 *  this event: receiver of the message, communicator, and the message
 *  tag. You can optionally add further information like the message
 *  length (size of the send buffer).
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param receiver      MPI rank of receiver in @p communicator.
 *  @param communicator  Communicator ID. References a @eref{Comm}, or a
 *                       @eref{InterComm} definition and will be mapped to
 *                       the global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_COMM} is available.
 *  @param msgTag        Message tag
 *  @param msgLength     Message length
 *  @param requestID     ID of the related request
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_MpiIsend( OTF2_EvtWriter*     writer,
                         OTF2_AttributeList* attributeList,
                         OTF2_TimeStamp      time,
                         uint32_t            receiver,
                         OTF2_CommRef        communicator,
                         uint32_t            msgTag,
                         uint64_t            msgLength,
                         uint64_t            requestID );


/** @brief Records a MpiIsendComplete event.
 *
 *  An @e MpiIsendComplete record indicates the completion of a non-
 *  blocking MPI send operation. In the case where the send request is
 *  released before it is completed by MPI, this record will only
 *  indicate the release, as it becomes impossible to track the
 *  completion of the send operation afterwards. This case may be
 *  identified by the surrounding events.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param requestID     ID of the related request
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_MpiIsendComplete( OTF2_EvtWriter*     writer,
                                 OTF2_AttributeList* attributeList,
                                 OTF2_TimeStamp      time,
                                 uint64_t            requestID );


/** @brief Records a MpiIrecvRequest event.
 *
 *  An @e MpiIrecvRequest record indicates that a non-blocking MPI receive
 *  operation was initiated (MPI_IRECV).
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param requestID     ID of the requested receive
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_MpiIrecvRequest( OTF2_EvtWriter*     writer,
                                OTF2_AttributeList* attributeList,
                                OTF2_TimeStamp      time,
                                uint64_t            requestID );


/** @brief Records a MpiRecv event.
 *
 *  An @e MpiRecv record indicates that an MPI message was received
 *  (MPI_RECV). It keeps the necessary information for this event:
 *  sender of the message, communicator, and the message tag. You can
 *  optionally add further information like the message length (size
 *  of the receive buffer).
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param sender        MPI rank of sender in @p communicator.
 *  @param communicator  Communicator ID. References a @eref{Comm}, or a
 *                       @eref{InterComm} definition and will be mapped to
 *                       the global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_COMM} is available.
 *  @param msgTag        Message tag
 *  @param msgLength     Message length
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_MpiRecv( OTF2_EvtWriter*     writer,
                        OTF2_AttributeList* attributeList,
                        OTF2_TimeStamp      time,
                        uint32_t            sender,
                        OTF2_CommRef        communicator,
                        uint32_t            msgTag,
                        uint64_t            msgLength );


/** @brief Records a MpiIrecv event.
 *
 *  An @e MpiIrecv record indicates the completion of a non-blocking MPI
 *  receive operation completed (MPI_IRECV). It keeps the necessary
 *  information for this event: sender of the message, communicator,
 *  and the message tag. You can optionally add further information
 *  like the message length (size of the receive buffer).
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param sender        MPI rank of sender in @p communicator.
 *  @param communicator  Communicator ID. References a @eref{Comm}, or a
 *                       @eref{InterComm} definition and will be mapped to
 *                       the global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_COMM} is available.
 *  @param msgTag        Message tag
 *  @param msgLength     Message length
 *  @param requestID     ID of the related request
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_MpiIrecv( OTF2_EvtWriter*     writer,
                         OTF2_AttributeList* attributeList,
                         OTF2_TimeStamp      time,
                         uint32_t            sender,
                         OTF2_CommRef        communicator,
                         uint32_t            msgTag,
                         uint64_t            msgLength,
                         uint64_t            requestID );


/** @brief Records a MpiRequestTest event.
 *
 *  This events appears if the program tests if a request has already
 *  completed but the test failed.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param requestID     ID of the related request
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_MpiRequestTest( OTF2_EvtWriter*     writer,
                               OTF2_AttributeList* attributeList,
                               OTF2_TimeStamp      time,
                               uint64_t            requestID );


/** @brief Records a MpiRequestCancelled event.
 *
 *  This events appears if the program canceled a request.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param requestID     ID of the related request
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_MpiRequestCancelled( OTF2_EvtWriter*     writer,
                                    OTF2_AttributeList* attributeList,
                                    OTF2_TimeStamp      time,
                                    uint64_t            requestID );


/** @brief Records a MpiCollectiveBegin event.
 *
 *  An @e MpiCollectiveBegin record marks the begin of an MPI collective
 *  operation (MPI_GATHER, MPI_SCATTER etc.).
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_MpiCollectiveBegin( OTF2_EvtWriter*     writer,
                                   OTF2_AttributeList* attributeList,
                                   OTF2_TimeStamp      time );


/** @brief Records a MpiCollectiveEnd event.
 *
 *  An @e MpiCollectiveEnd record marks the end of an MPI collective
 *  operation (MPI_GATHER, MPI_SCATTER etc.). It keeps the necessary
 *  information for this event: type of collective operation,
 *  communicator, the root of this collective operation. You can
 *  optionally add further information like sent and received bytes.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param collectiveOp  Determines which collective operation it is.
 *  @param communicator  Communicator References a @eref{Comm}, or a
 *                       @eref{InterComm} definition and will be mapped to
 *                       the global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_COMM} is available.
 *  @param root          Rank of root in @p communicator or any predefined
 *                       constant of @eref{OTF2_CollectiveRoot}.
 *  @param sizeSent      Size of the sent message.
 *  @param sizeReceived  Size of the received message.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_MpiCollectiveEnd( OTF2_EvtWriter*     writer,
                                 OTF2_AttributeList* attributeList,
                                 OTF2_TimeStamp      time,
                                 OTF2_CollectiveOp   collectiveOp,
                                 OTF2_CommRef        communicator,
                                 uint32_t            root,
                                 uint64_t            sizeSent,
                                 uint64_t            sizeReceived );


/** @brief Records a OmpFork event.
 *
 *  An @e OmpFork record marks that an OpenMP Thread forks a thread team.
 *
 *  This event record is superseded by the @eref{ThreadFork} event record
 *  and should not be used when the @eref{ThreadFork} event record is
 *  in use.
 *
 *  @param writer                   Writer object.
 *  @param attributeList            Generic attributes for the event.
 *  @param time                     The time for this event.
 *  @param numberOfRequestedThreads Requested size of the team.
 *
 *  @since Version 1.0
 *
 *  @deprecated In version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_OmpFork( OTF2_EvtWriter*     writer,
                        OTF2_AttributeList* attributeList,
                        OTF2_TimeStamp      time,
                        uint32_t            numberOfRequestedThreads ) __otf2_deprecated__;


/** @brief Records a OmpJoin event.
 *
 *  An @e OmpJoin record marks that a team of threads is joint and only
 *  the master thread continues execution.
 *
 *  This event record is superseded by the @eref{ThreadJoin} event record
 *  and should not be used when the @eref{ThreadJoin} event record is
 *  in use.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *
 *  @since Version 1.0
 *
 *  @deprecated In version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_OmpJoin( OTF2_EvtWriter*     writer,
                        OTF2_AttributeList* attributeList,
                        OTF2_TimeStamp      time ) __otf2_deprecated__;


/** @brief Records a OmpAcquireLock event.
 *
 *  An @e OmpAcquireLock record marks that a thread acquires an OpenMP
 *  lock.
 *
 *  This event record is superseded by the @eref{ThreadAcquireLock} event
 *  record and should not be used when the @eref{ThreadAcquireLock}
 *  event record is in use.
 *
 *  @param writer           Writer object.
 *  @param attributeList    Generic attributes for the event.
 *  @param time             The time for this event.
 *  @param lockID           ID of the lock.
 *  @param acquisitionOrder A monotonically increasing number to determine the
 *                          order of lock acquisitions (with unsynchronized
 *                          clocks this is otherwise not possible).
 *                          Corresponding acquire-release events have same
 *                          number.
 *
 *  @since Version 1.0
 *
 *  @deprecated In version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_OmpAcquireLock( OTF2_EvtWriter*     writer,
                               OTF2_AttributeList* attributeList,
                               OTF2_TimeStamp      time,
                               uint32_t            lockID,
                               uint32_t            acquisitionOrder ) __otf2_deprecated__;


/** @brief Records a OmpReleaseLock event.
 *
 *  An @e OmpReleaseLock record marks that a thread releases an OpenMP
 *  lock.
 *
 *  This event record is superseded by the @eref{ThreadReleaseLock} event
 *  record and should not be used when the @eref{ThreadReleaseLock}
 *  event record is in use.
 *
 *  @param writer           Writer object.
 *  @param attributeList    Generic attributes for the event.
 *  @param time             The time for this event.
 *  @param lockID           ID of the lock.
 *  @param acquisitionOrder A monotonically increasing number to determine the
 *                          order of lock acquisitions (with unsynchronized
 *                          clocks this is otherwise not possible).
 *                          Corresponding acquire-release events have same
 *                          number.
 *
 *  @since Version 1.0
 *
 *  @deprecated In version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_OmpReleaseLock( OTF2_EvtWriter*     writer,
                               OTF2_AttributeList* attributeList,
                               OTF2_TimeStamp      time,
                               uint32_t            lockID,
                               uint32_t            acquisitionOrder ) __otf2_deprecated__;


/** @brief Records a OmpTaskCreate event.
 *
 *  An @e OmpTaskCreate record marks that an OpenMP Task was/will be
 *  created in the current region.
 *
 *  This event record is superseded by the @eref{ThreadTaskCreate} event
 *  record and should not be used when the @eref{ThreadTaskCreate}
 *  event record is in use.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param taskID        Identifier of the newly created task instance.
 *
 *  @since Version 1.0
 *
 *  @deprecated In version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_OmpTaskCreate( OTF2_EvtWriter*     writer,
                              OTF2_AttributeList* attributeList,
                              OTF2_TimeStamp      time,
                              uint64_t            taskID ) __otf2_deprecated__;


/** @brief Records a OmpTaskSwitch event.
 *
 *  An @e OmpTaskSwitch record indicates that the execution of the current
 *  task will be suspended and another task starts/restarts its
 *  execution. Please note that this may change the current call stack
 *  of the executing location.
 *
 *  This event record is superseded by the @eref{ThreadTaskSwitch} event
 *  record and should not be used when the @eref{ThreadTaskSwitch}
 *  event record is in use.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param taskID        Identifier of the now active task instance.
 *
 *  @since Version 1.0
 *
 *  @deprecated In version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_OmpTaskSwitch( OTF2_EvtWriter*     writer,
                              OTF2_AttributeList* attributeList,
                              OTF2_TimeStamp      time,
                              uint64_t            taskID ) __otf2_deprecated__;


/** @brief Records a OmpTaskComplete event.
 *
 *  An @e OmpTaskComplete record indicates that the execution of an OpenMP
 *  task has finished.
 *
 *  This event record is superseded by the @eref{ThreadTaskComplete} event
 *  record and should not be used when the @eref{ThreadTaskComplete}
 *  event record is in use.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param taskID        Identifier of the completed task instance.
 *
 *  @since Version 1.0
 *
 *  @deprecated In version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_OmpTaskComplete( OTF2_EvtWriter*     writer,
                                OTF2_AttributeList* attributeList,
                                OTF2_TimeStamp      time,
                                uint64_t            taskID ) __otf2_deprecated__;


/** @brief Records a Metric event.
 *
 *  A @e Metric event is always stored at the location that recorded the
 *  metric. The event can reference a @eref{MetricClass} or
 *  @eref{MetricInstance}. Therefore, metric classes and instances
 *  share same ID space. Synchronous metrics are always located right
 *  immediately the corresponding @eref{Enter} and @eref{Leave} event.
 *
 *  @param writer          Writer object.
 *  @param attributeList   Generic attributes for the event.
 *  @param time            The time for this event.
 *  @param metric          Could be a metric class or a metric instance.
 *                         References a @eref{MetricClass}, or a
 *                         @eref{MetricInstance} definition and will be
 *                         mapped to the global definition if a mapping table
 *                         of type @eref{OTF2_MAPPING_METRIC} is available.
 *  @param numberOfMetrics Number of metrics with in the set.
 *  @param typeIDs         List of metric types. These types must match that of
 *                         the corresponding @eref{MetricMember} definitions.
 *  @param metricValues    List of metric values.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_Metric( OTF2_EvtWriter*         writer,
                       OTF2_AttributeList*     attributeList,
                       OTF2_TimeStamp          time,
                       OTF2_MetricRef          metric,
                       uint8_t                 numberOfMetrics,
                       const OTF2_Type*        typeIDs,
                       const OTF2_MetricValue* metricValues );


/** @brief Records a ParameterString event.
 *
 *  A @e ParameterString record marks that in the current region, the
 *  specified string parameter has the specified value.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param parameter     Parameter ID. References a @eref{Parameter} definition
 *                       and will be mapped to the global definition if a
 *                       mapping table of type @eref{OTF2_MAPPING_PARAMETER}
 *                       is available.
 *  @param string        Value: Handle of a string definition References a
 *                       @eref{String} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_STRING} is available.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_ParameterString( OTF2_EvtWriter*     writer,
                                OTF2_AttributeList* attributeList,
                                OTF2_TimeStamp      time,
                                OTF2_ParameterRef   parameter,
                                OTF2_StringRef      string );


/** @brief Records a ParameterInt event.
 *
 *  A @e ParameterInt record marks that in the current region, the
 *  specified integer parameter has the specified value.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param parameter     Parameter ID. References a @eref{Parameter} definition
 *                       and will be mapped to the global definition if a
 *                       mapping table of type @eref{OTF2_MAPPING_PARAMETER}
 *                       is available.
 *  @param value         Value of the recorded parameter.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_ParameterInt( OTF2_EvtWriter*     writer,
                             OTF2_AttributeList* attributeList,
                             OTF2_TimeStamp      time,
                             OTF2_ParameterRef   parameter,
                             int64_t             value );


/** @brief Records a ParameterUnsignedInt event.
 *
 *  A @e ParameterUnsignedInt record marks that in the current region, the
 *  specified unsigned integer parameter has the specified value.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param parameter     Parameter ID. References a @eref{Parameter} definition
 *                       and will be mapped to the global definition if a
 *                       mapping table of type @eref{OTF2_MAPPING_PARAMETER}
 *                       is available.
 *  @param value         Value of the recorded parameter.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_ParameterUnsignedInt( OTF2_EvtWriter*     writer,
                                     OTF2_AttributeList* attributeList,
                                     OTF2_TimeStamp      time,
                                     OTF2_ParameterRef   parameter,
                                     uint64_t            value );


/** @brief Records a RmaWinCreate event.
 *
 *  An @e RmaWinCreate record denotes the creation of an RMA window. Only
 *  valid if the @eref{RmaWin} definition was flagged with
 *  @eref{OTF2_RMA_WIN_FLAG_CREATE_DESTROY_EVENTS}. This event can be
 *  enclosed by an @eref{RmaCollectiveBegin} and
 *  @eref{RmaCollectiveEnd} event pair either with
 *  @eref{OTF2_COLLECTIVE_OP_CREATE_HANDLE} or
 *  @eref{OTF2_COLLECTIVE_OP_CREATE_HANDLE_AND_ALLOCATE} as the
 *  operation type.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param win           ID of the window created. References a @eref{RmaWin}
 *                       definition and will be mapped to the global
 *                       definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_RMA_WIN} is available.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_RmaWinCreate( OTF2_EvtWriter*     writer,
                             OTF2_AttributeList* attributeList,
                             OTF2_TimeStamp      time,
                             OTF2_RmaWinRef      win );


/** @brief Records a RmaWinDestroy event.
 *
 *  An @e RmaWinDestroy record denotes the destruction of an RMA window.
 *  Only valid if the @eref{RmaWin} definition was flagged with
 *  @eref{OTF2_RMA_WIN_FLAG_CREATE_DESTROY_EVENTS}. This event can be
 *  enclosed by an @eref{RmaCollectiveBegin} and
 *  @eref{RmaCollectiveEnd} event pair either with
 *  @eref{OTF2_COLLECTIVE_OP_DESTROY_HANDLE} or
 *  @eref{OTF2_COLLECTIVE_OP_DESTROY_HANDLE_AND_DEALLOCATE} as the
 *  operation type. In this case the RMA window is only marked for
 *  destruction, which will happen with the @eref{RmaCollectiveEnd}
 *  event.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param win           ID of the window destructed. References a @eref{RmaWin}
 *                       definition and will be mapped to the global
 *                       definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_RMA_WIN} is available.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_RmaWinDestroy( OTF2_EvtWriter*     writer,
                              OTF2_AttributeList* attributeList,
                              OTF2_TimeStamp      time,
                              OTF2_RmaWinRef      win );


/** @brief Records a RmaCollectiveBegin event.
 *
 *  An @e RmaCollectiveBegin record denotes the beginning of a collective
 *  RMA operation.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_RmaCollectiveBegin( OTF2_EvtWriter*     writer,
                                   OTF2_AttributeList* attributeList,
                                   OTF2_TimeStamp      time );


/** @brief Records a RmaCollectiveEnd event.
 *
 *  An @e RmaCollectiveEnd record denotes the end of a collective RMA
 *  operation.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param collectiveOp  Determines which collective operation it is.
 *  @param syncLevel     Synchronization level of this collective operation.
 *  @param win           ID of the window used for this operation. References a
 *                       @eref{RmaWin} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_RMA_WIN} is available.
 *  @param root          Root process for this operation or any predefined
 *                       constant of @eref{OTF2_CollectiveRoot}.
 *  @param bytesSent     Bytes sent in operation.
 *  @param bytesReceived Bytes receives in operation.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_RmaCollectiveEnd( OTF2_EvtWriter*     writer,
                                 OTF2_AttributeList* attributeList,
                                 OTF2_TimeStamp      time,
                                 OTF2_CollectiveOp   collectiveOp,
                                 OTF2_RmaSyncLevel   syncLevel,
                                 OTF2_RmaWinRef      win,
                                 uint32_t            root,
                                 uint64_t            bytesSent,
                                 uint64_t            bytesReceived );


/** @brief Records a RmaGroupSync event.
 *
 *  An @e RmaGroupSync record denotes the synchronization with a subgroup
 *  of processes on an RMA window.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param syncLevel     Synchronization level of this collective operation.
 *  @param win           ID of the window used for this operation. References a
 *                       @eref{RmaWin} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_RMA_WIN} is available.
 *  @param group         Group of remote processes involved in synchronization.
 *                       References a @eref{Group} definition and will be
 *                       mapped to the global definition if a mapping table
 *                       of type @eref{OTF2_MAPPING_GROUP} is available.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_RmaGroupSync( OTF2_EvtWriter*     writer,
                             OTF2_AttributeList* attributeList,
                             OTF2_TimeStamp      time,
                             OTF2_RmaSyncLevel   syncLevel,
                             OTF2_RmaWinRef      win,
                             OTF2_GroupRef       group );


/** @brief Records a RmaRequestLock event.
 *
 *  An @e RmaRequestLock record denotes the time a lock was requested and
 *  with it the earliest time it could have been granted.  It is used
 *  to mark (possibly) non-blocking lock request, as defined by the
 *  MPI standard.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param win           ID of the window used for this operation. References a
 *                       @eref{RmaWin} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_RMA_WIN} is available.
 *  @param remote        Rank of the locked remote process or
 *                       @eref{OTF2_UNDEFINED_UINT32} if all processes of the
 *                       specified window are locked.
 *  @param lockId        ID of the lock acquired, if multiple locks are defined
 *                       on a window.
 *  @param lockType      Type of lock acquired.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_RmaRequestLock( OTF2_EvtWriter*     writer,
                               OTF2_AttributeList* attributeList,
                               OTF2_TimeStamp      time,
                               OTF2_RmaWinRef      win,
                               uint32_t            remote,
                               uint64_t            lockId,
                               OTF2_LockType       lockType );


/** @brief Records a RmaAcquireLock event.
 *
 *  An @e RmaAcquireLock record denotes the time a lock was acquired by
 *  the process.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param win           ID of the window used for this operation. References a
 *                       @eref{RmaWin} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_RMA_WIN} is available.
 *  @param remote        Rank of the locked remote process or
 *                       @eref{OTF2_UNDEFINED_UINT32} if all processes of the
 *                       specified window are locked.
 *  @param lockId        ID of the lock acquired, if multiple locks are defined
 *                       on a window.
 *  @param lockType      Type of lock acquired.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_RmaAcquireLock( OTF2_EvtWriter*     writer,
                               OTF2_AttributeList* attributeList,
                               OTF2_TimeStamp      time,
                               OTF2_RmaWinRef      win,
                               uint32_t            remote,
                               uint64_t            lockId,
                               OTF2_LockType       lockType );


/** @brief Records a RmaTryLock event.
 *
 *  An @e RmaTryLock record denotes the time of an unsuccessful attempt to
 *  acquire the lock.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param win           ID of the window used for this operation. References a
 *                       @eref{RmaWin} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_RMA_WIN} is available.
 *  @param remote        Rank of the locked remote process or
 *                       @eref{OTF2_UNDEFINED_UINT32} if all processes of the
 *                       specified window are locked.
 *  @param lockId        ID of the lock acquired, if multiple locks are defined
 *                       on a window.
 *  @param lockType      Type of lock acquired.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_RmaTryLock( OTF2_EvtWriter*     writer,
                           OTF2_AttributeList* attributeList,
                           OTF2_TimeStamp      time,
                           OTF2_RmaWinRef      win,
                           uint32_t            remote,
                           uint64_t            lockId,
                           OTF2_LockType       lockType );


/** @brief Records a RmaReleaseLock event.
 *
 *  An @e RmaReleaseLock record denotes the time the lock was released.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param win           ID of the window used for this operation. References a
 *                       @eref{RmaWin} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_RMA_WIN} is available.
 *  @param remote        Rank of the locked remote process or
 *                       @eref{OTF2_UNDEFINED_UINT32} if all processes of the
 *                       specified window are locked.
 *  @param lockId        ID of the lock released, if multiple locks are defined
 *                       on a window.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_RmaReleaseLock( OTF2_EvtWriter*     writer,
                               OTF2_AttributeList* attributeList,
                               OTF2_TimeStamp      time,
                               OTF2_RmaWinRef      win,
                               uint32_t            remote,
                               uint64_t            lockId );


/** @brief Records a RmaSync event.
 *
 *  An @e RmaSync record denotes the direct synchronization with a
 *  possibly remote process.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param win           ID of the window used for this operation. References a
 *                       @eref{RmaWin} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_RMA_WIN} is available.
 *  @param remote        Rank of the locked remote process.
 *  @param syncType      Type of synchronization.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_RmaSync( OTF2_EvtWriter*     writer,
                        OTF2_AttributeList* attributeList,
                        OTF2_TimeStamp      time,
                        OTF2_RmaWinRef      win,
                        uint32_t            remote,
                        OTF2_RmaSyncType    syncType );


/** @brief Records a RmaWaitChange event.
 *
 *  An @e RmaWaitChange record denotes the change of a window that was
 *  waited for.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param win           ID of the window used for this operation. References a
 *                       @eref{RmaWin} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_RMA_WIN} is available.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_RmaWaitChange( OTF2_EvtWriter*     writer,
                              OTF2_AttributeList* attributeList,
                              OTF2_TimeStamp      time,
                              OTF2_RmaWinRef      win );


/** @brief Records a RmaPut event.
 *
 *  An @e RmaPut record denotes the time a put operation was issued.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param win           ID of the window used for this operation. References a
 *                       @eref{RmaWin} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_RMA_WIN} is available.
 *  @param remote        Rank of the target process.
 *  @param bytes         Bytes sent to target.
 *  @param matchingId    ID used for matching the corresponding completion
 *                       record.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_RmaPut( OTF2_EvtWriter*     writer,
                       OTF2_AttributeList* attributeList,
                       OTF2_TimeStamp      time,
                       OTF2_RmaWinRef      win,
                       uint32_t            remote,
                       uint64_t            bytes,
                       uint64_t            matchingId );


/** @brief Records a RmaGet event.
 *
 *  An @e RmaGet record denotes the time a get operation was issued.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param win           ID of the window used for this operation. References a
 *                       @eref{RmaWin} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_RMA_WIN} is available.
 *  @param remote        Rank of the target process.
 *  @param bytes         Bytes received from target.
 *  @param matchingId    ID used for matching the corresponding completion
 *                       record.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_RmaGet( OTF2_EvtWriter*     writer,
                       OTF2_AttributeList* attributeList,
                       OTF2_TimeStamp      time,
                       OTF2_RmaWinRef      win,
                       uint32_t            remote,
                       uint64_t            bytes,
                       uint64_t            matchingId );


/** @brief Records a RmaAtomic event.
 *
 *  An @e RmaAtomic record denotes the time an atomic RMA operation was
 *  issued.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param win           ID of the window used for this operation. References a
 *                       @eref{RmaWin} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_RMA_WIN} is available.
 *  @param remote        Rank of the target process.
 *  @param type          Type of atomic operation.
 *  @param bytesSent     Bytes sent to target.
 *  @param bytesReceived Bytes received from target.
 *  @param matchingId    ID used for matching the corresponding completion
 *                       record.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_RmaAtomic( OTF2_EvtWriter*     writer,
                          OTF2_AttributeList* attributeList,
                          OTF2_TimeStamp      time,
                          OTF2_RmaWinRef      win,
                          uint32_t            remote,
                          OTF2_RmaAtomicType  type,
                          uint64_t            bytesSent,
                          uint64_t            bytesReceived,
                          uint64_t            matchingId );


/** @brief Records a RmaOpCompleteBlocking event.
 *
 *  An @e RmaOpCompleteBlocking record denotes the local completion of a
 *  blocking RMA operation.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param win           ID of the window used for this operation. References a
 *                       @eref{RmaWin} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_RMA_WIN} is available.
 *  @param matchingId    ID used for matching the corresponding RMA operation
 *                       record.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_RmaOpCompleteBlocking( OTF2_EvtWriter*     writer,
                                      OTF2_AttributeList* attributeList,
                                      OTF2_TimeStamp      time,
                                      OTF2_RmaWinRef      win,
                                      uint64_t            matchingId );


/** @brief Records a RmaOpCompleteNonBlocking event.
 *
 *  An @e RmaOpCompleteNonBlocking record denotes the local completion of
 *  a non-blocking RMA operation.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param win           ID of the window used for this operation. References a
 *                       @eref{RmaWin} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_RMA_WIN} is available.
 *  @param matchingId    ID used for matching the corresponding RMA operation
 *                       record.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_RmaOpCompleteNonBlocking( OTF2_EvtWriter*     writer,
                                         OTF2_AttributeList* attributeList,
                                         OTF2_TimeStamp      time,
                                         OTF2_RmaWinRef      win,
                                         uint64_t            matchingId );


/** @brief Records a RmaOpTest event.
 *
 *  An @e RmaOpTest record denotes that a non-blocking RMA operation has
 *  been tested for completion unsuccessfully.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param win           ID of the window used for this operation. References a
 *                       @eref{RmaWin} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_RMA_WIN} is available.
 *  @param matchingId    ID used for matching the corresponding RMA operation
 *                       record.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_RmaOpTest( OTF2_EvtWriter*     writer,
                          OTF2_AttributeList* attributeList,
                          OTF2_TimeStamp      time,
                          OTF2_RmaWinRef      win,
                          uint64_t            matchingId );


/** @brief Records a RmaOpCompleteRemote event.
 *
 *  An @e RmaOpCompleteRemote record denotes the remote completion of an
 *  RMA operation.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param win           ID of the window used for this operation. References a
 *                       @eref{RmaWin} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_RMA_WIN} is available.
 *  @param matchingId    ID used for matching the corresponding RMA operation
 *                       record.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_RmaOpCompleteRemote( OTF2_EvtWriter*     writer,
                                    OTF2_AttributeList* attributeList,
                                    OTF2_TimeStamp      time,
                                    OTF2_RmaWinRef      win,
                                    uint64_t            matchingId );


/** @brief Records a ThreadFork event.
 *
 *  A @e ThreadFork record marks that a thread forks a thread team.
 *
 *  @param writer                   Writer object.
 *  @param attributeList            Generic attributes for the event.
 *  @param time                     The time for this event.
 *  @param model                    The threading paradigm this event took place.
 *  @param numberOfRequestedThreads Requested size of the team.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_ThreadFork( OTF2_EvtWriter*     writer,
                           OTF2_AttributeList* attributeList,
                           OTF2_TimeStamp      time,
                           OTF2_Paradigm       model,
                           uint32_t            numberOfRequestedThreads );


/** @brief Records a ThreadJoin event.
 *
 *  A @e ThreadJoin record marks that a team of threads is joint and only
 *  the master thread continues execution.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param model         The threading paradigm this event took place.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_ThreadJoin( OTF2_EvtWriter*     writer,
                           OTF2_AttributeList* attributeList,
                           OTF2_TimeStamp      time,
                           OTF2_Paradigm       model );


/** @brief Records a ThreadTeamBegin event.
 *
 *  The current location enters the specified thread team.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param threadTeam    Thread team References a @eref{Comm}, or a
 *                       @eref{InterComm} definition and will be mapped to
 *                       the global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_COMM} is available.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_ThreadTeamBegin( OTF2_EvtWriter*     writer,
                                OTF2_AttributeList* attributeList,
                                OTF2_TimeStamp      time,
                                OTF2_CommRef        threadTeam );


/** @brief Records a ThreadTeamEnd event.
 *
 *  The current location leaves the specified thread team.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param threadTeam    Thread team References a @eref{Comm}, or a
 *                       @eref{InterComm} definition and will be mapped to
 *                       the global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_COMM} is available.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_ThreadTeamEnd( OTF2_EvtWriter*     writer,
                              OTF2_AttributeList* attributeList,
                              OTF2_TimeStamp      time,
                              OTF2_CommRef        threadTeam );


/** @brief Records a ThreadAcquireLock event.
 *
 *  A @e ThreadAcquireLock record marks that a thread acquires a lock.
 *
 *  @param writer           Writer object.
 *  @param attributeList    Generic attributes for the event.
 *  @param time             The time for this event.
 *  @param model            The threading paradigm this event took place.
 *  @param lockID           ID of the lock.
 *  @param acquisitionOrder A monotonically increasing number to determine the
 *                          order of lock acquisitions (with unsynchronized
 *                          clocks this is otherwise not possible).
 *                          Corresponding acquire-release events have same
 *                          number.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_ThreadAcquireLock( OTF2_EvtWriter*     writer,
                                  OTF2_AttributeList* attributeList,
                                  OTF2_TimeStamp      time,
                                  OTF2_Paradigm       model,
                                  uint32_t            lockID,
                                  uint32_t            acquisitionOrder );


/** @brief Records a ThreadReleaseLock event.
 *
 *  A @e ThreadReleaseLock record marks that a thread releases a lock.
 *
 *  @param writer           Writer object.
 *  @param attributeList    Generic attributes for the event.
 *  @param time             The time for this event.
 *  @param model            The threading paradigm this event took place.
 *  @param lockID           ID of the lock.
 *  @param acquisitionOrder A monotonically increasing number to determine the
 *                          order of lock acquisitions (with unsynchronized
 *                          clocks this is otherwise not possible).
 *                          Corresponding acquire-release events have same
 *                          number.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_ThreadReleaseLock( OTF2_EvtWriter*     writer,
                                  OTF2_AttributeList* attributeList,
                                  OTF2_TimeStamp      time,
                                  OTF2_Paradigm       model,
                                  uint32_t            lockID,
                                  uint32_t            acquisitionOrder );


/** @brief Records a ThreadTaskCreate event.
 *
 *  A @e ThreadTaskCreate record marks that a task in was/will be created
 *  and will be processed by the specified thread team.
 *
 *  @param writer           Writer object.
 *  @param attributeList    Generic attributes for the event.
 *  @param time             The time for this event.
 *  @param threadTeam       Thread team References a @eref{Comm}, or a
 *                          @eref{InterComm} definition and will be mapped to
 *                          the global definition if a mapping table of type
 *                          @eref{OTF2_MAPPING_COMM} is available.
 *  @param creatingThread   Creating thread of this task.
 *  @param generationNumber Thread-private generation number of task's creating
 *                          thread.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_ThreadTaskCreate( OTF2_EvtWriter*     writer,
                                 OTF2_AttributeList* attributeList,
                                 OTF2_TimeStamp      time,
                                 OTF2_CommRef        threadTeam,
                                 uint32_t            creatingThread,
                                 uint32_t            generationNumber );


/** @brief Records a ThreadTaskSwitch event.
 *
 *  A @e ThreadTaskSwitch record indicates that the execution of the
 *  current task will be suspended and another task starts/restarts
 *  its execution. Please note that this may change the current call
 *  stack of the executing location.
 *
 *  @param writer           Writer object.
 *  @param attributeList    Generic attributes for the event.
 *  @param time             The time for this event.
 *  @param threadTeam       Thread team References a @eref{Comm}, or a
 *                          @eref{InterComm} definition and will be mapped to
 *                          the global definition if a mapping table of type
 *                          @eref{OTF2_MAPPING_COMM} is available.
 *  @param creatingThread   Creating thread of this task.
 *  @param generationNumber Thread-private generation number of task's creating
 *                          thread.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_ThreadTaskSwitch( OTF2_EvtWriter*     writer,
                                 OTF2_AttributeList* attributeList,
                                 OTF2_TimeStamp      time,
                                 OTF2_CommRef        threadTeam,
                                 uint32_t            creatingThread,
                                 uint32_t            generationNumber );


/** @brief Records a ThreadTaskComplete event.
 *
 *  A @e ThreadTaskComplete record indicates that the execution of an
 *  OpenMP task has finished.
 *
 *  @param writer           Writer object.
 *  @param attributeList    Generic attributes for the event.
 *  @param time             The time for this event.
 *  @param threadTeam       Thread team References a @eref{Comm}, or a
 *                          @eref{InterComm} definition and will be mapped to
 *                          the global definition if a mapping table of type
 *                          @eref{OTF2_MAPPING_COMM} is available.
 *  @param creatingThread   Creating thread of this task.
 *  @param generationNumber Thread-private generation number of task's creating
 *                          thread.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_ThreadTaskComplete( OTF2_EvtWriter*     writer,
                                   OTF2_AttributeList* attributeList,
                                   OTF2_TimeStamp      time,
                                   OTF2_CommRef        threadTeam,
                                   uint32_t            creatingThread,
                                   uint32_t            generationNumber );


/** @brief Records a ThreadCreate event.
 *
 *  The location created successfully a new thread.
 *
 *  @param writer           Writer object.
 *  @param attributeList    Generic attributes for the event.
 *  @param time             The time for this event.
 *  @param threadContingent The thread contingent. References a @eref{Comm}, or a
 *                          @eref{InterComm} definition and will be mapped to
 *                          the global definition if a mapping table of type
 *                          @eref{OTF2_MAPPING_COMM} is available.
 *  @param sequenceCount    A @p threadContingent unique number. The
 *                          corresponding @eref{ThreadBegin} event does have
 *                          the same number.
 *
 *  @since Version 1.3
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_ThreadCreate( OTF2_EvtWriter*     writer,
                             OTF2_AttributeList* attributeList,
                             OTF2_TimeStamp      time,
                             OTF2_CommRef        threadContingent,
                             uint64_t            sequenceCount );


/** @brief Records a ThreadBegin event.
 *
 *  Marks the begin of a thread created by another thread.
 *
 *  @param writer           Writer object.
 *  @param attributeList    Generic attributes for the event.
 *  @param time             The time for this event.
 *  @param threadContingent The thread contingent. References a @eref{Comm}, or a
 *                          @eref{InterComm} definition and will be mapped to
 *                          the global definition if a mapping table of type
 *                          @eref{OTF2_MAPPING_COMM} is available.
 *  @param sequenceCount    A @p threadContingent unique number. The
 *                          corresponding @eref{ThreadCreate} event does have
 *                          the same number.
 *
 *  @since Version 1.3
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_ThreadBegin( OTF2_EvtWriter*     writer,
                            OTF2_AttributeList* attributeList,
                            OTF2_TimeStamp      time,
                            OTF2_CommRef        threadContingent,
                            uint64_t            sequenceCount );


/** @brief Records a ThreadWait event.
 *
 *  The location waits for the completion of another thread.
 *
 *  @param writer           Writer object.
 *  @param attributeList    Generic attributes for the event.
 *  @param time             The time for this event.
 *  @param threadContingent The thread contingent. References a @eref{Comm}, or a
 *                          @eref{InterComm} definition and will be mapped to
 *                          the global definition if a mapping table of type
 *                          @eref{OTF2_MAPPING_COMM} is available.
 *  @param sequenceCount    A @p threadContingent unique number. The
 *                          corresponding @eref{ThreadEnd} event does have
 *                          the same number.
 *
 *  @since Version 1.3
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_ThreadWait( OTF2_EvtWriter*     writer,
                           OTF2_AttributeList* attributeList,
                           OTF2_TimeStamp      time,
                           OTF2_CommRef        threadContingent,
                           uint64_t            sequenceCount );


/** @brief Records a ThreadEnd event.
 *
 *  Marks the end of a thread.
 *
 *  @param writer           Writer object.
 *  @param attributeList    Generic attributes for the event.
 *  @param time             The time for this event.
 *  @param threadContingent The thread contingent. References a @eref{Comm}, or a
 *                          @eref{InterComm} definition and will be mapped to
 *                          the global definition if a mapping table of type
 *                          @eref{OTF2_MAPPING_COMM} is available.
 *  @param sequenceCount    A @p threadContingent unique number. The
 *                          corresponding @eref{ThreadWait} event does have
 *                          the same number. @eref{OTF2_UNDEFINED_UINT64} in
 *                          case no corresponding @eref{ThreadWait} event
 *                          exists.
 *
 *  @since Version 1.3
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_ThreadEnd( OTF2_EvtWriter*     writer,
                          OTF2_AttributeList* attributeList,
                          OTF2_TimeStamp      time,
                          OTF2_CommRef        threadContingent,
                          uint64_t            sequenceCount );


/** @brief Records a CallingContextEnter event.
 *
 *  The thread entered an instrumented region, represented by the
 *  referenced @eref{CallingContext}. In contrast to the @eref{Enter}
 *  event, it gives the full calling context through the
 *  @eref{CallingContext} tree.
 *
 *  Events based on the @eref{CallingContext} definition are mutually
 *  exclusive with the @eref{Enter}/@eref{Leave} events in a trace.
 *
 *  If no callback for this event is set but a callback for @eref{Enter}
 *  events is defined, the reader will automatically generate an
 *  @eref{Enter} callback call for the @eref{Region} referenced by the
 *  @eref{CallingContext} attribute of this event.  Note that this
 *  emulation does @b not re-create the full calling context!  It only
 *  re-creates the event order for instrumented regions.
 *
 *  @param writer         Writer object.
 *  @param attributeList  Generic attributes for the event.
 *  @param time           The time for this event.
 *  @param callingContext The entered region as referenced by the
 *                        @eref{CallingContext} definition. References a
 *                        @eref{CallingContext} definition and will be mapped
 *                        to the global definition if a mapping table of type
 *                        @eref{OTF2_MAPPING_CALLING_CONTEXT} is available.
 *  @param unwindDistance The unwindDistance for this @p callingContext. See the
 *                        description in @eref{CallingContext}.
 *
 *  @since Version 2.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_CallingContextEnter( OTF2_EvtWriter*        writer,
                                    OTF2_AttributeList*    attributeList,
                                    OTF2_TimeStamp         time,
                                    OTF2_CallingContextRef callingContext,
                                    uint32_t               unwindDistance );


/** @brief Records a CallingContextLeave event.
 *
 *  The thread left an instrumented region, represented by the referenced
 *  @eref{CallingContext}. In contrast to the @eref{Leave} event, it
 *  gives the full calling context through the @eref{CallingContext}
 *  tree.
 *
 *  The unwind distance for this @eref{CallingContext} is defined to be 1.
 *  Because it must be assumed that the instrumented region made
 *  progress since the previous @eref{CallingContext} event.
 *
 *  Events based on the @eref{CallingContext} definition are mutually
 *  exclusive with the @eref{Enter}/@eref{Leave} events in a trace.
 *
 *  The parent of the @eref{CallingContext} must be used as the previous
 *  calling context for the next event.
 *
 *  If no callback for this event is set but a callback for @eref{Leave}
 *  events is defined, the reader will automatically generate an
 *  @eref{Leave} callback call for the @eref{Region} referenced by the
 *  @eref{CallingContext} attribute of this event.  Note that this
 *  emulation does @b not re-create the full calling context!  It only
 *  re-creates the event order for instrumented regions.
 *
 *  @param writer         Writer object.
 *  @param attributeList  Generic attributes for the event.
 *  @param time           The time for this event.
 *  @param callingContext The left region as referenced by the
 *                        @eref{CallingContext} definition. References a
 *                        @eref{CallingContext} definition and will be mapped
 *                        to the global definition if a mapping table of type
 *                        @eref{OTF2_MAPPING_CALLING_CONTEXT} is available.
 *
 *  @since Version 2.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_CallingContextLeave( OTF2_EvtWriter*        writer,
                                    OTF2_AttributeList*    attributeList,
                                    OTF2_TimeStamp         time,
                                    OTF2_CallingContextRef callingContext );


/** @brief Records a CallingContextSample event.
 *
 *  The thread was interrupted to take a sample of its current state
 *  (region and source code location).
 *
 *  Events based on the @eref{CallingContext} definition are mutually
 *  exclusive with the @eref{Enter}/@eref{Leave} events in a trace.
 *
 *  @param writer             Writer object.
 *  @param attributeList      Generic attributes for the event.
 *  @param time               The time for this event.
 *  @param callingContext     Describes the calling context of the thread when it
 *                            was interrupted. References a
 *                            @eref{CallingContext} definition and will be
 *                            mapped to the global definition if a mapping
 *                            table of type
 *                            @eref{OTF2_MAPPING_CALLING_CONTEXT} is
 *                            available.
 *  @param unwindDistance     The unwindDistance for this @p callingContext. See
 *                            the description in @eref{CallingContext}.
 *  @param interruptGenerator  References a @eref{InterruptGenerator} definition
 *                            and will be mapped to the global definition if
 *                            a mapping table of type
 *                            @eref{OTF2_MAPPING_INTERRUPT_GENERATOR} is
 *                            available.
 *
 *  @since Version 1.5
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_CallingContextSample( OTF2_EvtWriter*            writer,
                                     OTF2_AttributeList*        attributeList,
                                     OTF2_TimeStamp             time,
                                     OTF2_CallingContextRef     callingContext,
                                     uint32_t                   unwindDistance,
                                     OTF2_InterruptGeneratorRef interruptGenerator );


/** @brief Records a IoCreateHandle event.
 *
 *  An @e IoCreateHandle record marks the creation of a new @emph{active}
 *  I/O handle that can be used by subsequent I/O operation events.
 *
 *  An @eref{IoHandle} is @emph{active} between a pair of consecutive
 *  @eref{IoCreateHandle} and @eref{IoDestroyHandle} events. All
 *  @eref{Location}s of a @eref{LocationGroup} have access to an
 *  @emph{active} @eref{IoHandle}.
 *
 *  If the @eref{Comm} attribute of the @eref{IoHandle} handle is not
 *  @eref{OTF2_UNDEFINED_COMM}, this is a collective operation over
 *  @eref{Comm}.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param handle        A previously @emph{inactive} I/O handle which will be
 *                       activated by this record. References a
 *                       @eref{IoHandle} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_IO_HANDLE} is available.
 *  @param mode          Determines which I/O operations can be applied to this
 *                       I/O handle (e.g., read-only, write-only, read-
 *                       write).
 *  @param creationFlags Requested I/O handle creation flags (e.g., create,
 *                       exclusive, etc.).
 *  @param statusFlags   I/O handle status flags which will be associated with
 *                       the @p handle attribute (e.g., append, create,
 *                       close-on-exec, async, etc).
 *
 *  @since Version 2.1
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_IoCreateHandle( OTF2_EvtWriter*     writer,
                               OTF2_AttributeList* attributeList,
                               OTF2_TimeStamp      time,
                               OTF2_IoHandleRef    handle,
                               OTF2_IoAccessMode   mode,
                               OTF2_IoCreationFlag creationFlags,
                               OTF2_IoStatusFlag   statusFlags );


/** @brief Records a IoDestroyHandle event.
 *
 *  An @e IoDestroyHandle record marks the end of an @emph{active} I/O
 *  handle's lifetime.
 *
 *  An @eref{IoHandle} is @emph{active} between a pair of consecutive
 *  @eref{IoCreateHandle} and @eref{IoDestroyHandle} events. All
 *  @eref{Location}s of a @eref{LocationGroup} have access to an
 *  @emph{active} @eref{IoHandle}.
 *
 *  If the @eref{Comm} attribute of the @eref{IoHandle} handle is not
 *  @eref{OTF2_UNDEFINED_COMM}, this is a collective operation over
 *  @eref{Comm}.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param handle        An @emph{active} I/O handle which will be inactivated by
 *                       this records. References a @eref{IoHandle}
 *                       definition and will be mapped to the global
 *                       definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_IO_HANDLE} is available.
 *
 *  @since Version 2.1
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_IoDestroyHandle( OTF2_EvtWriter*     writer,
                                OTF2_AttributeList* attributeList,
                                OTF2_TimeStamp      time,
                                OTF2_IoHandleRef    handle );


/** @brief Records a IoDuplicateHandle event.
 *
 *  An @e IoDuplicateHandle record marks the duplication of an already
 *  existing @emph{active} I/O handle.
 *
 *  The new I/O handle @p newHandle is @emph{active} after this event.
 *
 *  Both @eref{IoHandle}s must reference the same @eref{Comm} definition
 *  or be @eref{OTF2_UNDEFINED_COMM}.  If the @eref{Comm} attribute of
 *  the @eref{IoHandle} handles is not @eref{OTF2_UNDEFINED_COMM},
 *  this is a collective operation over @eref{Comm}.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param oldHandle     An @emph{active} I/O handle. References a
 *                       @eref{IoHandle} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_IO_HANDLE} is available.
 *  @param newHandle     A previously @emph{inactive} I/O handle which will be
 *                       activated by this record. References a
 *                       @eref{IoHandle} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_IO_HANDLE} is available.
 *  @param statusFlags   The status flag for the new I/O handle @p newHandle. No
 *                       status flags will be inherited from the I/O handle
 *                       @p oldHandle.
 *
 *  @since Version 2.1
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_IoDuplicateHandle( OTF2_EvtWriter*     writer,
                                  OTF2_AttributeList* attributeList,
                                  OTF2_TimeStamp      time,
                                  OTF2_IoHandleRef    oldHandle,
                                  OTF2_IoHandleRef    newHandle,
                                  OTF2_IoStatusFlag   statusFlags );


/** @brief Records a IoSeek event.
 *
 *  An @e IoSeek record marks a change of the position, e.g., within a
 *  file.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param handle        An @emph{active} I/O handle. References a
 *                       @eref{IoHandle} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_IO_HANDLE} is available.
 *  @param offsetRequest Requested offset.
 *  @param whence        Position inside the file from where @p offsetRequest
 *                       should be applied (e.g., absolute from the start or
 *                       end, relative to the current position).
 *  @param offsetResult  Resulting offset, e.g., within the file relative to the
 *                       beginning of the file.
 *
 *  @since Version 2.1
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_IoSeek( OTF2_EvtWriter*     writer,
                       OTF2_AttributeList* attributeList,
                       OTF2_TimeStamp      time,
                       OTF2_IoHandleRef    handle,
                       int64_t             offsetRequest,
                       OTF2_IoSeekOption   whence,
                       uint64_t            offsetResult );


/** @brief Records a IoChangeStatusFlags event.
 *
 *  An @e IoChangeStatusFlags record marks a change to the status flags
 *  associated with an @emph{active} I/O handle.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param handle        An @emph{active} I/O handle. References a
 *                       @eref{IoHandle} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_IO_HANDLE} is available.
 *  @param statusFlags   Set flags (e.g., close-on-exec, append, etc.).
 *
 *  @since Version 2.1
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_IoChangeStatusFlags( OTF2_EvtWriter*     writer,
                                    OTF2_AttributeList* attributeList,
                                    OTF2_TimeStamp      time,
                                    OTF2_IoHandleRef    handle,
                                    OTF2_IoStatusFlag   statusFlags );


/** @brief Records a IoDeleteFile event.
 *
 *  An @e IoDeleteFile record marks the deletion of an I/O file.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param ioParadigm    The I/O paradigm which induced the deletion. References
 *                       a @eref{IoParadigm} definition.
 *  @param file          File identifier. References a @eref{IoRegularFile}, or a
 *                       @eref{IoDirectory} definition and will be mapped to
 *                       the global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_IO_FILE} is available.
 *
 *  @since Version 2.1
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_IoDeleteFile( OTF2_EvtWriter*     writer,
                             OTF2_AttributeList* attributeList,
                             OTF2_TimeStamp      time,
                             OTF2_IoParadigmRef  ioParadigm,
                             OTF2_IoFileRef      file );


/** @brief Records a IoOperationBegin event.
 *
 *  An @e IoOperationBegin record marks the begin of a file operation
 *  (read, write, etc.).
 *
 *  See @ref otf2_io_operation_event_order for the possible event orders.
 *
 *  @param writer         Writer object.
 *  @param attributeList  Generic attributes for the event.
 *  @param time           The time for this event.
 *  @param handle         An @emph{active} I/O handle. References a
 *                        @eref{IoHandle} definition and will be mapped to
 *                        the global definition if a mapping table of type
 *                        @eref{OTF2_MAPPING_IO_HANDLE} is available.
 *  @param mode           Mode of an I/O handle operation (e.g., read or write).
 *  @param operationFlags Special semantic of this operation.
 *  @param bytesRequest   Requested bytes to write/read.
 *  @param matchingId     Identifier used to correlate associated event records
 *                        of an I/O operation. This identifier is unique for
 *                        the referenced @eref{IoHandle}.
 *
 *  @since Version 2.1
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_IoOperationBegin( OTF2_EvtWriter*      writer,
                                 OTF2_AttributeList*  attributeList,
                                 OTF2_TimeStamp       time,
                                 OTF2_IoHandleRef     handle,
                                 OTF2_IoOperationMode mode,
                                 OTF2_IoOperationFlag operationFlags,
                                 uint64_t             bytesRequest,
                                 uint64_t             matchingId );


/** @brief Records a IoOperationTest event.
 *
 *  An @e IoOperationTest record marks an unsuccessful test whether an I/O
 *  operation has already finished.
 *
 *  See @ref otf2_io_operation_event_order for the possible event orders.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param handle        An @emph{active} I/O handle. References a
 *                       @eref{IoHandle} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_IO_HANDLE} is available.
 *  @param matchingId    Identifier used to correlate associated event records of
 *                       an I/O operation. This identifier is unique for the
 *                       referenced @eref{IoHandle}.
 *
 *  @since Version 2.1
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_IoOperationTest( OTF2_EvtWriter*     writer,
                                OTF2_AttributeList* attributeList,
                                OTF2_TimeStamp      time,
                                OTF2_IoHandleRef    handle,
                                uint64_t            matchingId );


/** @brief Records a IoOperationIssued event.
 *
 *  An @e IoOperationIssued record marks the successful initiation of a
 *  non-blocking operation (read, write, etc.) on an @emph{active} I/O
 *  handle.
 *
 *  See @ref otf2_io_operation_event_order for the possible event orders.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param handle        An @emph{active} I/O handle. References a
 *                       @eref{IoHandle} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_IO_HANDLE} is available.
 *  @param matchingId    Identifier used to correlate associated event records of
 *                       an I/O operation. This identifier is unique for the
 *                       referenced @eref{IoHandle}.
 *
 *  @since Version 2.1
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_IoOperationIssued( OTF2_EvtWriter*     writer,
                                  OTF2_AttributeList* attributeList,
                                  OTF2_TimeStamp      time,
                                  OTF2_IoHandleRef    handle,
                                  uint64_t            matchingId );


/** @brief Records a IoOperationComplete event.
 *
 *  An @e IoOperationComplete record marks the end of a file operation
 *  (read, write, etc.) on an @emph{active} I/O handle.
 *
 *  See @ref otf2_io_operation_event_order for the possible event orders.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param handle        An @emph{active} I/O handle. References a
 *                       @eref{IoHandle} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_IO_HANDLE} is available.
 *  @param bytesResult   Number of actual transferred bytes.
 *  @param matchingId    Identifier used to correlate associated event records of
 *                       an I/O operation. This identifier is unique for the
 *                       referenced @eref{IoHandle}.
 *
 *  @since Version 2.1
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_IoOperationComplete( OTF2_EvtWriter*     writer,
                                    OTF2_AttributeList* attributeList,
                                    OTF2_TimeStamp      time,
                                    OTF2_IoHandleRef    handle,
                                    uint64_t            bytesResult,
                                    uint64_t            matchingId );


/** @brief Records a IoOperationCancelled event.
 *
 *  An @e IoOperationCancelled record marks the successful cancellation of
 *  a non-blocking operation (read, write, etc.) on an @emph{active}
 *  I/O handle.
 *
 *  See @ref otf2_io_operation_event_order for the possible event orders.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param handle        An @emph{active} I/O handle. References a
 *                       @eref{IoHandle} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_IO_HANDLE} is available.
 *  @param matchingId    Identifier used to correlate associated event records of
 *                       an I/O operation. This identifier is unique for the
 *                       referenced @eref{IoHandle}.
 *
 *  @since Version 2.1
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_IoOperationCancelled( OTF2_EvtWriter*     writer,
                                     OTF2_AttributeList* attributeList,
                                     OTF2_TimeStamp      time,
                                     OTF2_IoHandleRef    handle,
                                     uint64_t            matchingId );


/** @brief Records a IoAcquireLock event.
 *
 *  An @e IoAcquireLock record marks the acquisition of an I/O lock.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param handle        An @emph{active} I/O handle. References a
 *                       @eref{IoHandle} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_IO_HANDLE} is available.
 *  @param lockType      Type of the lock.
 *
 *  @since Version 2.1
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_IoAcquireLock( OTF2_EvtWriter*     writer,
                              OTF2_AttributeList* attributeList,
                              OTF2_TimeStamp      time,
                              OTF2_IoHandleRef    handle,
                              OTF2_LockType       lockType );


/** @brief Records a IoReleaseLock event.
 *
 *  An @e IoReleaseLock record marks the release of an I/O lock.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param handle        An @emph{active} I/O handle. References a
 *                       @eref{IoHandle} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_IO_HANDLE} is available.
 *  @param lockType      Type of the lock.
 *
 *  @since Version 2.1
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_IoReleaseLock( OTF2_EvtWriter*     writer,
                              OTF2_AttributeList* attributeList,
                              OTF2_TimeStamp      time,
                              OTF2_IoHandleRef    handle,
                              OTF2_LockType       lockType );


/** @brief Records a IoTryLock event.
 *
 *  An @e IoTryLock record marks when an I/O lock was requested but not
 *  granted.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param handle        An @emph{active} I/O handle. References a
 *                       @eref{IoHandle} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_IO_HANDLE} is available.
 *  @param lockType      Type of the lock.
 *
 *  @since Version 2.1
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_IoTryLock( OTF2_EvtWriter*     writer,
                          OTF2_AttributeList* attributeList,
                          OTF2_TimeStamp      time,
                          OTF2_IoHandleRef    handle,
                          OTF2_LockType       lockType );


/** @brief Records a ProgramBegin event.
 *
 *  The @e ProgramBegin record marks the begin of the program.
 *
 *  This event is restricted to happen at most once on any @eref{Location}
 *  in a @eref{LocationGroup} that is of type
 *  @eref{OTF2_LOCATION_GROUP_TYPE_PROCESS}.
 *
 *  If there is a ProgramBegin record, a corresponding @eref{ProgramEnd}
 *  record on any @eref{Location} in the same @eref{LocationGroup} is
 *  mandatory and vice versa.
 *
 *  None of the timestamps recorded within the same @eref{LocationGroup}
 *  must be smaller than ProgramBegin's timestamp.
 *
 *  @param writer            Writer object.
 *  @param attributeList     Generic attributes for the event.
 *  @param time              The time for this event.
 *  @param programName       The name of the program. References a @eref{String}
 *                           definition and will be mapped to the global
 *                           definition if a mapping table of type
 *                           @eref{OTF2_MAPPING_STRING} is available.
 *  @param numberOfArguments Number of additional arguments to the program.
 *  @param programArguments  List of additional arguments to the program.
 *
 *  @since Version 2.1
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_ProgramBegin( OTF2_EvtWriter*       writer,
                             OTF2_AttributeList*   attributeList,
                             OTF2_TimeStamp        time,
                             OTF2_StringRef        programName,
                             uint32_t              numberOfArguments,
                             const OTF2_StringRef* programArguments );


/** @brief Records a ProgramEnd event.
 *
 *  The @e ProgramEnd record marks the end of the program.
 *
 *  This event is restricted to happen at most once on any @eref{Location}
 *  in a @eref{LocationGroup} that is of type
 *  @eref{OTF2_LOCATION_GROUP_TYPE_PROCESS}.
 *
 *  If there is a ProgramEnd record, a corresponding @eref{ProgramBegin}
 *  record on any @eref{Location} in the same @eref{LocationGroup} is
 *  mandatory, and vice versa.
 *
 *  None of the timestamps recorded within the same @eref{LocationGroup}
 *  must be larger than ProgramEnd's timestamp.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param exitStatus    The exit status of the program. Note, that on some
 *                       systems only the least significant 8 bits may be
 *                       visible to other processes. Use
 *                       @eref{OTF2_UNDEFINED_INT64}, if the exit status was
 *                       not available.
 *
 *  @since Version 2.1
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_ProgramEnd( OTF2_EvtWriter*     writer,
                           OTF2_AttributeList* attributeList,
                           OTF2_TimeStamp      time,
                           int64_t             exitStatus );


/** @brief Records a NonBlockingCollectiveRequest event.
 *
 *  A @e NonBlockingCollectiveRequest record indicates that a non-blocking
 *  collective operation was initiated.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param requestID     ID of the requested operation.
 *
 *  @since Version 3.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_NonBlockingCollectiveRequest( OTF2_EvtWriter*     writer,
                                             OTF2_AttributeList* attributeList,
                                             OTF2_TimeStamp      time,
                                             uint64_t            requestID );


/** @brief Records a NonBlockingCollectiveComplete event.
 *
 *  A @e NonBlockingCollectiveComplete record indicates that a non-
 *  blocking collective operation completed.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param collectiveOp  Determines which collective operation it is.
 *  @param communicator  Communicator ID. References a @eref{Comm}, or a
 *                       @eref{InterComm} definition and will be mapped to
 *                       the global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_COMM} is available.
 *  @param root          Rank of root in @p communicator or any predefined
 *                       constant of @eref{OTF2_CollectiveRoot}.
 *  @param sizeSent      Size of the sent data.
 *  @param sizeReceived  Size of the received data.
 *  @param requestID     ID of the requested operation.
 *
 *  @since Version 3.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_NonBlockingCollectiveComplete( OTF2_EvtWriter*     writer,
                                              OTF2_AttributeList* attributeList,
                                              OTF2_TimeStamp      time,
                                              OTF2_CollectiveOp   collectiveOp,
                                              OTF2_CommRef        communicator,
                                              uint32_t            root,
                                              uint64_t            sizeSent,
                                              uint64_t            sizeReceived,
                                              uint64_t            requestID );


/** @brief Records a CommCreate event.
 *
 *  A @e CommCreate record denotes the creation of a communicator. Only
 *  valid if the @eref{Comm} definition was flagged with
 *  @eref{OTF2_COMM_FLAG_CREATE_DESTROY_EVENTS}. This event must be
 *  enclosed by an @eref{MpiCollectiveBegin} and
 *  @eref{MpiCollectiveEnd} or @eref{NonBlockingCollectiveRequest} and
 *  @eref{NonBlockingCollectiveComplete} event pair with
 *  @eref{OTF2_COLLECTIVE_OP_CREATE_HANDLE} as the operation type.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param communicator  Communicator ID. References a @eref{Comm}, or a
 *                       @eref{InterComm} definition and will be mapped to
 *                       the global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_COMM} is available.
 *
 *  @since Version 3.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_CommCreate( OTF2_EvtWriter*     writer,
                           OTF2_AttributeList* attributeList,
                           OTF2_TimeStamp      time,
                           OTF2_CommRef        communicator );


/** @brief Records a CommDestroy event.
 *
 *  A @e CommDestroy record marks the communicator for destruction at the
 *  end of the enclosing @eref{MpiCollectiveBegin} and
 *  @eref{MpiCollectiveEnd} event pair. Only valid if the @eref{Comm}
 *  definition was flagged with
 *  @eref{OTF2_COMM_FLAG_CREATE_DESTROY_EVENTS}. This event must be
 *  enclosed by an @eref{MpiCollectiveBegin} and
 *  @eref{MpiCollectiveEnd} event pair with
 *  @eref{OTF2_COLLECTIVE_OP_DESTROY_HANDLE} as the operation type.
 *
 *  @param writer        Writer object.
 *  @param attributeList Generic attributes for the event.
 *  @param time          The time for this event.
 *  @param communicator  Communicator ID. References a @eref{Comm}, or a
 *                       @eref{InterComm} definition and will be mapped to
 *                       the global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_COMM} is available.
 *
 *  @since Version 3.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_CommDestroy( OTF2_EvtWriter*     writer,
                            OTF2_AttributeList* attributeList,
                            OTF2_TimeStamp      time,
                            OTF2_CommRef        communicator );


/** @brief Please give me a documentation.
 *
 *  @param writer   Writer object.
 *  @param rewindId Generic attributes for the event.
 *
 *  @since Version 1.1
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_StoreRewindPoint( OTF2_EvtWriter* writer,
                                 uint32_t        rewindId );


/** @brief Please give me a documentation.
 *
 *  @param writer   Writer object.
 *  @param rewindId Generic attributes for the event.
 *
 *  @since Version 1.1
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_Rewind( OTF2_EvtWriter* writer,
                       uint32_t        rewindId );


/** @brief Please give me a documentation.
 *
 *  @param writer   Writer object.
 *  @param rewindId Generic attributes for the event.
 *
 *  @since Version 1.1
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EvtWriter_ClearRewindPoint( OTF2_EvtWriter* writer,
                                 uint32_t        rewindId );


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* !OTF2_EVT_WRITER_H */
