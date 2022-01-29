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
#include <tuple>
#include <utility>
#include <cstddef>
#include <cstring>

#include "../networking/Buffer.hpp"

namespace serialization {
	template<int size>
	struct UINT {
		using type = uint64_t;
	};
	
	template<>
	struct UINT<1> {
		using type = uint8_t;
	};
	
	template<>
	struct UINT<2> {
		using type = uint16_t;
	};
	
	template<>
	struct UINT<4> {
		using type = uint32_t;
	};
	
	template<>
	struct UINT<8> {
		using type = uint64_t;
	};
	
	class Writer {
	public:
		
		inline void SetBuffer(net::Buffer& buffer) {
			this->buffer = std::move(buffer);
		}
		
		inline net::Buffer& GetBuffer() {
			return buffer;
		}
		
		inline Writer& operator<<(int8_t v) = delete;
		inline Writer& operator<<(int16_t v) { return WriteWebOrder(v); }
		inline Writer& operator<<(int32_t v) { return WriteWebOrder(v); }
		inline Writer& operator<<(int64_t v) { return WriteWebOrder(v); }
		inline Writer& operator<<(long long v) { return WriteWebOrder(v); }
		inline Writer& operator<<(uint8_t v) = delete;
		inline Writer& operator<<(uint16_t v) { return WriteWebOrder(v); }
		inline Writer& operator<<(uint32_t v) { return WriteWebOrder(v); }
		inline Writer& operator<<(uint64_t v) { return WriteWebOrder(v); }
		inline Writer& operator<<(unsigned long long v) {
			return WriteWebOrder(v);
		}
		
		inline Writer& operator<<(float v) {
			return WriteWebOrder(v);
		}
		
		inline Writer& operator<<(double v) {
			return WriteWebOrder(v);
		}
		
		template<typename T>
		inline Writer& operator<<(const std::vector<T>& v) {
			*this << (int32_t)v.size();
			for(size_t i=0; i<v.size(); ++i)
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
			for(const T& e : v)
				*this << e;
			return *this;
		}
		
		template<typename T>
		inline Writer& operator<<(const std::unordered_set<T>& v) {
			*this << (int32_t)v.size();
			for(const T& e : v)
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
		
		inline Writer& operator<<(const net::Buffer& buffer) {
			this->buffer.Write(buffer.Data(), buffer.Size());
			return *this;
		}
		
	private:
		
		template<typename T>
		inline Writer& WriteWebOrder(T v) {
			union {
				T v2;
				typename UINT<sizeof(T)>::type _v;
			};
			v2 = v;
			for(size_t i=0; i<sizeof(T)*8; i+=8) {
				buffer.Write((uint8_t)((_v>>i)&0xFF));
			}
			return *this;
		}
		
		net::Buffer buffer;
	};
	
	
	
	
	
	class Reader {
	public:
		
		inline net::Buffer& GetBuffer() { return buffer; }
		inline int32_t GetReadBytes() { return read; }
		
		inline Reader(net::Buffer& buffer) : buffer(buffer), read(0) {
		}
		
		inline Reader& operator>>(int8_t &v) = delete;
		inline Reader& operator>>(int16_t &v) { return ReadWebOrder(v); }
		inline Reader& operator>>(int32_t &v) { return ReadWebOrder(v); }
		inline Reader& operator>>(int64_t &v) { return ReadWebOrder(v); }
		inline Reader& operator>>(long long &v) { return ReadWebOrder(v); }
		inline Reader& operator>>(uint8_t &v) = delete;
		inline Reader& operator>>(uint16_t &v) { return ReadWebOrder(v); }
		inline Reader& operator>>(uint32_t &v) { return ReadWebOrder(v); }
		inline Reader& operator>>(uint64_t &v) { return ReadWebOrder(v); }
		inline Reader& operator>>(unsigned long long &v) {
			return ReadWebOrder(v);
		}
		
		inline Reader& operator>>(float &v) {
			return ReadWebOrder(v);
		}
		
		inline Reader& operator>>(double &v) {
			return ReadWebOrder(v);
		}
		
		template<typename T>
		inline Reader& operator>>(std::vector<T>& v) {
			int32_t size=0;
			*this >> size;
			v.resize(size);
			for(size_t i=0; i<v.size(); ++i)
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
			for(int32_t i=0; i<size; ++i) {
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
			v.resize(size);
			for(size_t i=0; i<size; ++i) {
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
			for(int32_t i=0; i<size; ++i) {
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
			v.resize(size);
			for(size_t i=0; i<size; ++i) {
				K k;
				*this >> k;
				*this >> v[k];
			}
			return *this;
		}
		
		inline Reader& operator>>(net::Buffer& buffer) {
			std::string_view view;
			*this >> view;
			buffer.Clear();
			buffer.Write(view.data(), view.size());
			return *this;
		}
		
	private:
		
		template<typename T>
		inline Reader& ReadWebOrder(T& v) {
			union {
				T v2;
				typename UINT<sizeof(T)>::type _v;
			};
			_v = 0;
			if((int32_t)sizeof(T)+read <= buffer.Size()) {
				for(size_t i=0; i<sizeof(T)*8; i+=8) {
					_v |= ((typename UINT<sizeof(T)>::type)buffer[read++]) << i;
				}
			}
			v = v2;
			return *this;
		}
		
		net::Buffer& buffer;
		int32_t read;
	};
	
	template <typename Tuple, typename F, std::size_t ...Indices>
	void for_each_impl(Tuple&& tuple, F&& f, std::index_sequence<Indices...>) {
		using swallow = int[];
		(void)swallow{1,
			(f(std::get<Indices>(std::forward<Tuple>(tuple))), void(), int{})...
		};
	}
	
	template <typename Tuple, typename F>
	void for_each(Tuple&& tuple, F&& f) {
		constexpr std::size_t N =
			std::tuple_size<std::remove_reference_t<Tuple>>::value;
		for_each_impl(std::forward<Tuple>(tuple), std::forward<F>(f),
				std::make_index_sequence<N>{});
	}

	template<typename... Args>
	inline Writer& operator<<(serialization::Writer& writer,
			const std::tuple<Args...>& v) {
		for_each(v, [&](const auto& x){
					writer << x;
				});
		return writer;
	}

	template<typename... Args>
	inline Reader& operator>>(serialization::Reader& reader,
			std::tuple<Args...>& v) {
		for_each(v, [&](auto& x){
					reader >> x;
				});
		return reader;
	}
}

#endif

