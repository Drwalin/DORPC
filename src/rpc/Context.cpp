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

#include "../Debug.hpp"

namespace rpc {
	Context::Context(
			std::function<void(net::Socket*, Context*,
				bool, std::string)> onOpenSocket,
			std::function<void(net::Socket*, Context*,
				int, void*)> onCloseSocket,
			std::function<void(net::Event& event)> onFailedSend,
			const char* keyFileName, const char* certFileName,
			const char* caFileName, const char* passphrase) {
		this->onOpenSocket = onOpenSocket;
		this->onCloseSocket = onCloseSocket;
		this->onFailedSend = onFailedSend;
		running = false;
		loop = net::Loop::Make();
		context = net::Context::Make(loop, InternalOnOpenSocket,
				InternalOnCloseSocket, InternalOnMessage,
				keyFileName, certFileName, caFileName, passphrase);
		context->userData = this;
		socketIdCounter = 1;
		InitSingleton(this);
	}
	
	Context::~Context() {
		WaitEnd();
	}
	
	void Context::Run() {
		DEBUG("");
		if(running) {
			WaitEnd();
		} else {
			running = true;
			loop->Run();
			running = false;
		}
		DEBUG("Run end");
	}
	
	void Context::AsyncRun() {
		runningThread = std::thread([](Context* c) {
				if(c->IsRunning())
					return;
				c->Run();
			}, this);
	}
	
	void Context::Listen(const char* ip, int port) {
		context->StartListening(ip, port);
	}
	
	void Context::InternalListen(const char* ip, int port) {
		context->InternalStartListening(ip, port);
	}
	
	void Context::WaitEnd() {
		while(running) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
	
	
	
	Context*& Context::Singleton() {
		static Context* context = NULL;
		return context;
	}
	
	void Context::InitSingleton(Context* context) {
		Singleton() = context;
	}
	
	
	
	net::Socket* Context::InternalConnect(const char* ip, int port) {
		return context->InternalConnect(ip, port);
	}
	
	void Context::Connect(const char* ip, int port) {
		context->Connect(ip, port);
	}
	
	void Context::Disconnect(uint32_t socketId) {
		net::Event* event = net::Event::Allocate();
		event->data32 = socketId;
		event->type = net::Event::CUSTOM;
		event->after = ExecuteDisconnectEvent;
		loop->PushEvent(event);
	}
	
	
	
	void Context::Call(uint32_t socketId,
			net::Buffer&& message) {
		net::Event* event = net::Event::Allocate();
		event->buffer_or_ip = std::move(std::move(message));
		if(socketId == 0) {
			event->type = net::Event::ALLCAST_CONTEXT;
			event->context = context;
		} else {
			event->after = Context::ExecuteSendEvent;
			event->data32 = socketId;
			event->type = net::Event::Type::CUSTOM;
		}
		loop->PushEvent(event);
	}
	
	
	
	void Context::ExecuteDisconnectEvent(net::Event& event) {
		auto it = Context::Singleton()->sockets.find(event.data32);
		if(it != Context::Singleton()->sockets.end()) {
			it->second->InternalClose();
		}
	}
	
	void Context::ExecuteSendEvent(net::Event& event) {
		auto it = Singleton()->sockets.find(event.data32);
		if(it != Singleton()->sockets.end()) {
			it->second->Send(event.buffer_or_ip);
		} else if(Singleton()->onFailedSend) {
			Singleton()->onFailedSend(event);
		}
	}
	
	
	
	
	void Context::InternalOnOpenSocket(net::Socket* socket, bool isClient,
			std::string ip) {
		DEBUG("");
		Context* context = (Context*)(socket->context->userData);
		if(context) {
		DEBUG("");
			context->AssignSocketIdToSocket(socket);
		DEBUG("");
			if(context->onOpenSocket) {
		DEBUG("");
				return context->onOpenSocket(socket, context,
						isClient, ip);
			}
		}
	}
	
	void Context::InternalOnCloseSocket(net::Socket* socket, int ec,
			void* edata) {
		DEBUG("");
		Context* context = (Context*)(socket->context->userData);
		if(context) {
			if(context->onCloseSocket)
				return context->onCloseSocket(socket, context, ec,
						edata);
			context->sockets.erase(socket->userData32);
			socket->userData32 = 0;
		}
	}
	
	void Context::InternalOnMessage(net::Buffer& buffer, net::Socket* socket) {
		DEBUG("");
		Context* context = (Context*)
			socket->context->userData;
		if(context) {
			serialization::Reader reader(buffer);
			FunctionRegistry::Call(reader);
		}
	}
	
	
	
	uint32_t Context::AssignSocketIdToSocket(net::Socket* socket) {
		for(;;) {
			uint32_t id = ++socketIdCounter;
			if(id==0 || sockets.contains(id)) {
				continue;
			}
			socket->userData32 = id;
			sockets[id] = socket;
			DEBUG("Assigned: %u", id);
			return id;
		}
	}
}

