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

#include "RpcNetworkingContext.hpp"
#include "../serialization/serializator.hpp"

#include <sstream>
#include <thread>
#include <chrono>

#include <libusockets.h>

namespace rpc {
	std::atomic<uint32_t> RpcNetworkingContext::Node::atomicNodeIds = 1;
	
	RpcNetworkingContext::RpcNetworkingContext(
			std::function<void(networking::Socket*, RpcNetworkingContext*,
				bool, char*, int)> onOpenSocket,
			std::function<void(networking::Socket*, RpcNetworkingContext*,
				int, void*)> onCloseSocket,
			const char* keyFileName, const char* certFileName,
			const char* caFileName, const char* passphrase) {
		this->onOpenSocket = onOpenSocket;
		this->onCloseSocket = onCloseSocket;
		running = false;
		loop = networking::Loop::Make();
		context = networking::Context::Make(loop,
				[](networking::Socket*socket, int isClient, char* ip,
					int ipLength) {
					RpcNetworkingContext* context = (RpcNetworkingContext*)
						socket->context->userData;
					if(context)
						if(context->onOpenSocket)
							return context->onOpenSocket(socket, context,
									isClient, ip, ipLength);
				},
				[](networking::Socket* socket, int ec, void* edata) {
					RpcNetworkingContext* context = (RpcNetworkingContext*)
						socket->context->userData;
					if(context)
						if(context->onCloseSocket)
							return context->onCloseSocket(socket, context, ec,
									edata);
				},
				[](networking::Buffer& buffer, networking::Socket* socket) {
					RpcNetworkingContext* context = (RpcNetworkingContext*)
						socket->context->userData;
					if(context)
						return context->OnMessage(buffer, socket);
				},
				keyFileName, certFileName, caFileName, passphrase);
		context->userData = this;
		runningThread = std::thread([](RpcNetworkingContext* context) {
				context->Run();
			}, this);
	}
	
	RpcNetworkingContext::~RpcNetworkingContext() {
		WaitEnd();
	}
	
	void RpcNetworkingContext::Run() {
		running = true;
		loop->Run();
		running = false;
	}
	
	RpcNetworkingContext*& RpcNetworkingContext::Singleton() {
		static RpcNetworkingContext* context = NULL;
		return context;
	}
	
	void RpcNetworkingContext::InitSingleton(RpcNetworkingContext* context) {
		Singleton() = context;
	}
	
	
	
	void RpcNetworkingContext::Listen(const char* ip, int port) {
		context->StartListening(ip, port);
	}
	
	void RpcNetworkingContext::WaitEnd() {
		while(running) {
			std::this_thread::yield();
		}
	}
	
	
	
	void RpcNetworkingContext::Call(uint32_t nodeId,
			networking::Buffer&& message) {
		networking::Event* event = networking::Event::Allocate();
		event->after = RpcNetworkingContext::ExecuteSendEvent;
		event->buffer_or_ip = std::move(std::move(message));
		event->data32 = nodeId;
		event->type = networking::Event::Type::CUSTOM;
		loop->PushEvent(event);
	}
	
	void RpcNetworkingContext::ExecuteSendEvent(networking::Event& event) {
		Node* node = Singleton()->InternalGetNode(event.data32);
		RpcNetworkingContext* context = Singleton();
		if(!node->socket) {
			if(node->connecting) {

			} else if(node->ip == "") {
				throw "Node has no IP address.";
			} else {
				node->socket = context->context->InternalConnect(
						node->ip.c_str(), node->port);
			}
		}
		if(node->socket) {
			node->socket->InternalSend(event.buffer_or_ip);
		} else {
			networking::Event* pass = networking::Event::Allocate();
			pass->MoveFrom(std::move(event));;
			context->loop->DeferEvent(5, pass);
		}
	}
	
	
	
	void RpcNetworkingContext::OnMessage(networking::Buffer& buffer,
			networking::Socket* socket) {
		serialization::Reader reader(buffer);
		FunctionRegistry::Call(reader);
	}
	
	void RpcNetworkingContext::OnOpenSocket(networking::Socket* socket,
			bool isClient, char* ip, int ipLength) {
		std::string ipString = TranslateIp(ip, ipLength);
		auto it = ipNodes.find(ipString);
		Node* node = NULL;
		if(it == ipNodes.end()) {
			node = new Node();
			node->nodeId = ++Node::atomicNodeIds;
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
	
	void RpcNetworkingContext::OnCloseSocket(networking::Socket* socket,
			int ec, void* edata) {
		auto it = socketNodes.find(socket);
		if(it != socketNodes.end()) {
			it->second->socket = NULL;
			socketNodes.erase(it);
		}
	}



	std::string RpcNetworkingContext::TranslateIp(const char* ip,
			int ipLength) {
		std::string str;
		std::stringstream ss;
		if(ipLength == 4) {
			ss << std::dec;
			for(int i=0; i<ipLength; ++i) {
				if(i)
					ss << '.';
				ss << (unsigned)ip[i];
			}
		} else {
			ss << std::hex;
			for(int i=0; i<ipLength; i+=2) {
				if(i)
					ss << '.';
				ss << (((unsigned)ip[i]) | (((unsigned)ip[i+1])<<8));
			}
		}
		return str;
	}
}

