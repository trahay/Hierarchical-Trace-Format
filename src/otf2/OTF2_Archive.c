#include <stdlib.h>
#include <stdio.h>

#include "htf.h"
#include "htf_archive.h"
#include "htf_write.h"
#include "otf2.h"
#include "OTF2_Archive.h"

OTF2_Archive*
OTF2_Archive_Open( const char*              archivePath,
                   const char*              archiveName,
                   const OTF2_FileMode      fileMode,
                   const uint64_t           chunkSizeEvents,
                   const uint64_t           chunkSizeDefs,
                   const OTF2_FileSubstrate fileSubstrate,
                   const OTF2_Compression   compression ) {
  OTF2_Archive* archive = malloc(sizeof(OTF2_Archive));

  htf_write_archive_open(&archive->archive,
			 archivePath,
			 archiveName,
			 0); /* TODO: add missing archive_id_t */

  archive->globalDefWriter = NULL;

  archive->def_writers = NULL;
  archive->evt_writers = NULL;
  archive->nb_locations = 0;
	pthread_mutex_init(&archive->lock, NULL);

  return archive;
}

OTF2_ErrorCode
OTF2_Archive_Close( OTF2_Archive* archive ) {
  htf_write_archive_close(&archive->archive);
  return OTF2_SUCCESS;
}

