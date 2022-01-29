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
	
	RpcNetworkingContext::RpcNetworkingContext(
			std::function<void(net::Socket*, RpcNetworkingContext*,
				bool, char*, int)> onOpenSocket,
			std::function<void(net::Socket*, RpcNetworkingContext*,
				int, void*)> onCloseSocket,
			const char* keyFileName, const char* certFileName,
			const char* caFileName, const char* passphrase) {
		this->onOpenSocket = onOpenSocket;
		this->onCloseSocket = onCloseSocket;
		running = false;
		loop = net::Loop::Make();
		context = net::Context::Make(loop,
				[](net::Socket*socket, int isClient, char* ip,
					int ipLength) {
					RpcNetworkingContext* context = (RpcNetworkingContext*)
						socket->context->userData;
					if(context)
						if(context->onOpenSocket)
							return context->onOpenSocket(socket, context,
									isClient, ip, ipLength);
				},
				[](net::Socket* socket, int ec, void* edata) {
					RpcNetworkingContext* context = (RpcNetworkingContext*)
						socket->context->userData;
					if(context)
						if(context->onCloseSocket)
							return context->onCloseSocket(socket, context, ec,
									edata);
				},
				[](net::Buffer& buffer, net::Socket* socket) {
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
			net::Buffer&& message) {
		net::Event* event = net::Event::Allocate();
		event->after = RpcNetworkingContext::ExecuteSendEvent;
		event->buffer_or_ip = std::move(std::move(message));
		event->data32 = nodeId;
		event->type = net::Event::Type::CUSTOM;
		loop->PushEvent(event);
	}
	
	void RpcNetworkingContext::ExecuteSendEvent(net::Event& event) {
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
			net::Event* pass = net::Event::Allocate();
			pass->MoveFrom(std::move(event));
			context->loop->DeferEvent(5, pass);
		}
	}
	
	
	
	void RpcNetworkingContext::OnMessage(net::Buffer& buffer,
			net::Socket* socket) {
		serialization::Reader reader(buffer);
		FunctionRegistry::Call(reader);
	}
	
	void RpcNetworkingContext::OnOpenSocket(net::Socket* socket,
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
	
	void RpcNetworkingContext::OnCloseSocket(net::Socket* socket,
			int ec, void* edata) {
		auto it = socketNodes.find(socket);
		if(it != socketNodes.end()) {
			it->second->socket = NULL;
			socketNodes.erase(it);
		}
	}
}

