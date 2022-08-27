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

#include <algorithm>
#include <functional>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

#include "ros_raft/node_state.hpp"

#ifndef ROS_RAFT__STATE_MACHINE_HPP_
#define ROS_RAFT__STATE_MACHINE_HPP_

namespace ros_raft
{

typedef std::pair<ros_raft::NodeState, ros_raft::NodeState> StateTransition;
typedef std::function<void(const StateTransition & transition)> StateTransitionCallback;

class RaftStateMachine
{
public:
  /**
   * @brief Construct a new Raft State Machine and
   * initializes the state to Follower
   */
  RaftStateMachine();

  /**
   * @brief Gets the currently active state
   *
   * @return NodeState
   */
  NodeState GetState() const;

  /**
   * @brief Gets a list of valid states that could
   * be transitioned to from the currently active state
   *
   * @return std::vector<NodeState> List of states that could be transitioned to
   */
  std::vector<NodeState> GetValidTransitions() const;

  /**
   * @brief Gets a list of valid states that could
   * be transitioned to from @p state
   *
   * @param[in] state State to get valid transitions state from
   * @return std::vector<NodeState> List of valid transitions from @p state.
   */
  std::vector<NodeState> GetValidTransitions(const NodeState & state) const;

  /**
   * @brief Attempts to transition the current state to @p newState
   *
   * @param newState State to transition to
   * @return true The transition was successful
   * @return false The transition failed
   */
  bool TransitionState(const NodeState & newState);

  void RegisterTransitionCallback(
    const StateTransition & transition, StateTransitionCallback callback);

private:
  static const std::unordered_map<NodeState, std::vector<NodeState>> kTransition_map_;

  std::map<StateTransition, StateTransitionCallback> transition_callback_map_;

  mutable std::shared_mutex state_mtx_;

  NodeState current_state_;
};

}  // namespace ros_raft

#endif  // ROS_RAFT__STATE_MACHINE_HPP_
