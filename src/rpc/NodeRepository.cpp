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

#include "NodeRepository.hpp"
#include "../serialization/serializator.hpp"

#include <sstream>
#include <thread>
#include <chrono>

#include <libusockets.h>

namespace rpc {
	
	NodeRepository::NodeRepository() : atomicNodeIds(1) {
	}
	
	NodeRepository::~NodeRepository() {
	}
	
	void NodeRepository::OnOpenSocket(net::Socket* socket,
			bool isClient, char* ip, int ipLength) {
		Node* node = InternalGetNode();
		if(it == ipNodes.end()) {
			node = new Node();
			node->nodeId = ++(Singleton()->atomicNodeIds);
		} else {
			node = it->second;
		}
		if(node) {
			node->connecting = false;
			node->socket = socket;
			node->ip = ipString;
			// TODO search by port
			node->port = us_socket_local_port(socket->ssl, socket->socket);
			socket->userData = node;
		}
	}
	
	void NodeRepository::OnCloseSocket(net::Socket* socket,
			int ec, void* edata) {
		auto it = socketNodes.find(socket);
		if(it != socketNodes.end()) {
			it->second->socket = NULL;
			socketNodes.erase(it);
		}
	}
}

