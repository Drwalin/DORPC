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
		RpcNetworkingContext(std::function<void(net::Socket*,
					RpcNetworkingContext*, bool, char*, int)> onOpenSocket,
				std::function<void(net::Socket*, RpcNetworkingContext*,
					int, void*)> onCloseSocket,
				const char* keyFileName, const char* certFileName,
				const char* caFileName, const char* passphrase);
		~RpcNetworkingContext();
		
		void Listen(const char* ip, int port);
		void WaitEnd();
		void Connect(const char* ip, int port);
		void Disconnect(const char* ip, int port);
		void Disconnect(uint32_t nodeId);
		
		void Call(uint32_t nodeId, net::Buffer&& message);
		
		static RpcNetworkingContext*& Singleton();
		static void InitSingleton(RpcNetworkingContext* context);
		
	public:
		
		static void ExecuteSendEvent(net::Event& event);
		
	private:
		
		static void InternalDisconnect(net::Event& event);
		static void InternalConnect(net::Event& event);
		
		struct Node {
			net::Socket* socket;
			uint32_t nodeId;
			std::string ip;
			int port;
			bool connecting;
		};
		
		Node* InternalGetNode(uint32_t nodeId);
		Node* InternalGetNode(const std::string& ipString, int port);
		Node* InternalGetNode(net::Socket* socket);
		void InternalSetNodeId(Node* node, uint32_t nodeId);
		void InternalSetIp(Node* node, const std::string& ipString, int port);
		void InternalSetNodeSocket(Node* node, net::Socket* socket);
		
		
		void InternalHandshakeClientSide(net::Socket* socket);
		void InternalHandshakeServerSide(net::Socket* socket);
		static void InternalHandshakeReceiveFromServer(net::Buffer& buffer);
		static void InternalHandshakeReceiveFromClient(net::Buffer& buffer);
		
		
		void Run();
		
		void OnMessage(net::Buffer& buffer,
				net::Socket* socket);
		void OnOpenSocket(net::Socket* socket, bool isClient, char* ip,
				int ipLength);
		void OnCloseSocket(net::Socket* socket, int ec, void* edata);
		
		std::function<void(net::Socket*, RpcNetworkingContext*, bool,
				char*, int)> onOpenSocket;
		std::function<void(net::Socket*, RpcNetworkingContext*, int,
				void*)> onCloseSocket;
		
		net::Loop* loop;
		net::Context* context;
		
		std::thread runningThread;
		bool running;
		std::unordered_map<uint32_t, Node*> nodes;
		std::unordered_map<std::string, Node*> ipNodes;
		std::unordered_map<net::Socket*, Node*> socketNodes;
		
		std::atomic<uint32_t> atomicNodeIds;
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

#define RPC(FUNC, ...) rpc::Call<FUNC>::Do::Run(__VA_ARGS__)

#endif

