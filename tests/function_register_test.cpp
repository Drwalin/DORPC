
#include <iostream>
#include <ostream>

namespace networking {
	class Buffer;
}
std::ostream& operator<<(std::ostream& s, networking::Buffer& buffer);

#include <rpc/FunctionRegistry.hpp>
#include <rpc/Function.hpp>

template<typename T>
std::ostream& operator<<(std::ostream& s, std::vector<T> v) {
	s << "(" << v.size() << ")[";
	for(size_t i=0; i<v.size(); ++i) {
		if(i!=0)
			s << ",\n";
		s << v[i];
	}
	s << "]\n";
	return s;
}

template<typename T>
std::ostream& operator<<(std::ostream& s, std::set<T> v) {
	s << "(" << v.size() << "){";
	int i=0;
	for(const T& e : v) {
		if(i!=0)
			s << ",\n";
		s << e;
		++i;
	}
	s << "}\n";
	return s;
}

template<typename K, typename V>
std::ostream& operator<<(std::ostream& s, std::map<K, V> v) {
	s << "(" << v.size() << "){";
	int i=0;
	for(const auto& e : v) {
		if(i!=0)
			s << ",\n";
		s << e.first << ": " << e.second;
		++i;
	}
	s << "}\n";
	return s;
}

std::ostream& operator<<(std::ostream& s, networking::Buffer& buffer) {
	s << " buffer [" << buffer.Size() << "]<<";
	for(int32_t i=0; i<buffer.Size(); ++i) {
		if(i!=0)
			s << ", ";
		s << (int)buffer[i];
	}
	s << ">>\n" << std::flush;
	return s;
}



int valid=0, invalid=0, total=0;

template<typename Ret, typename Args>
Ret Call__(rpc::FunctionBase* function, Args args) {
	serialization::Writer writer, ret;
	writer << args;
	serialization::Reader reader(writer.GetBuffer());
	function->ExecuteWithReturn(reader, ret);
	Ret r;
	serialization::Reader retReader(ret.GetBuffer());
	retReader >> r;
	return r;
}

template<typename Type, Type func, typename Args>
void Call(int testId, Args args) {
	using Ret = decltype(std::apply(func, args));
	auto a = Call__<Ret>(FUNCTION(func), args);
	auto b = std::apply(func, args);
	bool result = a == b;
	printf(" test %i ... %s\n", testId, result?"OK":"FAILED");
	fflush(stdout);
	if(result)
		++valid;
	else
		++invalid;
	++total;
}

#define CALL(I, F, ...) Call<decltype(&F), F>(I, \
		rpc::FunctionTraits<decltype(&F)>::MakeTuple(__VA_ARGS__))





int functionA(int32_t a, float b, long long c) {
	int ret = c + (int64_t)(a*b);
	return ret;
}

std::string functionB(std::string a, std::vector<uint32_t> b,
		std::vector<std::string> c) {
	for(int i : b) {
		a += ".." + c[i%c.size()];
	}
	return a;
}

int main() {
	REGISTER_FUNCTION(functionA);
	REGISTER_FUNCTION(functionB);
	
	CALL(1, functionA, 'a', 'b', 'c');
	CALL(2, functionA, 'd', 'e', 'f');
	CALL(3, functionA, 'g', 'h', 'i');
	CALL(4, functionA, 'j', 'k', 'l');
	
	CALL(5, functionB, "AA:", {1, 1, 0}, {"__0", "__1"});
	CALL(6, functionB, "_B:", {1, 1, 0}, {"__3", "__2"});
	CALL(7, functionB, "C_:", {1, 1, 0}, {"__4", "__5", "_6"});
	CALL(8, functionB, "++:", {1, 1, 0}, {"_9", "_10", "_11"});
	
	
	
	printf(" tests %i/%i ... OK\n", valid, total);
	if(invalid)
		printf(" tests %i/%i ... FAILED\n", invalid, total);
	
	return invalid;
}

