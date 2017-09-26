////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2016 ArangoDB GmbH, Cologne, Germany
/// Copyright 2004-2014 triAGENS GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Jan Steemann
/// @author Jan Christoph Uhde
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGOD_REST_HANDLER_REST_REPLICATION_HANDLER_H
#define ARANGOD_REST_HANDLER_REST_REPLICATION_HANDLER_H 1

#include "Basics/Common.h"

#include "RestHandler/RestVocbaseBaseHandler.h"
#include "VocBase/replication-common.h"

namespace arangodb {
class ClusterInfo;
class CollectionNameResolver;
class LogicalCollection;
class SingleCollectionTransaction;

namespace transaction {
class Methods;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief replication request handler
////////////////////////////////////////////////////////////////////////////////

class RestReplicationHandler : public RestVocbaseBaseHandler {
 public:
  RestStatus execute() override;

  // Never instantiate this.
  // Only specific implementations allowed
 protected:
  RestReplicationHandler(GeneralRequest*, GeneralResponse*);
  ~RestReplicationHandler();

 protected:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief creates an error if called on a coordinator server
  //////////////////////////////////////////////////////////////////////////////

  bool isCoordinatorError();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief turn the server into a slave of another
  //////////////////////////////////////////////////////////////////////////////

  void handleCommandMakeSlave();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief forward a command in the coordinator case
  //////////////////////////////////////////////////////////////////////////////

  void handleTrampolineCoordinator();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief returns the cluster inventory, only on coordinator
  //////////////////////////////////////////////////////////////////////////////

  void handleCommandClusterInventory();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief handle a restore command for a specific collection
  //////////////////////////////////////////////////////////////////////////////

  void handleCommandRestoreCollection();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief handle a restore command for a specific collection
  //////////////////////////////////////////////////////////////////////////////

  void handleCommandRestoreIndexes();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief handle a restore command for a specific collection
  //////////////////////////////////////////////////////////////////////////////

  void handleCommandRestoreData();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief handle a server-id command
  //////////////////////////////////////////////////////////////////////////////

  void handleCommandServerId();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief handle a sync command
  //////////////////////////////////////////////////////////////////////////////

  void handleCommandSync();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief return the configuration of the the replication applier
  //////////////////////////////////////////////////////////////////////////////

  void handleCommandApplierGetConfig();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief configure the replication applier
  //////////////////////////////////////////////////////////////////////////////

  void handleCommandApplierSetConfig();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief start the replication applier
  //////////////////////////////////////////////////////////////////////////////

  void handleCommandApplierStart();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief stop the replication applier
  //////////////////////////////////////////////////////////////////////////////

  void handleCommandApplierStop();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief return the state of the replication applier
  //////////////////////////////////////////////////////////////////////////////

  void handleCommandApplierGetState();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief delete the replication applier state
  //////////////////////////////////////////////////////////////////////////////

  void handleCommandApplierDeleteState();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief add a follower of a shard to the list of followers
  //////////////////////////////////////////////////////////////////////////////

  void handleCommandAddFollower();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief remove a follower of a shard from the list of followers
  //////////////////////////////////////////////////////////////////////////////

  void handleCommandRemoveFollower();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief hold a read lock on a collection to stop writes temporarily
  //////////////////////////////////////////////////////////////////////////////

  void handleCommandHoldReadLockCollection();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief check if we are holding a read lock on a collection
  //////////////////////////////////////////////////////////////////////////////

  void handleCommandCheckHoldReadLockCollection();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief cancel holding a read lock on a collection
  //////////////////////////////////////////////////////////////////////////////

  void handleCommandCancelHoldReadLockCollection();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief get an ID for a hold read lock job
  //////////////////////////////////////////////////////////////////////////////

  void handleCommandGetIdForReadLockCollection();
  
  //////////////////////////////////////////////////////////////////////////////
  /// @brief return the state of the replication logger
  /// @route GET logger-state
  /// @caller Syncer::getMasterState
  /// @response VPackObject describing the ServerState in a certain point
  ///           * state (server state)
  ///           * server (version / id)
  ///           * clients (list of followers)
  //////////////////////////////////////////////////////////////////////////////
  
  void handleCommandLoggerState();
  
  //////////////////////////////////////////////////////////////////////////////
  /// @brief return the first tick available in a logfile
  /// @route GET logger-first-tick
  /// @caller js/client/modules/@arangodb/replication.js
  /// @response VPackObject with minTick of LogfileManager->ranges()
  //////////////////////////////////////////////////////////////////////////////
  
  void handleCommandLoggerFirstTick();
  
  //////////////////////////////////////////////////////////////////////////////
  /// @brief return the available logfile range
  /// @route GET logger-tick-ranges
  /// @caller js/client/modules/@arangodb/replication.js
  /// @response VPackArray, containing info about each datafile
  ///           * filename
  ///           * status
  ///           * tickMin - tickMax
  //////////////////////////////////////////////////////////////////////////////
  
