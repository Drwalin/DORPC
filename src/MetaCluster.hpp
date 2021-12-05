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

#ifndef DORPC_META_CLUSTER_HPP
#define DORPC_META_CLUSTER_HPP

#include <cinttypes>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class MetaCluster {
public:
	
	MetaCluster();
	~MetaCluster();
	
	void SetId(uint64_t id);
	void SetEndpoint(const std::string& ip, int port);
	
	void RegisterPointerRange(uint64_t begin);
	void SetSocket(struct Socket* socket);
	
	
	
	
private:
	
	uint64_t id;
	std::unordered_set<uint64_t> pointersRangeBegin;
	
	std::string ip;
	int port;
	
	struct Socket* socket;
};

#endif

