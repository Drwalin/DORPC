/*
 *  This file is part of DORPC. Please see README for details.
 *  Copyright (C) 2021-2022 Marek Zalewski aka Drwalin
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

#include <algorithm>

#include <cinttypes>
#include <cstring>

#include <libusockets.h>

#include "Loop.hpp"
#include "Context.hpp"
#include "Event.hpp"

#include "Socket.hpp"

namespace net {
	void Socket::Init(struct us_socket_t* socket, int ssl) {
		this->socket = socket;
		this->ssl = ssl;
		context = (Context*)us_socket_context_ext(ssl,
				us_socket_context(ssl, socket));
		loop = context->loop;
	}

	void Socket::Destroy() {
		us_socket_shutdown(ssl, socket);
	}

	void Socket::OnOpen(char* ip, int ipLength) {
		remoteIp = new std::string(TranslateIp(ip, ipLength));
		context->sockets->insert(this);
		bytes_to_receive = 0;
		received_bytes_of_size = 0;
	}

	void Socket::OnEnd() {
		buffer.Destroy();
		context->sockets->erase(this);
	}

	void Socket::OnClose(int code, void* reason) {
		if(remoteIp)
			delete remoteIp;
		remoteIp = NULL;
		buffer.Destroy();
		context->sockets->erase(this);
	}

	void Socket::OnTimeout() {
		// TODO
	}

	void Socket::OnWritable() {
		// TODO
	}

	void Socket::OnData(uint8_t* data, int length) {
		while(length) {
			if(received_bytes_of_size < 4) {
				int bytes_to_copy = std::min(4-received_bytes_of_size, length);
				memcpy(received_size+received_bytes_of_size, data,
						bytes_to_copy);
				length -= bytes_to_copy;
				data += bytes_to_copy;
				received_bytes_of_size += bytes_to_copy;
				if(received_bytes_of_size == 4) {
					bytes_to_receive =
						(int(received_size[0]))
						| (int(received_size[1]) << 8)
						| (int(received_size[2]) << 16)
						| (int(received_size[3]) << 24);
				}
			} else {
				int32_t bytes_to_copy = std::min(bytes_to_receive, length);
				buffer.Write(data, bytes_to_copy);
				data += bytes_to_copy;
				length -= bytes_to_copy;
				bytes_to_receive -= bytes_to_copy;
				if(bytes_to_receive == 0) {
					if(onReceiveMessage)
						(*onReceiveMessage)(buffer, this);
					buffer.Clear();
				}
			}
		}
	}

	void Socket::Send(Buffer& sendBuffer) {
		Event* event = Event::Allocate();
		event->after = NULL;
		event->buffer_or_ip = std::move(sendBuffer);
		event->socket = this;
		event->listenSocket = NULL;
		event->type = Event::SOCKET_SEND;
		if(loop == Loop::ThisThreadLoop())
			event->Run();
		else
			loop->PushEvent(event);
	}

	void Socket::InternalSend(Buffer& buffer) {
		int32_t length = buffer.Size();
		uint8_t b[4];
		b[0] = (length)&0xFF;
		b[1] = (length>>8)&0xFF;
		b[2] = (length>>16)&0xFF;
		b[3] = (length>>24)&0xFF;
		us_socket_write(ssl, socket, (char*)b, 4, length);
		us_socket_write(ssl, socket, (char*)buffer.Data(), length, 0);
	}

	void Socket::InternalClose() {
		us_socket_close(ssl, socket, 0, NULL);
	}
}

