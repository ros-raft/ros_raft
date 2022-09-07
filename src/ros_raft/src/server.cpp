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

#include "ros_raft/server.hpp"

ros_raft::Server::Server() : rclcpp::Node("test")
{ 
    RCLCPP_INFO(get_logger(), "Initialized ROS Raft server [ %s ]", get_name());

    auto cb_group = create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive, false);
    append_entries_srv_ = create_service<ros_raft_interfaces::srv::AppendEntries>(
        std::string("/append_entries"),
        std::bind(&Server::appendEntriesCallback, this,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3),
        rmw_qos_profile_services_default,
        cb_group);

    request_vote_srv_ = create_service<ros_raft_interfaces::srv::RequestVote>(
        std::string("/request_vote"),
        std::bind(&Server::requestVoteCallback, this,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3),
        rmw_qos_profile_services_default,
        cb_group);

    append_entries_client_ = create_client<ros_raft_interfaces::srv::AppendEntries>(
        std::string("/append_entries"),
        rmw_qos_profile_services_default,
        cb_group);

    request_vote_client_ = create_client<ros_raft_interfaces::srv::RequestVote>(
        std::string("/request_vote"),
        rmw_qos_profile_services_default,
        cb_group);

}

void ros_raft::Server::appendEntriesCallback(
    const std::shared_ptr<rmw_request_id_t> req_header,
    const std::shared_ptr<ros_raft_interfaces::srv::AppendEntries::Request> request,
    std::shared_ptr<ros_raft_interfaces::srv::AppendEntries::Response> response
)
{
    (void)req_header;
    (void)request;
    (void)response;

}
void ros_raft::Server::requestVoteCallback(
    const std::shared_ptr<rmw_request_id_t> req_header,
    const std::shared_ptr<ros_raft_interfaces::srv::RequestVote::Request> request,
    std::shared_ptr<ros_raft_interfaces::srv::RequestVote::Response> response
)
{
    (void)req_header;
    (void)request;
    (void)response;
}
