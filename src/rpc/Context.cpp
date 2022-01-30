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

#include "Context.hpp"
#include "../serialization/serializator.hpp"

#include <sstream>
#include <thread>
#include <chrono>

#include <libusockets.h>

namespace rpc {
	
	Context::Context(
			std::function<void(net::Socket*, Context*,
				bool, std::string)> onOpenSocket,
			std::function<void(net::Socket*, Context*,
				int, void*)> onCloseSocket,
			const char* keyFileName, const char* certFileName,
			const char* caFileName, const char* passphrase) {
		this->onOpenSocket = onOpenSocket;
		this->onCloseSocket = onCloseSocket;
		running = false;
		loop = net::Loop::Make();
		context = net::Context::Make(loop,
				[](net::Socket*socket, bool isClient, std::string ip) {
					Context* context = (Context*)
						socket->context->userData;
					if(context)
						if(context->onOpenSocket)
							return context->onOpenSocket(socket, context,
									isClient, ip);
				},
				[](net::Socket* socket, int ec, void* edata) {
					Context* context = (Context*)
						socket->context->userData;
					if(context)
						if(context->onCloseSocket)
							return context->onCloseSocket(socket, context, ec,
									edata);
				},
				[](net::Buffer& buffer, net::Socket* socket) {
					Context* context = (Context*)
						socket->context->userData;
					if(context)
						return context->OnMessage(buffer, socket);
				},
				keyFileName, certFileName, caFileName, passphrase);
		context->userData = this;
		runningThread = std::thread([](Context* context) {
				context->Run();
			}, this);
	}
	
	Context::~Context() {
		WaitEnd();
	}
	
	void Context::Run() {
		running = true;
		loop->Run();
		running = false;
	}
	
	Context*& Context::Singleton() {
		static Context* context = NULL;
		return context;
	}
	
	void Context::InitSingleton(Context* context) {
		Singleton() = context;
	}
	
	
	
	void Context::Listen(const char* ip, int port) {
		context->StartListening(ip, port);
	}
	
	void Context::WaitEnd() {
		while(running) {
			std::this_thread::yield();
		}
	}
	
	
	
	void Context::Call(uint32_t nodeId,
			net::Buffer&& message) {
		net::Event* event = net::Event::Allocate();
		event->after = Context::ExecuteSendEvent;
		event->buffer_or_ip = std::move(std::move(message));
		event->data32 = nodeId;
		event->type = net::Event::Type::CUSTOM;
		loop->PushEvent(event);
	}
	
	void Context::ExecuteSendEvent(net::Event& event) {
		Node* node = Singleton()->nodeRepository.InternalGetNode(event.data32);
		Context* context = Singleton();
		if(node) {
			if(!node->socket) {
				if(node->connecting) {

				} else if(node->ip == "") {
					throw "Node has no IP address.";
				} else {
					node->socket = context->context->InternalConnect(
							node->ip.c_str(), node->port);
				}
			}
		} else {
			context->NodeNotFound(event.data32);
		}
		if(node == NULL || (node!=NULL && node->socket==NULL)) {
			net::Event* pass = net::Event::Allocate();
			pass->MoveFrom(std::move(event));
			context->loop->DeferEvent(5, pass);
		} else {
			node->socket->InternalSend(event.buffer_or_ip);
		}
	}
	
	void Context::NodeNotFound(uint32_t nodeId) {
			// TODO
		net::Event* event = net::Event::Allocate();
		event->type = net::Event::ALLCAST_CONTEXT;
		event->context = context;
		event->buffer_or_ip = net::Buffer(); // TODO make this buffer correct, use some predefined (built in) RPC funcion ID
		loop->PushEvent(event);
	}
	
	
	
	void Context::OnMessage(net::Buffer& buffer,
			net::Socket* socket) {
		serialization::Reader reader(buffer);
		FunctionRegistry::Call(reader);
	}
	
	void Context::OnOpenSocket(net::Socket* socket, bool isClient,
			std::string ip) {
		socket->userData = NULL;
		nodeRepository.OnOpenSocket(socket, isClient);
	}
	
	void Context::OnCloseSocket(net::Socket* socket, int ec, void* edata) {
		nodeRepository.OnCloseSocket(socket);
	}
}

