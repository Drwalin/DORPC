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

#ifndef DORPC_RPC_CONTEXT_HPP
#define DORPC_RPC_CONTEXT_HPP

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
	class Context {
	public:
		Context(std::function<void(std::shared_ptr<net::Socket>,
					Context*, bool, std::string)> onOpenSocket,
				std::function<void(std::shared_ptr<net::Socket>, Context*,
					int, void*)> onCloseSocket,
				std::function<void(net::Event& event)> onFailedSend,
				const char* keyFileName, const char* certFileName,
				const char* caFileName, const char* passphrase);
		~Context();
		
		void AsyncRun();
		void Run();
		void Listen(const char* ip, int port);
		void InternalListen(const char* ip, int port);
		void WaitEnd();
		
		static Context*& Singleton();
		static void InitSingleton(Context* context);
		
		void Connect(const char* ip, int port);
		void InternalConnect(const char* ip, int port);
		void Disconnect(uint32_t socketId);
		
		void Call(uint32_t socketId, net::Buffer&& message);
		
		inline bool IsRunning() const { return running; }
		
	private:
		
		static void ExecuteDisconnectEvent(net::Event& event);
		static void ExecuteSendEvent(net::Event& event);
		
		static void InternalOnOpenSocket(std::shared_ptr<net::Socket> socket,
				bool isClient, std::string ip);
		static void InternalOnCloseSocket(std::shared_ptr<net::Socket> socket,
				int ec, void* edata);
		static void InternalOnMessage(net::Buffer& buffer,
				std::shared_ptr<net::Socket> socket);
		
		uint32_t AssignSocketIdToSocket(std::shared_ptr<net::Socket> socket);
		
		std::function<void(std::shared_ptr<net::Socket>, Context*, bool,
				std::string)> onOpenSocket;
		std::function<void(std::shared_ptr<net::Socket>, Context*, int,
				void*)> onCloseSocket;
		std::function<void(net::Event& event)> onFailedSend;
		
		std::thread runningThread;
		bool running;
		
		std::unordered_map<uint32_t, std::shared_ptr<net::Socket>> sockets;
		uint32_t socketIdCounter;
		
	public:
		
		std::shared_ptr<net::Loop> loop;
		std::shared_ptr<net::Context> context;
	};
	
	template<auto func>
	struct Call {
		template<typename T>
		struct _Do;

		template<typename Ret, typename...Args>
		struct _Do<Ret(*)(Args...)> {
			inline static bool Run(uint32_t socketId, Args... args) {
				serialization::Writer preparedArgs;
				if(rpc::FunctionRegistry::PrepareFunctionCall<func, Args...>(
							preparedArgs, args...) == false)
					return false;
				Context::Singleton()->Call(socketId,
						std::move(preparedArgs.GetBuffer()));
				return true;
			}
		};
		
		struct Do : _Do<decltype(func)> {};
	};
}

#define RPC(FUNC, ...) rpc::Call<FUNC>::Do::Run(__VA_ARGS__)

#endif

