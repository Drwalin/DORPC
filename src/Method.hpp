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

#ifndef DORPC_METHOD_HPP
#define DORPC_METHOD_HPP

#include <functional>
#include <cinttypes>
#include <string>

#include "Return.hpp"

class MethodBase {
public:
	
	virtual ~MethodBase() = default;
	
	virtual bool Execute(void* objectPtr, class Return& returnValue,
			void* args, uint32_t argsSize) const = 0;
	
	inline void* GetPtr() const { return methodPtr; }
	inline uint64_t GetId() const { return id; }
	inline const std::string& GetName() const { return name; }
	
	virtual void Add() = 0;
	virtual void Update() = 0;
	
protected:
	
	MethodBase(void* ptr, uint64_t id, const std::string& name) :
		methodPtr(ptr), id(id), name(name) {}
	
protected:
	
	void* methodPtr;
	uint64_t id;
	std::string name;
};

template<typename T, typename Ret, typename... Args>
class Method;

#include "Method.inl.hpp"

#endif

