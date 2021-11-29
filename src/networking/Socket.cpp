/*
 *  This file is part of DORPC. Please see README for details.
 *  Copyright (C) 2021 Marek Zalewski aka Drwalin
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

#include <cinttypes>

#include <msgpack.h>
#include <libusockets.h>

#include "Loop.hpp"
#include "Context.hpp"
#include "../Cluster.hpp"
#include "Event.hpp"

#include "Socket.hpp"

void Socket::Init(struct us_socket_t* socket, int ssl) {
	this->socket = socket;
	this->ssl = ssl;
	context = (Context*)us_socket_context_ext(ssl,
			us_socket_context(ssl, socket));
	loop = context->loop;
	
	buffer = NULL;
}

void Socket::Destroy() {
	us_socket_shutdown(ssl, socket);
}

void Socket::OnOpen(char* ip, int ipLength) {
	
}

void Socket::OnEnd() {
	
}

void Socket::OnClose(int code, void* reason) {
	
}

void Socket::OnTimeout() {
	
}

void Socket::OnWritable() {
	
}

void Socket::OnData(uint8_t* data, int length) {
	while(length) {
		if(buffer == NULL)
			buffer = Buffer::Allocate();
		int size = buffer->Size();
		int request_size = 0;

		while(buffer->Size() < 4) {
			buffer->Write(data, 1);
			++data;
			--length;
			if(length == 0)
				return;
		}

		if(size >= 4) {
			uint8_t* p = (uint8_t*)buffer->Data();
			request_size = (int(p[0])) | (int(p[1]) << 8) | (int(p[2]) << 16)
				| (int(p[3]) << 24);
		}

		int to_put = std::min(length, request_size - size);
		buffer->Write(data, to_put);
		size += to_put;
		length -= to_put;
		
		if(size == request_size) {
			Cluster::Singleton()->Execute(this, buffer);
			buffer = NULL;
		}
	}
}

void Socket::Send(Buffer* sendBuffer) {
	loop->PushEvent(
			Event {
				.type=Event::SEND,
				.buffer=sendBuffer,
				.socket=this
			});
}