OTF2_ErrorCode
OTF2_Archive_SwitchFileMode( OTF2_Archive* archive,
                             OTF2_FileMode newFileMode ) {
    NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_SetDefChunkSize( OTF2_Archive* archive,
                              uint64_t      chunkSize ) {
    NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_SetMachineName( OTF2_Archive* archive,
                             const char*   machineName ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_SetDescription( OTF2_Archive* archive,
                             const char*   description ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_SetCreator( OTF2_Archive* archive,
                         const char*   creator ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_SetFlushCallbacks( OTF2_Archive*              archive,
                                const OTF2_FlushCallbacks* flushCallbacks,
                                void*                      flushData ) {
  archive->flushCallbacks.otf2_pre_flush = flushCallbacks->otf2_pre_flush;
  archive->flushCallbacks.otf2_post_flush = flushCallbacks->otf2_post_flush;
  archive->flushData = flushData;
  return OTF2_SUCCESS;
}

OTF2_ErrorCode
OTF2_Archive_SetMemoryCallbacks( OTF2_Archive*               archive,
                                 const OTF2_MemoryCallbacks* memoryCallbacks,
                                 void*                       memoryData ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_SetCollectiveCallbacks( OTF2_Archive*                   archive,
                                     const OTF2_CollectiveCallbacks* collectiveCallbacks,
                                     void*                           collectiveData,
                                     OTF2_CollectiveContext*         globalCommContext,
                                     OTF2_CollectiveContext*         localCommContext ) {
  return OTF2_SUCCESS;
}

OTF2_ErrorCode
OTF2_Archive_SetSerialCollectiveCallbacks( OTF2_Archive* archive ) {
  return OTF2_SUCCESS;
}

OTF2_ErrorCode
OTF2_Archive_IsPrimary( OTF2_Archive* archive,
                        bool*         result ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_IsMaster( OTF2_Archive* archive,
                       bool*         result ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_SetLockingCallbacks( OTF2_Archive*                archive,
                                  const OTF2_LockingCallbacks* lockingCallbacks,
                                  void*                        lockingData ) {
  archive->lockingCallbacks.otf2_release = lockingCallbacks->otf2_release;
  archive->lockingCallbacks.otf2_create  = lockingCallbacks->otf2_create;
  archive->lockingCallbacks.otf2_destroy = lockingCallbacks->otf2_destroy;
  archive->lockingCallbacks.otf2_lock    = lockingCallbacks->otf2_lock;
  archive->lockingCallbacks.otf2_unlock  = lockingCallbacks->otf2_unlock;

  archive->lockingData = lockingData;
  return OTF2_SUCCESS;
}

OTF2_ErrorCode
OTF2_Archive_SetHint( OTF2_Archive* archive,
                      OTF2_Hint     hint,
                      void*         value ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_SetProperty( OTF2_Archive* archive,
                          const char*   name,
                          const char*   value,
                          bool          overwrite ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_SetBoolProperty( OTF2_Archive* archive,
                              const char*   name,
                              bool          value,
                              bool          overwrite ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_GetPropertyNames( OTF2_Archive* archive,
                               uint32_t*     numberOfProperties,
                               char***       names ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_GetProperty( OTF2_Archive* archive,
                          const char*   name,
                          char**        value ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_GetTraceId( OTF2_Archive* archive,
                         uint64_t*     id ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_GetNumberOfLocations( OTF2_Archive* archive,
                                   uint64_t*     numberOfLocations ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_GetNumberOfGlobalDefinitions( OTF2_Archive* archive,
                                           uint64_t*     numberOfDefinitions ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_GetMachineName( OTF2_Archive* archive,
                             char**        machineName ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_GetDescription( OTF2_Archive* archive,
                             char**        description ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_GetCreator( OTF2_Archive* archive,
                         char**        creator ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_GetVersion( OTF2_Archive* archive,
                         uint8_t*      major,
                         uint8_t*      minor,
                         uint8_t*      bugfix ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_GetChunkSize( OTF2_Archive* archive,
                           uint64_t*     chunkSizeEvents,
                           uint64_t*     chunkSizeDefs ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_GetFileSubstrate( OTF2_Archive*       archive,
                               OTF2_FileSubstrate* substrate ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_GetCompression( OTF2_Archive*     archive,
                             OTF2_Compression* compression ) {
  NOT_IMPLEMENTED;
}


int new_location(OTF2_Archive* archive, OTF2_LocationRef location) {
  int index = archive->nb_locations++;

  if(index == 0) {
    /* TODO: hacky ! we should not have to do that ! */
    archive->archive.id = location;
  }

  archive->def_writers = realloc(archive->def_writers, sizeof(OTF2_DefWriter*) * archive->nb_locations);
  archive->evt_writers = realloc(archive->evt_writers, sizeof(OTF2_EvtWriter*) * archive->nb_locations);

  archive->def_writers[index] = malloc(sizeof(OTF2_DefWriter));
  archive->def_writers[index]->locationRef = location;
  archive->def_writers[index]->archive = &archive->archive;
  archive->def_writers[index]->thread_writer = malloc(sizeof(struct htf_thread_writer));

  htf_write_thread_open(&archive->archive,
			archive->def_writers[index]->thread_writer,
			location);
  
#if 0
  htf_write_init_thread(&archive->trace,
			,
			location,
			index);
#endif

  archive->evt_writers[index] = malloc(sizeof(OTF2_EvtWriter));
  archive->evt_writers[index]->locationRef = location;
  archive->evt_writers[index]->archive = &archive->archive;
  archive->evt_writers[index]->thread_writer = archive->def_writers[index]->thread_writer;
  return index;
}

OTF2_EvtWriter*
OTF2_Archive_GetEvtWriter( OTF2_Archive*    archive,
                           OTF2_LocationRef location ) {
	pthread_mutex_lock(&archive->lock);
  printf("OTF2_Archive_GetEvtWriter (%lu)\n", location);
  for(int i=0; i<archive->nb_locations; i++) {
    if(archive->evt_writers[i]->locationRef == location) {
      printf("\t->%d (.location=%lu, .writer=%p)\n", i,
	     archive->evt_writers[i]->locationRef,
	     archive->evt_writers[i]->thread_writer);

      //      htf_assert(archive->evt_writers[i]->thread_writer->thread_trace.container);
	    pthread_mutex_unlock(&archive->lock);
      return archive->evt_writers[i];
    }
  }

  int index = new_location(archive, location);

	printf("New EvtWriter (ref=%lu, writer=%p)\n",
	       archive->evt_writers[index]->locationRef,
	       archive->evt_writers[index]->thread_writer);
  
  //  htf_assert(archive->evt_writers[index]->thread_writer->thread_trace.container);
	pthread_mutex_unlock(&archive->lock);
  return archive->evt_writers[index];
}

OTF2_DefWriter*
OTF2_Archive_GetDefWriter( OTF2_Archive*    archive,
                           OTF2_LocationRef location ) {
	pthread_mutex_lock(&archive->lock);
  for(int i=0; i<archive->nb_locations; i++) {
    if(archive->def_writers[i]->locationRef == location) {
	    pthread_mutex_unlock(&archive->lock);
	    return archive->def_writers[i];
    }
  }
  int index = new_location(archive, location);

  printf("New DefWriter (ref=%lu, writer=%p)\n",
	 archive->evt_writers[index]->locationRef,
	 archive->evt_writers[index]->thread_writer);
	pthread_mutex_unlock(&archive->lock);
  return archive->def_writers[index];
}

OTF2_GlobalDefWriter*
OTF2_Archive_GetGlobalDefWriter( OTF2_Archive* archive ) {
  if(!archive->globalDefWriter) {
    archive->globalDefWriter = malloc(sizeof(OTF2_GlobalDefWriter));

    htf_write_global_archive_open(&archive->globalDefWriter->archive,
				  archive->archive.dir_name,
				  archive->archive.trace_name);

  }
  return archive->globalDefWriter;
}

OTF2_SnapWriter*
OTF2_Archive_GetSnapWriter( OTF2_Archive*    archive,
                            OTF2_LocationRef location ) {
  NOT_IMPLEMENTED;
}

OTF2_ThumbWriter*
OTF2_Archive_GetThumbWriter( OTF2_Archive*      archive,
                             const char*        name,
                             const char*        description,
                             OTF2_ThumbnailType type,
                             uint32_t           numberOfSamples,
                             uint32_t           numberOfMetrics,
                             const uint64_t*    refsToDefs ) {
  NOT_IMPLEMENTED;
}

OTF2_MarkerWriter*
OTF2_Archive_GetMarkerWriter( OTF2_Archive* archive ) {
  NOT_IMPLEMENTED;
}

OTF2_EvtReader*
OTF2_Archive_GetEvtReader( OTF2_Archive*    archive,
                           OTF2_LocationRef location ) {
  NOT_IMPLEMENTED;
}

OTF2_GlobalEvtReader*
OTF2_Archive_GetGlobalEvtReader( OTF2_Archive* archive ) {
  NOT_IMPLEMENTED;
}

OTF2_DefReader*
OTF2_Archive_GetDefReader( OTF2_Archive*    archive,
                           OTF2_LocationRef location ) {
  NOT_IMPLEMENTED;
}

OTF2_GlobalDefReader*
OTF2_Archive_GetGlobalDefReader( OTF2_Archive* archive ) {
  NOT_IMPLEMENTED;
}

OTF2_SnapReader*
OTF2_Archive_GetSnapReader( OTF2_Archive*    archive,
                            OTF2_LocationRef location ) {
  NOT_IMPLEMENTED;
}

OTF2_GlobalSnapReader*
OTF2_Archive_GetGlobalSnapReader( OTF2_Archive* archive ) {
  NOT_IMPLEMENTED;
}

OTF2_ThumbReader*
OTF2_Archive_GetThumbReader( OTF2_Archive* archive,
                             uint32_t      number ) {
  NOT_IMPLEMENTED;
}

OTF2_MarkerReader*
OTF2_Archive_GetMarkerReader( OTF2_Archive* archive ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_CloseEvtWriter( OTF2_Archive*   archive,
                             OTF2_EvtWriter* writer ) {
  //  NOT_IMPLEMENTED;
  htf_write_thread_close(writer->thread_writer);
  return OTF2_SUCCESS;
}

OTF2_ErrorCode
OTF2_Archive_CloseDefWriter( OTF2_Archive*   archive,
                             OTF2_DefWriter* writer ) {
  //  NOT_IMPLEMENTED;
  //TODO: something like this ?

  return OTF2_SUCCESS;
}

OTF2_ErrorCode
OTF2_Archive_CloseMarkerWriter( OTF2_Archive*      archive,
                                OTF2_MarkerWriter* writer ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_CloseSnapWriter( OTF2_Archive*    archive,
                              OTF2_SnapWriter* writer ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_CloseGlobalDefWriter( OTF2_Archive*         archive,
                                   OTF2_GlobalDefWriter* writer ) {

  htf_write_global_archive_close(&writer->archive);
  return OTF2_SUCCESS;
}

OTF2_ErrorCode
OTF2_Archive_CloseEvtReader( OTF2_Archive*   archive,
                             OTF2_EvtReader* reader ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_CloseThumbReader( OTF2_Archive*     archive,
                               OTF2_ThumbReader* reader ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_CloseGlobalEvtReader( OTF2_Archive*         archive,
                                   OTF2_GlobalEvtReader* globalEvtReader ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_CloseDefReader( OTF2_Archive*   archive,
                             OTF2_DefReader* reader ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_CloseGlobalDefReader( OTF2_Archive*         archive,
                                   OTF2_GlobalDefReader* globalDefReader ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_CloseSnapReader( OTF2_Archive*    archive,
                              OTF2_SnapReader* reader ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_CloseGlobalSnapReader( OTF2_Archive*          archive,
                                    OTF2_GlobalSnapReader* globalSnapReader ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_CloseMarkerReader( OTF2_Archive*      archive,
                                OTF2_MarkerReader* markerReader ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_GetNumberOfThumbnails( OTF2_Archive* archive,
                                    uint32_t*     number ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_SetNumberOfSnapshots( OTF2_Archive* archive,
                                   uint32_t      number ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_GetNumberOfSnapshots( OTF2_Archive* archive,
                                   uint32_t*     number ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_OpenEvtFiles( OTF2_Archive* archive ) {
  return OTF2_SUCCESS;
}

OTF2_ErrorCode
OTF2_Archive_CloseEvtFiles( OTF2_Archive* archive ) {
  return OTF2_SUCCESS;
}

OTF2_ErrorCode
OTF2_Archive_OpenDefFiles( OTF2_Archive* archive ) {
  //  NOT_IMPLEMENTED;
  return OTF2_SUCCESS;
}

OTF2_ErrorCode
OTF2_Archive_CloseDefFiles( OTF2_Archive* archive ) {
  //  NOT_IMPLEMENTED;
  return OTF2_SUCCESS;
}

OTF2_ErrorCode
OTF2_Archive_OpenSnapFiles( OTF2_Archive* archive ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_CloseSnapFiles( OTF2_Archive* archive ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_SelectLocation( OTF2_Archive*    archive,
                             OTF2_LocationRef location ) {
  NOT_IMPLEMENTED;
}






