#include <stdio.h>
#include <stdlib.h>

#include "OTF2_EvtReaderCallbacks.h"
#include "htf.h"
#include "otf2.h"

OTF2_EvtReaderCallbacks* OTF2_EvtReaderCallbacks_New(void) {
  NOT_IMPLEMENTED;
}

void OTF2_EvtReaderCallbacks_Delete(OTF2_EvtReaderCallbacks* evtReaderCallbacks) {
  NOT_IMPLEMENTED;
}

void OTF2_EvtReaderCallbacks_Clear(OTF2_EvtReaderCallbacks* evtReaderCallbacks) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetUnknownCallback(OTF2_EvtReaderCallbacks* evtReaderCallbacks,
                                                          OTF2_EvtReaderCallback_Unknown unknownCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetBufferFlushCallback(OTF2_EvtReaderCallbacks* evtReaderCallbacks,
                                                              OTF2_EvtReaderCallback_BufferFlush bufferFlushCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetMeasurementOnOffCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_MeasurementOnOff measurementOnOffCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetEnterCallback(OTF2_EvtReaderCallbacks* evtReaderCallbacks,
                                                        OTF2_EvtReaderCallback_Enter enterCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetLeaveCallback(OTF2_EvtReaderCallbacks* evtReaderCallbacks,
                                                        OTF2_EvtReaderCallback_Leave leaveCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetMpiSendCallback(OTF2_EvtReaderCallbacks* evtReaderCallbacks,
                                                          OTF2_EvtReaderCallback_MpiSend mpiSendCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetMpiIsendCallback(OTF2_EvtReaderCallbacks* evtReaderCallbacks,
                                                           OTF2_EvtReaderCallback_MpiIsend mpiIsendCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetMpiIsendCompleteCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_MpiIsendComplete mpiIsendCompleteCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetMpiIrecvRequestCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_MpiIrecvRequest mpiIrecvRequestCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetMpiRecvCallback(OTF2_EvtReaderCallbacks* evtReaderCallbacks,
                                                          OTF2_EvtReaderCallback_MpiRecv mpiRecvCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetMpiIrecvCallback(OTF2_EvtReaderCallbacks* evtReaderCallbacks,
                                                           OTF2_EvtReaderCallback_MpiIrecv mpiIrecvCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetMpiRequestTestCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_MpiRequestTest mpiRequestTestCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetMpiRequestCancelledCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_MpiRequestCancelled mpiRequestCancelledCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetMpiCollectiveBeginCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_MpiCollectiveBegin mpiCollectiveBeginCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetMpiCollectiveEndCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_MpiCollectiveEnd mpiCollectiveEndCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetOmpForkCallback(OTF2_EvtReaderCallbacks* evtReaderCallbacks,
                                                          OTF2_EvtReaderCallback_OmpFork ompForkCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetOmpJoinCallback(OTF2_EvtReaderCallbacks* evtReaderCallbacks,
                                                          OTF2_EvtReaderCallback_OmpJoin ompJoinCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetOmpAcquireLockCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_OmpAcquireLock ompAcquireLockCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetOmpReleaseLockCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_OmpReleaseLock ompReleaseLockCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetOmpTaskCreateCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_OmpTaskCreate ompTaskCreateCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetOmpTaskSwitchCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_OmpTaskSwitch ompTaskSwitchCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetOmpTaskCompleteCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_OmpTaskComplete ompTaskCompleteCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetMetricCallback(OTF2_EvtReaderCallbacks* evtReaderCallbacks,
                                                         OTF2_EvtReaderCallback_Metric metricCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetParameterStringCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_ParameterString parameterStringCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetParameterIntCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_ParameterInt parameterIntCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetParameterUnsignedIntCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_ParameterUnsignedInt parameterUnsignedIntCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetRmaWinCreateCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_RmaWinCreate rmaWinCreateCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetRmaWinDestroyCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_RmaWinDestroy rmaWinDestroyCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetRmaCollectiveBeginCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_RmaCollectiveBegin rmaCollectiveBeginCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetRmaCollectiveEndCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_RmaCollectiveEnd rmaCollectiveEndCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetRmaGroupSyncCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_RmaGroupSync rmaGroupSyncCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetRmaRequestLockCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_RmaRequestLock rmaRequestLockCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetRmaAcquireLockCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_RmaAcquireLock rmaAcquireLockCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetRmaTryLockCallback(OTF2_EvtReaderCallbacks* evtReaderCallbacks,
                                                             OTF2_EvtReaderCallback_RmaTryLock rmaTryLockCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetRmaReleaseLockCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_RmaReleaseLock rmaReleaseLockCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetRmaSyncCallback(OTF2_EvtReaderCallbacks* evtReaderCallbacks,
                                                          OTF2_EvtReaderCallback_RmaSync rmaSyncCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetRmaWaitChangeCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_RmaWaitChange rmaWaitChangeCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetRmaPutCallback(OTF2_EvtReaderCallbacks* evtReaderCallbacks,
                                                         OTF2_EvtReaderCallback_RmaPut rmaPutCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetRmaGetCallback(OTF2_EvtReaderCallbacks* evtReaderCallbacks,
                                                         OTF2_EvtReaderCallback_RmaGet rmaGetCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetRmaAtomicCallback(OTF2_EvtReaderCallbacks* evtReaderCallbacks,
                                                            OTF2_EvtReaderCallback_RmaAtomic rmaAtomicCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetRmaOpCompleteBlockingCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_RmaOpCompleteBlocking rmaOpCompleteBlockingCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetRmaOpCompleteNonBlockingCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_RmaOpCompleteNonBlocking rmaOpCompleteNonBlockingCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetRmaOpTestCallback(OTF2_EvtReaderCallbacks* evtReaderCallbacks,
                                                            OTF2_EvtReaderCallback_RmaOpTest rmaOpTestCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetRmaOpCompleteRemoteCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_RmaOpCompleteRemote rmaOpCompleteRemoteCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetThreadForkCallback(OTF2_EvtReaderCallbacks* evtReaderCallbacks,
                                                             OTF2_EvtReaderCallback_ThreadFork threadForkCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetThreadJoinCallback(OTF2_EvtReaderCallbacks* evtReaderCallbacks,
                                                             OTF2_EvtReaderCallback_ThreadJoin threadJoinCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetThreadTeamBeginCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_ThreadTeamBegin threadTeamBeginCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetThreadTeamEndCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_ThreadTeamEnd threadTeamEndCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetThreadAcquireLockCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_ThreadAcquireLock threadAcquireLockCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetThreadReleaseLockCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_ThreadReleaseLock threadReleaseLockCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetThreadTaskCreateCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_ThreadTaskCreate threadTaskCreateCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetThreadTaskSwitchCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_ThreadTaskSwitch threadTaskSwitchCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetThreadTaskCompleteCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_ThreadTaskComplete threadTaskCompleteCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetThreadCreateCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_ThreadCreate threadCreateCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetThreadBeginCallback(OTF2_EvtReaderCallbacks* evtReaderCallbacks,
                                                              OTF2_EvtReaderCallback_ThreadBegin threadBeginCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetThreadWaitCallback(OTF2_EvtReaderCallbacks* evtReaderCallbacks,
                                                             OTF2_EvtReaderCallback_ThreadWait threadWaitCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetThreadEndCallback(OTF2_EvtReaderCallbacks* evtReaderCallbacks,
                                                            OTF2_EvtReaderCallback_ThreadEnd threadEndCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetCallingContextEnterCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_CallingContextEnter callingContextEnterCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetCallingContextLeaveCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_CallingContextLeave callingContextLeaveCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetCallingContextSampleCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_CallingContextSample callingContextSampleCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetIoCreateHandleCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_IoCreateHandle ioCreateHandleCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetIoDestroyHandleCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_IoDestroyHandle ioDestroyHandleCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetIoDuplicateHandleCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_IoDuplicateHandle ioDuplicateHandleCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetIoSeekCallback(OTF2_EvtReaderCallbacks* evtReaderCallbacks,
                                                         OTF2_EvtReaderCallback_IoSeek ioSeekCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetIoChangeStatusFlagsCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_IoChangeStatusFlags ioChangeStatusFlagsCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetIoDeleteFileCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_IoDeleteFile ioDeleteFileCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetIoOperationBeginCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_IoOperationBegin ioOperationBeginCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetIoOperationTestCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_IoOperationTest ioOperationTestCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetIoOperationIssuedCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_IoOperationIssued ioOperationIssuedCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetIoOperationCompleteCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_IoOperationComplete ioOperationCompleteCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetIoOperationCancelledCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_IoOperationCancelled ioOperationCancelledCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetIoAcquireLockCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_IoAcquireLock ioAcquireLockCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetIoReleaseLockCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_IoReleaseLock ioReleaseLockCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetIoTryLockCallback(OTF2_EvtReaderCallbacks* evtReaderCallbacks,
                                                            OTF2_EvtReaderCallback_IoTryLock ioTryLockCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetProgramBeginCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_ProgramBegin programBeginCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetProgramEndCallback(OTF2_EvtReaderCallbacks* evtReaderCallbacks,
                                                             OTF2_EvtReaderCallback_ProgramEnd programEndCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetNonBlockingCollectiveRequestCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_NonBlockingCollectiveRequest nonBlockingCollectiveRequestCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetNonBlockingCollectiveCompleteCallback(
  OTF2_EvtReaderCallbacks* evtReaderCallbacks,
  OTF2_EvtReaderCallback_NonBlockingCollectiveComplete nonBlockingCollectiveCompleteCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetCommCreateCallback(OTF2_EvtReaderCallbacks* evtReaderCallbacks,
                                                             OTF2_EvtReaderCallback_CommCreate commCreateCallback) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReaderCallbacks_SetCommDestroyCallback(OTF2_EvtReaderCallbacks* evtReaderCallbacks,
                                                              OTF2_EvtReaderCallback_CommDestroy commDestroyCallback) {
  NOT_IMPLEMENTED;
}
