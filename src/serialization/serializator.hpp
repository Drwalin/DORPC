/*
 *  This file is part of DORPC. Please see README for details.
 *  Copyright (C) 2021-2022 Marek Zalewski aka Drwalin
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

#ifndef DORPC_SERIALIZATION_SERIALIZATOR_HPP
#define DORPC_SERIALIZATION_SERIALIZATOR_HPP

#include <vector>
#include <string>
#include <string_view>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <cstring>

#include "networking/Buffer.hpp"

namespace serialization {
	class Writer {
	public:
		
		inline void SetBuffer(networking::Buffer& buffer) {
			this->buffer = std::move(buffer);
		}
		
		inline networking::Buffer& GetBuffer() {
			return buffer;
		}
		
		inline Writer& operator<<(int8_t v) { return WriteWebOrder(v); }
		inline Writer& operator<<(int16_t v) { return WriteWebOrder(v); }
		inline Writer& operator<<(int32_t v) { return WriteWebOrder(v); }
		inline Writer& operator<<(int64_t v) { return WriteWebOrder(v); }
		inline Writer& operator<<(uint8_t v) { return WriteWebOrder(v); }
		inline Writer& operator<<(uint16_t v) { return WriteWebOrder(v); }
		inline Writer& operator<<(uint32_t v) { return WriteWebOrder(v); }
		inline Writer& operator<<(uint64_t v) { return WriteWebOrder(v); }
		
		inline Writer& operator<<(float v) {
			return WriteWebOrder(*(uint32_t*)&v);
		}
		
		inline Writer& operator<<(double v) {
			return WriteWebOrder(*(uint64_t*)&v);
		}
		
		template<typename T>
		inline Writer& operator<<(const std::vector<T>& v) {
			*this << (int32_t)v.size();
			for(int i=0; i<v.size(); ++i)
				*this << v[i];
			return *this;
		}
		
		inline Writer& operator<<(const char* str) {
			int32_t len = strlen(str);
			*this << len;
			buffer.Write(str, len);
			return *this;
		}
		
		inline Writer& operator<<(std::string_view v) {
			*this << (int32_t)v.size();
			buffer.Write(v.data(), v.size());
			return *this;
		}
		
		inline Writer& operator<<(const std::string& v) {
			*this << (int32_t)v.size();
			buffer.Write(v.data(), v.size());
			return *this;
		}
		
		inline Writer& operator<<(const std::vector<uint8_t>& v) {
			*this << (int32_t)v.size();
			buffer.Write(v.data(), v.size());
			return *this;
		}
		
		inline Writer& operator<<(const std::vector<int8_t>& v) {
			*this << (int32_t)v.size();
			buffer.Write(v.data(), v.size());
			return *this;
		}
		
		template<typename T>
		inline Writer& operator<<(const std::set<T>& v) {
			*this << (int32_t)v.size();
			for(const auto& e : v)
				*this << e;
			return *this;
		}
		
		template<typename T>
		inline Writer& operator<<(const std::unordered_set<T>& v) {
			*this << (int32_t)v.size();
			for(const auto& e : v)
				*this << e;
			return *this;
		}
		
		template<typename K, typename V>
		inline Writer& operator<<(const std::map<K, V>& v) {
			*this << (int32_t)v.size();
			for(const auto& e : v)
				(*this << e.first) << e.second;
			return *this;
		}
		
		template<typename K, typename V>
		inline Writer& operator<<(const std::unordered_map<K, V>& v) {
			*this << (int32_t)v.size();
			for(const auto& e : v)
				(*this << e.first) << e.second;
			return *this;
		}
		
		inline Writer& operator<<(const networking::Buffer& buffer) {
			this->buffer.Write(buffer.Data(), buffer.Size());
			return *this;
		}
		
	private:
		
		template<typename T>
		inline Writer& WriteWebOrder(T v) {
			for(int i=0; i<sizeof(T)*8; i+=8) {
				buffer.Write((uint8_t)(((uint64_t)(v>>i))&0xFF));
			}
			return *this;
		}
		
		networking::Buffer buffer;
	};
	
	
	
	
	
	class Reader {
	public:
		
		inline Reader(networking::Buffer& buffer) : buffer(buffer), read(0) {
		}
		
		inline Reader& operator>>(int8_t v) { return ReadWebOrder(v); }
		inline Reader& operator>>(int16_t v) { return ReadWebOrder(v); }
		inline Reader& operator>>(int32_t v) { return ReadWebOrder(v); }
		inline Reader& operator>>(int64_t v) { return ReadWebOrder(v); }
		inline Reader& operator>>(uint8_t v) { return ReadWebOrder(v); }
		inline Reader& operator>>(uint16_t v) { return ReadWebOrder(v); }
		inline Reader& operator>>(uint32_t v) { return ReadWebOrder(v); }
		inline Reader& operator>>(uint64_t v) { return ReadWebOrder(v); }
		
		inline Reader& operator>>(float v) {
			return ReadWebOrder(*(uint32_t*)&v);
		}
		
		inline Reader& operator>>(double v) {
			return ReadWebOrder(*(uint64_t*)&v);
		}
		
		template<typename T>
		inline Reader& operator>>(std::vector<T>& v) {
			int32_t size=0;
			*this >> size;
			v.resize(size);
			for(int i=0; i<v.size(); ++i)
				*this >> v[i];
			return *this;
		}
		
		inline Reader& operator>>(std::string_view& v) {
			int32_t size=0;
			*this >> size;
			if(read+size <= buffer.Size()) {
				v = std::string_view((const char*)buffer.Data()+read, size);
				read += size;
			} else {
				read = buffer.Size();
			}
			return *this;
		}
		
		inline Reader& operator>>(std::string& v) {
			std::string_view view;
			*this >> view;
			v = view;
			return *this;
		}
		
		inline Reader& operator>>(std::vector<uint8_t>& v) {
			std::string_view view;
			*this >> view;
			v.clear();
			v.insert(v.begin(), (const uint8_t*)view.data(),
					(const uint8_t*)view.end());
			return *this;
		}
		
		inline Reader& operator>>(std::vector<int8_t>& v) {
			std::string_view view;
			*this >> view;
			v.clear();
			v.insert(v.begin(), view.data(), view.end());
			return *this;
		}
		
		template<typename T>
		inline Reader& operator>>(std::set<T>& v) {
			int32_t size=0;
			*this >> size;
			for(int i=0; i<v.size(); ++i) {
				T _v;
				*this >> _v;
				v.insert(_v);
			}
			return *this;
		}
		
		template<typename T>
		inline Reader& operator>>(std::unordered_set<T>& v) {
			int32_t size=0;
			*this >> size;
			for(int i=0; i<v.size(); ++i) {
				T _v;
				*this >> _v;
				v.insert(_v);
			}
			return *this;
		}
		
		template<typename K, typename V>
		inline Reader& operator>>(std::map<K, V>& v) {
			int32_t size=0;
			*this >> size;
			for(int i=0; i<v.size(); ++i) {
				K k;
				*this >> k;
				*this >> v[k];
			}
			return *this;
		}
		
		template<typename K, typename V>
		inline Reader& operator>>(std::unordered_map<K, V>& v) {
			int32_t size=0;
			*this >> size;
			for(int i=0; i<v.size(); ++i) {
				K k;
				*this >> k;
				*this >> v[k];
			}
			return *this;
		}
		
		inline Reader& operator>>(networking::Buffer& buffer) {
			std::string_view view;
			*this >> view;
			buffer.Clear();
			buffer.Write(view.data(), view.size());
			return *this;
		}
		
	private:
		
		template<typename T>
		inline Reader& ReadWebOrder(T v) {
			v = 0;
			if(read+sizeof(T) <= buffer.Size()) {
				for(int i=0; i<sizeof(T)*8; i+=8) {
					v |= ((T)buffer[read++]) << i;
				}
			}
			return *this;
		}
		
		networking::Buffer& buffer;
		int32_t read;
	};
}

#endif

