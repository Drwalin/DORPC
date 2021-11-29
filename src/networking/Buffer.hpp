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

#ifndef DORPC_NETWORKING_BUFFER_HPP
#define DORPC_NETWORKING_BUFFER_HPP

#include <msgpack.h>
#include <vector>
#include <string>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>

#include <mpmc_pool.hpp>

struct Buffer : public concurrent::node<Buffer> {
	msgpack_sbuffer buffer;
	
	Buffer(const Buffer&) = delete;
	Buffer(Buffer&) = delete;
	Buffer(Buffer&& other) = delete;
	Buffer();
	~Buffer();
	
	void Alloc();
	void Destroy();
	
	Buffer& operator=(const Buffer&) = delete;
	Buffer& operator=(Buffer&) = delete;
	Buffer& operator=(Buffer&& other) = delete;
	
	inline void Write(void* data, int size) {
		msgpack_sbuffer_write(&buffer, (char*)data, size); 
	}
	
	inline void PackAdd(struct msgpack_packer* pk, uint64_t value);
	inline void PackAdd(struct msgpack_packer* pk, int64_t value);
	inline void PackAdd(struct msgpack_packer* pk, float value);
	inline void PackAdd(struct msgpack_packer* pk, double value);
	inline void PackAdd(struct msgpack_packer* pk, bool value);
	inline void PackAdd(struct msgpack_packer* pk, const std::string& value);
	inline void PackAdd(struct msgpack_packer* pk, std::string_view value);
	template<typename T, template<typename> typename container>
	inline void PackAdd(struct msgpack_packer* pk,
			const container<T>& value);
	template<typename K, typename V,
		template<typename, typename> typename container>
	inline void PackAdd(struct msgpack_packer* pk,
			const container<K, V>& value);
	template<template<typename> typename container>
	inline void PackAdd(struct msgpack_packer* pk,
			const container<uint8_t>& value);
	
	template<typename T>
	inline void PackAdd(T value);
	
	inline int Size() const { return buffer.size; }
	inline void* Data() { return buffer.data; }
	
	static Buffer* Allocate();
	static void Free(Buffer* buffer);
};



template<typename T>
inline void Buffer::PackAdd(T value) {
    msgpack_packer pk;
    msgpack_packer_init(&pk, &buffer, msgpack_sbuffer_write);
	PackAdd(&pk, value);
}

#define PACK_ADD(TYPE, FUNCTION) \
	inline void Buffer::PackAdd(struct msgpack_packer* pk, TYPE value) { \
		FUNCTION(pk, value); \
	}
PACK_ADD(uint64_t, msgpack_pack_uint64);
PACK_ADD(int64_t, msgpack_pack_int64);
PACK_ADD(float, msgpack_pack_float);
PACK_ADD(double, msgpack_pack_double);
#undef PACK_ADD

inline void Buffer::PackAdd(struct msgpack_packer* pk,
		const std::string& value) {
	msgpack_pack_str_with_body(pk, value.c_str(), value.size());
}

inline void Buffer::PackAdd(struct msgpack_packer* pk,
		std::string_view value) {
	msgpack_pack_str_with_body(pk, value.data(), value.size());
}

inline void Buffer::PackAdd(struct msgpack_packer* pk, bool value) {
	if(value) msgpack_pack_true(pk);
	else msgpack_pack_false(pk);
}


template<typename T, template<typename> typename container>
inline void Buffer::PackAdd(struct msgpack_packer* pk,
		const container<T>& value) {
	msgpack_pack_array(pk, value.size());
	for(const auto& it : value) {
		PackAdd(pk, it);
	}
}

template<typename K, typename V,
	template<typename, typename> typename container>
inline void Buffer::PackAdd(struct msgpack_packer* pk,
		const container<K, V>& value) {
	msgpack_pack_map(pk, value.size());
	for(const auto& it : value) {
		PackAdd(pk, it.first);
		PackAdd(pk, it.second);
	}
}

template<template<typename> typename container>
inline void Buffer::PackAdd(struct msgpack_packer* pk,
		const container<uint8_t>& value) {
	msgpack_pack_bin_with_body(pk, value.data(), value.size());
}

#endif

