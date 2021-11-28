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

#include "MethodRepository.hpp"

void NameRepository::Destroy() {
	for(auto& it : methodIdPtr) {
		delete it.second;
	}
	methodNameId.clear();
	methodIdPtr.clear();
	methodPtrId.clear();
}

void NameRepository::Clear() {
	methodNameId.clear();
	methodIdPtr.clear();
	methodPtrId.clear();
}

void NameRepository::Add(MethodBase* method) {
	methodNameId[method->GetName()] = method->GetId();
	methodIdPtr[method->GetId()] = method;
	methodPtrId[method->GetPtr()] = method->GetId();
}

void NameRepository::Update(MethodBase* method) {
	uint64_t oldId = methodNameId[method->GetName()];
	if(methodIdPtr[oldId] == method) {
		methodIdPtr.erase(oldId);
	}
	Add(method);
}

MethodBase* NameRepository::Find(uint64_t id) const {
	auto it = methodIdPtr.find(id);
	if(it != methodIdPtr.end())
		return it->second;
	return NULL;
}

uint64_t NameRepository::Find(void* ptr) {
	auto it = methodPtrId.find(ptr);
	if(it != methodPtrId.end())
		return it->second;
	return 0;
}



GeneralMethodRepository::~GeneralMethodRepository() {
	repository.Destroy();
}

