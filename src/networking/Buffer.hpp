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

#ifndef DORPC_NETWORKING_BUFFER_HPP
#define DORPC_NETWORKING_BUFFER_HPP

#include <vector>
#include <atomic>

#include <mpmc_pool.hpp>

namespace networking {
	struct Buffer {

		class Vector : public concurrent::node<Vector> {
		public:
			std::vector<uint8_t> vector;
			inline void clear() {vector.clear();}
			inline int32_t size() {return vector.size();}
			inline uint8_t& operator[](int32_t id) {return vector[id];}
			inline void resize(int32_t size) {vector.resize(size);}
			inline void append(const void* buffer, int32_t bytes) {
				vector.insert(vector.end(), (const uint8_t*)buffer,
						(const uint8_t*)buffer+bytes);
			}
			inline uint8_t* data() {return vector.data();}
		};

		union {
			std::atomic<Buffer::Vector*> buffer;
			std::atomic<uint64_t> __atomic;
		};

		Buffer(const Buffer&) = delete;
		Buffer(Buffer&) = delete;
		Buffer(Buffer&& other);
		Buffer();
		~Buffer();

		Buffer& operator=(const Buffer&) = delete;
		Buffer& operator=(Buffer&) = delete;
		Buffer& operator=(Buffer&& other);

		inline void Clear() {
			if(buffer)
				buffer.load()->clear();
		}

		inline void Destroy() {
			Free(buffer);
		}

		inline void Assure() {
			if(buffer == NULL)
				buffer = Allocate();
		}

		inline void Write(uint8_t byte) {
			Assure();
			buffer.load()->append(&byte, 1);
		}

		inline void Write(const void* data, int32_t size) {
			if(data && size) {
				Assure();
				buffer.load()->append(data, size);
			}
		}

		inline int32_t Size() const {
			if(buffer == NULL)
				return 0;
			return buffer.load()->size();
		}
		inline uint8_t* Data() const {
			if(buffer.load())
				return &(buffer.load()->operator[](0));
			return NULL;
		}
		inline uint8_t* Data() {
			Assure();
			return &(buffer.load()->operator[](0));
		}
		inline uint8_t& operator[](int32_t id) {
			Assure();
			if(buffer.load()->size() <= id)
				buffer.load()->resize(buffer.load()->size()+1);
			return Data()[id];
		}

	private:
		static Vector* Allocate();
		static void Free(Vector* buffer);
	};
}

namespace std {
	inline void swap(networking::Buffer& a, networking::Buffer& b) {
		networking::Buffer::Vector* tmp = a.buffer;
		a.buffer = b.buffer.load();
		b.buffer = tmp;
	}
}

#endif

