// Copyright 2022 John Farrell
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.

#include "ros_raft/state_machine.hpp"

// Create the map of valid transitions between states
const std::map<ros_raft::NodeState, std::vector<ros_raft::NodeState>>
  ros_raft::RaftStateMachine::kTransition_map_ = {
    {ros_raft::NodeState::FOLLOWER, {ros_raft::NodeState::CANDIDATE}},
    {ros_raft::NodeState::CANDIDATE,
     {ros_raft::NodeState::FOLLOWER, ros_raft::NodeState::CANDIDATE, ros_raft::NodeState::LEADER}},
    {ros_raft::NodeState::LEADER, {ros_raft::NodeState::FOLLOWER}}};

ros_raft::RaftStateMachine::RaftStateMachine() : current_state_(ros_raft::NodeState::FOLLOWER) {}

ros_raft::NodeState ros_raft::RaftStateMachine::GetState() const
{
  std::shared_lock lock(state_mtx_);
  return current_state_;
}

bool ros_raft::RaftStateMachine::TransitionState(const ros_raft::NodeState & state)
{
  std::unique_lock lock(state_mtx_);

  // We don't use the GetValidTransitions() override here because we
  // want to hold this unique lock for the duration of this function
  auto valid_transitions = GetValidTransitions(current_state_);

  // Make sure the transition is valid
  if (
    std::find(valid_transitions.begin(), valid_transitions.end(), state) ==
    valid_transitions.end()) {
    return false;
  }

  current_state_ = state;
  return true;
}

std::vector<ros_raft::NodeState> ros_raft::RaftStateMachine::GetValidTransitions() const
{
  std::shared_lock lock(state_mtx_);
  return GetValidTransitions(current_state_);
}

std::vector<ros_raft::NodeState> ros_raft::RaftStateMachine::GetValidTransitions(
  const ros_raft::NodeState & state) const
{
  try {
    return kTransition_map_.at(state);
  } catch (const std::out_of_range & e) {
    return {};
  }
}