  void handleCommandLoggerTickRanges();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief determine chunk size from request
  ///        Reads chunkSize attribute from request
  //////////////////////////////////////////////////////////////////////////////

  uint64_t determineChunkSize() const;
  
  //////////////////////////////////////////////////////////////////////////////
  /// @brief Grant temporary restore rights
  //////////////////////////////////////////////////////////////////////////////
  void grantTemporaryRights();

 private:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief restores the structure of a collection
  //////////////////////////////////////////////////////////////////////////////

  int processRestoreCollection(VPackSlice const&, bool, bool, bool,
                               std::string&);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief restores the structure of a collection, coordinator case
  //////////////////////////////////////////////////////////////////////////////

  int processRestoreCollectionCoordinator(VPackSlice const&, bool, bool, bool,
                                          uint64_t, std::string&, uint64_t,
                                          bool);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief restores the data of the _users collection
  //////////////////////////////////////////////////////////////////////////////

  Result processRestoreUsersBatch(std::string const& colName, bool useRevision);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief restores the data of a collection
  //////////////////////////////////////////////////////////////////////////////

  Result processRestoreDataBatch(transaction::Methods& trx,
                                 std::string const& colName, bool useRevision);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief restores the indexes of a collection
  //////////////////////////////////////////////////////////////////////////////

  int processRestoreIndexes(VPackSlice const&, bool, std::string&);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief restores the indexes of a collection, coordinator case
  //////////////////////////////////////////////////////////////////////////////

  int processRestoreIndexesCoordinator(VPackSlice const&, bool, std::string&);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief restores the data of a collection
  //////////////////////////////////////////////////////////////////////////////

  Result processRestoreData(std::string const& colName, bool useRevision);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief parse an input batch
  //////////////////////////////////////////////////////////////////////////////

  Result parseBatch(std::string const& collectionName, bool useRevision,
                    std::unordered_map<std::string, VPackValueLength>& latest,
                    VPackBuilder& allMarkers);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief creates a collection, based on the VelocyPack provided
  //////////////////////////////////////////////////////////////////////////////

  int createCollection(VPackSlice, arangodb::LogicalCollection**, bool);

 protected:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief condition locker to wake up holdReadLockCollection jobs
  //////////////////////////////////////////////////////////////////////////////

  static arangodb::basics::ConditionVariable _condVar;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief global set of ids of holdReadLockCollection jobs, an
  /// id mapping to false here indicates that a request to get the
  /// read lock has been started, the mapped value holds the
  /// actual transaction. if the read lock is not yet acquired, the
  /// shared_ptr will contain a nullptr still.
  /// To cancel the read lock,
  /// remove the entry here (under the protection of the mutex of
  /// condVar) and send a broadcast to the condition variable,
  /// the job with that id is terminated. If it timeouts, then
  /// the read lock is released automatically and the entry here
  /// is deleted.
  //////////////////////////////////////////////////////////////////////////////

  static std::unordered_map<std::string,
                            std::shared_ptr<SingleCollectionTransaction>>
      _holdReadLockJobs;

 private:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief minimum chunk size
  //////////////////////////////////////////////////////////////////////////////

  static uint64_t const _defaultChunkSize;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief maximum chunk size
  //////////////////////////////////////////////////////////////////////////////

  static uint64_t const _maxChunkSize;

 protected:
  //////////////////////////////////////////////////////////////////////////////
  /// SECTION:
  /// Functions to be implemented by specialisation
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  /// @brief handle a follow command for the replication log
  //////////////////////////////////////////////////////////////////////////////

  virtual void handleCommandLoggerFollow() = 0;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief handle the command to determine the transactions that were open
  /// at a certain point in time
  //////////////////////////////////////////////////////////////////////////////

  virtual void handleCommandDetermineOpenTransactions() = 0;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief handle a batch command
  //////////////////////////////////////////////////////////////////////////////

  virtual void handleCommandBatch() = 0;
  
  //////////////////////////////////////////////////////////////////////////////
  /// @brief add or remove a WAL logfile barrier
  //////////////////////////////////////////////////////////////////////////////
  
  virtual void handleCommandBarrier() = 0;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief return the inventory (current replication and collection state)
  //////////////////////////////////////////////////////////////////////////////

  virtual void handleCommandInventory() = 0;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief produce list of keys for a specific collection
  //////////////////////////////////////////////////////////////////////////////

  virtual void handleCommandCreateKeys() = 0;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief returns a key range
  //////////////////////////////////////////////////////////////////////////////

  virtual void handleCommandGetKeys() = 0;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief returns date for a key range
  //////////////////////////////////////////////////////////////////////////////

  virtual void handleCommandFetchKeys() = 0;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief remove a list of keys for a specific collection
  //////////////////////////////////////////////////////////////////////////////

  virtual void handleCommandRemoveKeys() = 0;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief handle a dump command for a specific collection
  //////////////////////////////////////////////////////////////////////////////

  virtual void handleCommandDump() = 0;
};
}
#endif
