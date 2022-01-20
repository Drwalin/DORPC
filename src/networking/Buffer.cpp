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

#include "Buffer.hpp"

#include <mpmc_pool.hpp>

namespace impl {
	concurrent::mpmc::pool<Buffer::Vector> bufferPool;
}

Buffer::Buffer() {
	buffer = NULL;
}

Buffer::Buffer(Buffer&& other) {
	Free(buffer);
	buffer = other.buffer.load();
	other.buffer = NULL;
}

Buffer::~Buffer() {
	Free(buffer);
}

Buffer& Buffer::operator=(Buffer&& other) {
	Free(buffer);
	buffer = other.buffer.load();
	other.buffer = NULL;
	return *this;
}

Buffer::Vector* Buffer::Allocate() {
	return impl::bufferPool.acquire();
}

void Buffer::Free(Buffer::Vector* buffer) {
	if(buffer)
		impl::bufferPool.release(buffer);
}

