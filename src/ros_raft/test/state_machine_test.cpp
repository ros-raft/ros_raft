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

#include <algorithm>

#include "gtest/gtest.h"

class StateMachineTest : public ::testing::Test
{
protected:
  ros_raft::RaftStateMachine sm_;
};

TEST_F(StateMachineTest, InitializesToFollower)
{
  ASSERT_EQ(sm_.GetState(), ros_raft::NodeState::FOLLOWER);
}

TEST_F(StateMachineTest, RegistersValidStateTransitions)
{
  const auto f_trans = sm_.GetValidTransitions(ros_raft::NodeState::FOLLOWER);
  const auto c_trans = sm_.GetValidTransitions(ros_raft::NodeState::CANDIDATE);
  const auto l_trans = sm_.GetValidTransitions(ros_raft::NodeState::LEADER);

  ASSERT_EQ(f_trans.size(), 1);
  ASSERT_EQ(c_trans.size(), 3);
  ASSERT_EQ(l_trans.size(), 1);

  ASSERT_NE(
    std::find(f_trans.begin(), f_trans.end(), ros_raft::NodeState::CANDIDATE), f_trans.end());

  ASSERT_NE(std::find(c_trans.begin(), c_trans.end(), ros_raft::NodeState::LEADER), c_trans.end());
  ASSERT_NE(
    std::find(c_trans.begin(), c_trans.end(), ros_raft::NodeState::CANDIDATE), c_trans.end());
  ASSERT_NE(
    std::find(c_trans.begin(), c_trans.end(), ros_raft::NodeState::FOLLOWER), c_trans.end());

  ASSERT_NE(
    std::find(l_trans.begin(), l_trans.end(), ros_raft::NodeState::FOLLOWER), l_trans.end());
}

TEST_F(StateMachineTest, InvalidStateTransitionsNoThrow)
{
  // Deliberately cast invalid enum values to attempt to force an exception
  ASSERT_NO_THROW(sm_.GetValidTransitions(static_cast<ros_raft::NodeState>(-1)));
  ASSERT_NO_THROW(sm_.GetValidTransitions(static_cast<ros_raft::NodeState>(5)));
}

TEST_F(StateMachineTest, ValidTransitionsSucceed)
{
  // Follower -> Candidate
  ASSERT_TRUE(sm_.TransitionState(ros_raft::NodeState::CANDIDATE));
  // Candidate -> Candidate
  ASSERT_TRUE(sm_.TransitionState(ros_raft::NodeState::CANDIDATE));
  // Candidate -> Follower
  ASSERT_TRUE(sm_.TransitionState(ros_raft::NodeState::FOLLOWER));
  // Follower -> Candidate (again)
  ASSERT_TRUE(sm_.TransitionState(ros_raft::NodeState::CANDIDATE));
  // Candidate -> Leader
  ASSERT_TRUE(sm_.TransitionState(ros_raft::NodeState::LEADER));
  // Leader -> Follower
  ASSERT_TRUE(sm_.TransitionState(ros_raft::NodeState::FOLLOWER));
}

TEST_F(StateMachineTest, InvalidTransitionsFail)
{
  // Make sure we know what state we're in
  ASSERT_EQ(sm_.GetState(), ros_raft::NodeState::FOLLOWER);

  ASSERT_FALSE(sm_.TransitionState(ros_raft::NodeState::LEADER));
  ASSERT_FALSE(sm_.TransitionState(ros_raft::NodeState::FOLLOWER));

  // There aren't any invalid transitions for the candidate state,
  // so go straight to leader
  sm_.TransitionState(ros_raft::NodeState::CANDIDATE);
  sm_.TransitionState(ros_raft::NodeState::LEADER);

  // Double check we're in Leader state
  ASSERT_EQ(sm_.GetState(), ros_raft::NodeState::LEADER);

  ASSERT_FALSE(sm_.TransitionState(ros_raft::NodeState::CANDIDATE));
  ASSERT_FALSE(sm_.TransitionState(ros_raft::NodeState::LEADER));
}
