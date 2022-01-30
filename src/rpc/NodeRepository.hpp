/*
 *  This file is part of DORPC. Please see README for details.
 *  Copyright (C) 2022 Marek Zalewski aka Drwalin
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef DORPC_RPC_NODE_REPOSITORY_HPP
#define DORPC_RPC_NODE_REPOSITORY_HPP

#include <unordered_map>
#include <functional>
#include <vector>
#include <cinttypes>
#include <thread>
#include <memory>

#include "Function.hpp"
#include "FunctionRegistry.hpp"
#include "../serialization/serializator.hpp"
#include "../networking/Context.hpp"
#include "../networking/Loop.hpp"
#include "../networking/Socket.hpp"

namespace rpc {
	struct Node {
		net::Socket* socket;
		uint32_t nodeId;
		std::string ip;
		int port;
		bool connecting;
	};

	class NodeRepository {
	public:
		NodeRepository();
		~NodeRepository();
		
		Node* InternalGetNode(uint32_t nodeId);
		Node* InternalGetNode(const std::string& ipString, int port);
		Node* InternalGetNode(net::Socket* socket);
		void InternalSetNodeId(Node* node, uint32_t nodeId);
		void InternalSetIp(Node* node, const std::string& ipString, int port);
		void InternalSetNodeSocket(Node* node, net::Socket* socket);
		
		
		void InternalHandshakeClientSide(net::Socket* socket);
		void InternalHandshakeServerSide(net::Socket* socket);
		static void InternalHandshakeReceiveFromServer(net::Buffer& buffer);
		static void InternalHandshakeReceiveFromClient(net::Buffer& buffer);
		
		void OnOpenSocket(net::Socket* socket, bool isClient);
		void OnCloseSocket(net::Socket* socket);
		
		void Disconnect(uint32_t nodeId);
		
	private:
		
		std::unordered_map<uint32_t, Node*> nodes;
		std::unordered_map<std::string, Node*> ipNodes;
		std::unordered_map<net::Socket*, Node*> socketNodes;
		
		std::atomic<uint32_t> atomicNodeIds;
	};
}

#endif

