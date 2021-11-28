def Settings( **kwargs ):
  return {
    'flags': ['-x', 'c++', '-Wall', '-pedantic', '-Isrc', '-Imsgpack-c/include',
    '-std=c++20', '-I/usr/include', '-I./uSockets/src',
    '-Imsgpack-c/include/msgpack', '-IConcurrent/'],
  }
