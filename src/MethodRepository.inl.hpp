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

#ifndef METHOD_REPOSITORY_INL_HPP
#define METHOD_REPOSITORY_INL_HPP

#include <cinttypes>

#include "MethodRepository.hpp"

template<typename T, typename MethodType>
inline void GeneralMethodRepository::Add(MethodType method,
		const std::string& name) {
	
	++functionIdCounter;
	uint64_t id = functionIdCounter;
	
	(new Method(method, id, name))->Add();
}

template<typename T, typename Ret, typename... Args>
Method<T, Ret, Args...>::~Method() {
	MethodRepository<T>::Singleton()->Clear();
}

template<typename T, typename Ret, typename... Args>
void Method<T, Ret, Args...>::Add() {
	GeneralMethodRepository::Singleton()->Add<T>(this);
	MethodRepository<T>::Singleton()->Add(this);
}

template<typename T, typename Ret, typename... Args>
void Method<T, Ret, Args...>::Update() {
	GeneralMethodRepository::Singleton()->Update(this);
	MethodRepository<T>::Singleton()->Update(this);
}


#endif

