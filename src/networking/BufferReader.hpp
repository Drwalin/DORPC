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

#ifndef DORPC_NETWORKING_BUFFER_READER_HPP
#define DORPC_NETWORKING_BUFFER_READER_HPP

#include <msgpack.h>
#include <string>

#include "Buffer.hpp"

struct BufferReader {
	
	struct Iterator {
		msgpack_object object;
		uint32_t id;
	};
	
	struct Buffer* buffer;
	msgpack_unpacked unpacked;
	std::vector<Iterator> stack;
	
	
	BufferReader(const BufferReader&) = delete;
	BufferReader(BufferReader&) = delete;
	BufferReader(BufferReader&& other) = default;
	BufferReader(struct Buffer* buffer);
	~BufferReader();
	
	void Alloc();
	void Destroy();
	
	BufferReader& operator=(const BufferReader&) = delete;
	BufferReader& operator=(BufferReader&) = delete;
	BufferReader& operator=(BufferReader&& other) = delete;
	
	inline void Write(void* data, int size) {
		msgpack_sbuffer_write(&buffer, (char*)data, size); 
	}
	
	inline bool Read(uint64_t& value);
	inline bool Read(int64_t& value);
	inline bool Read(uint32_t& value);
	inline bool Read(int32_t& value);
	inline bool Read(uint16_t& value);
	inline bool Read(int16_t& value);
	inline bool Read(uint8_t& value);
	inline bool Read(int8_t& value);
	inline bool Read(float& value);
	inline bool Read(double& value);
	inline bool Read(bool& value);
	inline bool Read(std::string_view& value);
	template<typename T>
	inline bool Read(std::vector<T>& value);
	template<typename K, typename V>
	inline bool Read(std::map<K, V>& value);
	template<typename K, typename V>
	inline bool Read(std::unordered_map<K, V>& value);
	
	template<typename T>
	inline bool Read(T& value);
	
	inline bool ReadArrayHeader(uint32_t& size);
	inline bool ReadMapHeader(uint32_t& size);
	inline bool ReadStringHeader(uint32_t& size);
	inline bool ReadRawHeader(uint32_t& size);
};



BufferReader::BufferReader(struct Buffer* buffer) {
	this->buffer = buffer;
	
	msgpack_unpacked_init(&unpacked);
	msgpack_unpack_return ret = msgpack_unpack_next(&unpacked,
			(const char*)buffer->Data(),
			buffer->Size(), NULL);
	if(ret != MSGPACK_UNPACK_SUCCESS) {
		msgpack_unpacked_destroy(&unpacked);
		return;
	}
	stack.emplace_back(unpacked.data, 0);
}

BufferReader::~BufferReader() {
	msgpack_unpacked_destroy(&unpacked);
}



#define READ_PRIMITIVE(TYPE, VIA, A, B) \
inline bool BufferReader::Read(TYPE& value) { \
	if(!stack.empty()) { \
		if(stack.back().object.type == A || stack.back().object.type == B) { \
			value = stack.back().object.via.VIA; \
			stack.pop_back(); \
			return true; \
		} \
	} \
	return false; \
}
READ_PRIMITIVE(uint64_t, u64, MSGPACK_OBJECT_POSITIVE_INTEGER,
		MSGPACK_OBJECT_POSITIVE_INTEGER);
READ_PRIMITIVE(uint32_t, u64, MSGPACK_OBJECT_POSITIVE_INTEGER,
		MSGPACK_OBJECT_POSITIVE_INTEGER);
READ_PRIMITIVE(uint16_t, u64, MSGPACK_OBJECT_POSITIVE_INTEGER,
		MSGPACK_OBJECT_POSITIVE_INTEGER);
READ_PRIMITIVE(uint8_t, u64, MSGPACK_OBJECT_POSITIVE_INTEGER,
		MSGPACK_OBJECT_POSITIVE_INTEGER);
READ_PRIMITIVE(int64_t, i64, MSGPACK_OBJECT_POSITIVE_INTEGER,
		MSGPACK_OBJECT_NEGATIVE_INTEGER);
READ_PRIMITIVE(int32_t, i64, MSGPACK_OBJECT_POSITIVE_INTEGER,
		MSGPACK_OBJECT_NEGATIVE_INTEGER);
READ_PRIMITIVE(int16_t, i64, MSGPACK_OBJECT_POSITIVE_INTEGER,
		MSGPACK_OBJECT_NEGATIVE_INTEGER);
READ_PRIMITIVE(int8_t, i64, MSGPACK_OBJECT_POSITIVE_INTEGER,
		MSGPACK_OBJECT_NEGATIVE_INTEGER);
READ_PRIMITIVE(float, f64, MSGPACK_OBJECT_FLOAT32,
		MSGPACK_OBJECT_FLOAT64);
READ_PRIMITIVE(bool, boolean, MSGPACK_OBJECT_BOOLEAN,
		MSGPACK_OBJECT_BOOLEAN);
#undef READ_PRIMITIVE

inline bool BufferReader::Read(std::string_view& value) {
	if(!stack.empty()) {
		if(stack.back().object.type == MSGPACK_OBJECT_STR ||
				stack.back().object.type == MSGPACK_OBJECT_BIN) {
			value = std::string_view(stack.back().object.via.str.ptr,
					stack.back().object.via.str.size);
			stack.pop_back();
			return true;
		}
	}
	return false;
}






#define READ_ADD(TYPE, FUNCTION) \
	inline void BufferReader::Read(struct msgpack_packer* pk, TYPE value) { \
		FUNCTION(pk, value); \
	}
READ_ADD(uint64_t, msgpack_pack_uint64);
READ_ADD(int64_t, msgpack_pack_int64);
READ_ADD(float, msgpack_pack_float);
READ_ADD(double, msgpack_pack_double);
#undef READ_ADD

inline void BufferReader::Read(std::string& value) {
}

inline void BufferReader::Read(std::string_view& value) {
}

inline void BufferReader::Read(bool& value) {
}


template<typename T, template<typename> typename container>
inline void BufferReader::Read(struct msgpack_packer* pk,
		const container<T>& value) {
	msgpack_pack_array(pk, value.size());
	for(const auto& it : value) {
		Read(pk, it);
	}
}

template<typename K, typename V,
	template<typename, typename> typename container>
inline void BufferReader::Read(struct msgpack_packer* pk,
		const container<K, V>& value) {
	msgpack_pack_map(pk, value.size());
	for(const auto& it : value) {
		Read(pk, it.first);
		Read(pk, it.second);
	}
}

template<template<typename> typename container>
inline void BufferReader::Read(struct msgpack_packer* pk,
		const container<uint8_t>& value) {
	msgpack_pack_bin_with_body(pk, value.data(), value.size());
}

#endif

