/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */
/** @file
 * Everything that has to do with writing a trace during an execution.
 */
#pragma once
#include "htf.h"
#include "htf_archive.h"
#include "htf_attribute.h"
#ifdef __cplusplus
namespace htf {
#endif
/**
 * @brief Writes one thread to the HTF trace format.
 */
typedef struct ThreadWriter {
  Thread thread_trace; /**< Thread being written. */
  Sequence** og_seq;   /**< Array of pointers to sequences. todo: Complete this. */
  int cur_depth;       /**< Current depth in the callstack. */
  int max_depth;       /**< Maximum depth in the callstack. */
  int thread_rank;     /**< Rank of this thread. todo: MPI rank ? */
#ifdef __cplusplus
 private:
  void findLoopBasic(size_t maxLoopLength);
  void findLoopFilter();
  /** @brief Tries to find a Loop in the current array of tokens.  */
  void findLoop();
  /** @brief Creates a Loop in the trace, and returns a pointer to it.
   * Does not change the current array of tokens.
   * @param start_index Starting index of the loop (first token in the loop).
   * @param loop_len Lenght of the sequence repeated in the loop.
   * */
  Loop* createLoop(int start_index, int loop_len);
  /** @brief Create a Loop and change the current array of token to reflect that.
   *
   * For example, replaces e1 e2 e3 e4 e1 e2 e3 e4 with l1.
   * @param loop_len Length of the sequence repeated in the loop.
   * @param index_first_iteration Starting index of the first iteration of the loop.
   * @param index_second_iteration Starting index of the second iteration of the loop.
   */
  void replaceTokensInLoop(int loop_len, size_t index_first_iteration, size_t index_second_iteration);
  /** @brief Returns a pointer to the current Sequence being written. */
  [[nodiscard]] Sequence* getCurrentSequence() const { return og_seq[cur_depth]; };
  /** @brief Stores the timestamp in the given EventSummary. */
  void storeTimestamp(EventSummary* es, htf_timestamp_t ts);
  /** @brief Stores the attribute list in the given EventSummary. */
  void storeAttributeList(EventSummary* es, AttributeList* attribute_list, size_t occurence_index);
  /** @brief Stores the tokens in that Sequence's array of Tokens, then tries to find a Loop.*/
  void storeToken(Sequence* seq, Token t);
  /** @brief Move up the callstack and create a new Sequence. */
  void recordEnterFunction();
  /** @brief Close a Sequence and move down the callstack. */
  void recordExitFunction();

 public:
  void open(Archive* archive, ThreadId thread_id);
  void threadClose();
  /** Creates the new Event and stores it. Returns the occurence index of that new Event. */
  size_t storeEvent(enum EventType event_type,
                    TokenId event_id,
                    htf_timestamp_t ts,
                    struct AttributeList* attribute_list);

#endif
} ThreadWriter;
#ifdef __cplusplus
}
extern "C" {
#endif

/* C Callbacks */

extern void htf_write_global_archive_open(HTF(Archive) * archive, const char* dir_name, const char* trace_name);
extern void htf_write_global_archive_close(HTF(Archive) * archive);

extern void htf_write_thread_open(HTF(Archive) * archive, HTF(ThreadWriter) * thread_writer, HTF(ThreadId) thread_id);

extern void htf_write_thread_close(HTF(ThreadWriter) * thread_writer);

extern void htf_write_define_location_group(HTF(Archive) * archive,
                                            HTF(LocationGroupId) id,
                                            HTF(StringRef) name,
                                            HTF(LocationGroupId) parent);

extern void htf_write_define_location(HTF(Archive) * archive,
                                      HTF(ThreadId) id,
                                      HTF(StringRef) name,
                                      HTF(LocationGroupId) parent);

extern void htf_write_archive_open(HTF(Archive) * archive,
                                   const char* dir_name,
                                   const char* trace_name,
                                   HTF(LocationGroupId) location_group);

extern void htf_write_archive_close(HTF(Archive) * archive);

extern void htf_store_event(HTF(ThreadWriter) * thread_writer,
                            enum HTF(EventType) event_type,
                            HTF(TokenId) id,
                            htf_timestamp_t ts,
                            HTF(AttributeList) * attribute_list);

/* Event Records */

extern void htf_record_enter(HTF(ThreadWriter) * thread_writer,
                             HTF(AttributeList) * attributeList,
                             htf_timestamp_t time,
                             HTF(RegionRef) region_ref);

extern void htf_record_leave(HTF(ThreadWriter) * thread_writer,
                             HTF(AttributeList) * attributeList,
                             htf_timestamp_t time,
                             HTF(RegionRef) region_ref);

extern void htf_record_thread_begin(HTF(ThreadWriter) * thread_writer,
                                    HTF(AttributeList) * attributeList,
                                    htf_timestamp_t time);

extern void htf_record_thread_end(HTF(ThreadWriter) * thread_writer,
                                  HTF(AttributeList) * attributeList,
                                  htf_timestamp_t time);

extern void htf_record_thread_team_begin(HTF(ThreadWriter) * thread_writer,
                                         HTF(AttributeList) * attributeList,
                                         htf_timestamp_t time);

extern void htf_record_thread_team_end(HTF(ThreadWriter) * thread_writer,
                                       HTF(AttributeList) * attributeList,
                                       htf_timestamp_t time);

extern void htf_record_mpi_send(HTF(ThreadWriter) * thread_writer,
                                HTF(AttributeList) * attributeList,
                                htf_timestamp_t time,
                                uint32_t receiver,
                                uint32_t communicator,
                                uint32_t msgTag,
                                uint64_t msgLength);

extern void htf_record_mpi_isend(HTF(ThreadWriter) * thread_writer,
                                 HTF(AttributeList) * attribute_list,
                                 htf_timestamp_t time,
                                 uint32_t receiver,
                                 uint32_t communicator,
                                 uint32_t msgTag,
                                 uint64_t msgLength,
                                 uint64_t requestID);

extern void htf_record_mpi_isend_complete(HTF(ThreadWriter) * thread_writer,
                                          HTF(AttributeList) * attribute_list,
                                          htf_timestamp_t time,
                                          uint64_t requestID);

extern void htf_record_mpi_irecv_request(HTF(ThreadWriter) * thread_writer,
                                         HTF(AttributeList) * attribute_list,
                                         htf_timestamp_t time,
                                         uint64_t requestID);

extern void htf_record_mpi_recv(HTF(ThreadWriter) * thread_writer,
                                HTF(AttributeList) * attributeList,
                                htf_timestamp_t time,
                                uint32_t sender,
                                uint32_t communicator,
                                uint32_t msgTag,
                                uint64_t msgLength);

extern void htf_record_mpi_irecv(HTF(ThreadWriter) * thread_writer,
                                 HTF(AttributeList) * attribute_list,
                                 htf_timestamp_t time,
                                 uint32_t sender,
                                 uint32_t communicator,
                                 uint32_t msgTag,
                                 uint64_t msgLength,
                                 uint64_t requestID);

extern void htf_record_mpi_collective_begin(HTF(ThreadWriter) * thread_writer,
                                            HTF(AttributeList) * attribute_list,
                                            htf_timestamp_t time);

extern void htf_record_mpi_collective_end(HTF(ThreadWriter) * thread_writer,
                                          HTF(AttributeList) * attribute_list,
                                          htf_timestamp_t time,
                                          uint32_t collectiveOp,
                                          uint32_t communicator,
                                          uint32_t root,
                                          uint64_t sizeSent,
                                          uint64_t sizeReceived);

#ifdef __cplusplus
};
#endif

extern C_CXX(_Thread_local, thread_local) int htf_recursion_shield;

/* -*-
   mode: c;
   c-file-style: "k&r";
   c-basic-offset 2;
   tab-width 2 ;
   indent-tabs-mode nil
   -*- */
