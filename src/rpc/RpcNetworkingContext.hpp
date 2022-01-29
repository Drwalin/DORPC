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

#ifndef DORPC_RPC_RPC_NETWORKING_CONTEXT_HPP
#define DORPC_RPC_RPC_NETWORKING_CONTEXT_HPP

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
	class RpcNetworkingContext {
	public:
		
		RpcNetworkingContext(std::function<void(networking::Socket*,
					RpcNetworkingContext*, bool, char*, int)> onOpenSocket,
				std::function<void(networking::Socket*, RpcNetworkingContext*,
					int, void*)> onCloseSocket,
				const char* keyFileName, const char* certFileName,
				const char* caFileName, const char* passphrase);
		~RpcNetworkingContext();
		
		void Listen(const char* ip, int port);
		void WaitEnd();
		
		void Call(uint32_t nodeId, networking::Buffer&& message);
		
		static RpcNetworkingContext*& Singleton();
		static void InitSingleton(RpcNetworkingContext* context);
		
		static std::string TranslateIp(const char* ip, int ipLength);
		
	public:
		
		static void ExecuteSendEvent(networking::Event& event);
		
	private:
		
		struct Node {
			static std::atomic<uint32_t> atomicNodeIds;
			
			networking::Socket* socket;
			uint32_t nodeId;
			std::string ip;
			int port;
			bool connecting;
		};
		
		Node* InternalGetNode(uint32_t nodeId);
		Node* InternalGetNode(const std::string& ipString, int port);
		Node* InternalGetNode(networking::Socket* socket);
		void InternalSetNodeId(const std::string& ipString, uint32_t nodeId);
		void InternalSetNodeSocket(Node* node, networking::Socket* socket);
		
		
		void Run();
		
		void OnMessage(networking::Buffer& buffer,
				networking::Socket* socket);
		void OnOpenSocket(networking::Socket* socket, bool isClient, char* ip,
				int ipLength);
		void OnCloseSocket(networking::Socket* socket, int ec, void* edata);
		
		std::function<void(networking::Socket*, RpcNetworkingContext*, bool,
				char*, int)> onOpenSocket;
		std::function<void(networking::Socket*, RpcNetworkingContext*, int,
				void*)> onCloseSocket;
		
		networking::Loop* loop;
		networking::Context* context;
		
		std::thread runningThread;
		bool running;
		std::unordered_map<uint32_t, Node*> nodes;
		std::unordered_map<std::string, Node*> ipNodes;
		std::unordered_map<networking::Socket*, Node*> socketNodes;
	};
	
	template<auto func>
	struct Call {
		template<typename T>
		struct _Do;

		template<typename Ret, typename...Args>
		struct _Do<Ret(*)(Args...)> {
			inline static bool Run(uint32_t nodeId, Args... args) {
				serialization::Writer preparedArgs;
				if(rpc::FunctionRegistry::PrepareFunctionCall<func, Args...>(
							preparedArgs, args...) == false)
					return false;
				RpcNetworkingContext::Singleton()->Call(nodeId,
						std::move(preparedArgs.GetBuffer()));
			}
		};
		
		struct Do : _Do<decltype(func)> {};
	};
}

#endif

