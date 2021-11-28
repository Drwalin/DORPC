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

#pragma once

#ifndef DORPC_CLUSTER_HPP
#define DORPC_CLUSTER_HPP

#include <cinttypes>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Reference.hpp"

struct Cluster {
public:
	
	Cluster();
	~Cluster();
	
	
	void ConfigureCertificate(const std::string& certKey,
			const std::string& certFile, const std::string& certRootCA);
	
	void StartListeningTCP(const std::string& ip, int port);
	void StartListeningSSL(const std::string& ip, int port);
	
	bool ConnectInnerTCP(const std::string& ip, int port);
	bool ConnectInnerSSL(const std::string& ip, int port);
	
	bool ConnectToClusterSSL(const std::string& ip, int port);
	bool ConnectToClusterTCP(const std::string& ip, int port);
	
	
	template<class T, typename... Args>
	inline void Call(Reference<T> ref, void(T::*method)(Args...), Args... args);
	
	
	static Cluster* Singleton();
	
private:

	bool IsLocal(uint64_t referenceId);
	
private:
	
	uint64_t clusterId;
	
	std::string certKey;
	std::string certFile;
	std::string certRootCA;
	
	struct Loop* loop;
	std::vector<struct Context*> contexts;
	std::unordered_set<struct Socket*> sockets;
	
	std::unordered_map<uint64_t, void*> localObjects;
	std::unordered_map<uint64_t, Socket*> remoteObjects; 
};



template<class T, typename... Args>
inline void Cluster::Call(Reference<T> ref,
		void(T::*method)(Args...), Args... args) {
	if(IsLocal(ref.Get())) {
		
	} else {

	}
}

#endif

