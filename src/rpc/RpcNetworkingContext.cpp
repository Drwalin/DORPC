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

namespace rpc {
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

	}
	
	void RpcNetworkingContext::ExecuteSendEvent(networking::Event* event) {
		std::shared_ptr<Node> node = Singleton()->InternalGetNode(
				(uint32_t)(uint64_t)event->socket);
		node.
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
		if(it == ipNodes.end()) {
			// TODO
		} else {
			
		}
	}
	
	void RpcNetworkingContext::OnCloseSocket(networking::Socket* socket,
			int ec, void* edata) {
		auto it = socketNodes.find(socket);
		if(it != socketNodes.end()) {
			uint32_t nodeId = it->second;
			// TODO
			
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

