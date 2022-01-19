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

#ifndef DORPC_METHOD_REPOSITORY_HPP
#define DORPC_METHOD_REPOSITORY_HPP

#include <cinttypes>
#include <unordered_map>
#include <map>
#include <string>

#include "Method.hpp"

class NameRepository {
public:
	
	NameRepository() = default;
	~NameRepository() = default;
	
	void Destroy();
	void Clear();
	
	void Add(MethodBase* method);
	void Update(MethodBase* method);
	
	inline MethodBase* Find(uint64_t id) const;
	inline uint64_t Find(void* ptr) const;
	
private:
	
	std::unordered_map<std::string, uint64_t> methodNameId;
	std::unordered_map<uint64_t, MethodBase*> methodIdPtr;
	std::unordered_map<void*, uint64_t> methodPtrId;
};

class GeneralMethodRepository {
public:
	
	~GeneralMethodRepository();
	
	template<typename T, typename MethodType>
	inline void Add(MethodType method, const std::string& name);
	template<typename MethodType>
	inline void AddFunction(MethodType method, const std::string& name);
	inline void Update(MethodBase* method) { repository.Update(method); }
	
	inline MethodBase* Find(uint64_t id) const { return repository.Find(id); }
	inline uint64_t Find(void* ptr) { return repository.Find(ptr); }
	
	inline static GeneralMethodRepository* Singleton() {
		static GeneralMethodRepository singleton;
		return &singleton;
	}
	
	static void UpdateFrom(const std::map<std::string, uint64_t>& names);
	
private:
	
	GeneralMethodRepository() : functionIdCounter(0) {}
	
	NameRepository repository;
	uint64_t functionIdCounter;
};

template<typename T>
class MethodRepository {
public:
	
	MethodRepository() = default;

	inline void Add(MethodBase* method) { repository.Add(method); }
	inline void Update(MethodBase* method) { repository.Update(method); }
	inline void Clear() { repository.Clear(); }
	
	inline MethodBase* Find(uint64_t id) const { return repository.Find(id); }
	inline uint64_t Find(void* ptr) { return repository.Find(ptr); }
	
	inline static MethodRepository<T>* Singleton() {
		static MethodRepository<T> singleton;
		return &singleton;
	}
	
private:
	
	~MethodRepository() = default;
	
	NameRepository repository;
};

class StaticFunctionRepository {
public:
	
	StaticFunctionRepository() = default;

	inline void Add(MethodBase* method) { repository.Add(method); }
	inline void Update(MethodBase* method) { repository.Update(method); }
	inline void Clear() { repository.Clear(); }
	
	inline MethodBase* Find(uint64_t id) const { return repository.Find(id); }
	inline uint64_t Find(void* ptr) { return repository.Find(ptr); }
	
	inline static StaticFunctionRepository* Singleton() {
		static StaticFunctionRepository singleton;
		return &singleton;
	}
	
private:
	
	~StaticFunctionRepository() = default;
	
	NameRepository repository;
};

#include "MethodRepository.inl.hpp"

#define REGISTER_METHOD(CLASS, METHOD) \
	( \
		GeneralMethodRepository::Singleton() \
			->Add<CLASS, decltype(CLASS::METHOD)> \
			(CLASS::&METHOD, CLASS##"::"##METHOD) \
	)

#define REGISTER_STATIC_FUNCTION(CLASS, METHOD) \
	( \
		GeneralMethodRepository::Singleton() \
			->Add<CLASS, decltype(CLASS::METHOD)> \
			(CLASS::&METHOD, CLASS##"::"##METHOD) \
	)

#endif

