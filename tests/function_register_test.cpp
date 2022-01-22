
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

template<typename Type, Type func, typename Ret, typename... Args>
Ret Call__(Args... args) {
	serialization::Writer preparedArgs, returned;
	if(rpc::FunctionRegistry::PrepareFunctionCall<Type, func, Args...>(
				preparedArgs, args...) == false)
		return Ret();
	serialization::Reader argsReader(preparedArgs.GetBuffer());
	if(rpc::FunctionRegistry::Call(argsReader, returned) == false)
		return Ret();
	
	serialization::Reader retReader(returned.GetBuffer());
	Ret r;
	retReader >> r;
	return r;
}

template<typename Type, Type func, typename... Args>
void Call(int testId, Args... args) {
	using Ret = decltype(func(args...));
	auto a = Call__<Type, func, Ret, Args...>(args...);
	auto b = func(args...);
	bool result = a == b;
	printf(" test %i ... %s\n", testId, result?"OK":"FAILED");
	fflush(stdout);
	if(result)
		++valid;
	else
		++invalid;
	++total;
}

#define CALL(I, F, a, b, c) Call<decltype(&F), F>(I, a, b, c)





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
	
	Call<decltype(&functionA), functionA, int32_t, float, long long>(1, 'a', 'b', 'c');
	Call<decltype(&functionA), functionA, int32_t, float, long long>(2, 'd', 'e', 'f');
	Call<decltype(&functionA), functionA, int32_t, float, long long>(3, 'g', 'h', 'i');
	Call<decltype(&functionA), functionA, int32_t, float, long long>(4, 'j', 'k', 'l');
	Call<decltype(&functionB), functionB, std::string, std::vector<uint32_t>, std::vector<std::string>>(5, "AA:", {1, 1, 0}, {"__0", "__1"});
	Call<decltype(&functionB), functionB, std::string, std::vector<uint32_t>, std::vector<std::string>>(6, "_B:", {1, 1, 0}, {"__3", "__2"});
	Call<decltype(&functionB), functionB, std::string, std::vector<uint32_t>, std::vector<std::string>>(7, "C_:", {1, 2, 0}, {"__4", "__5", "_6"});
	Call<decltype(&functionB), functionB, std::string, std::vector<uint32_t>, std::vector<std::string>>(8, "++:", {1, 2, 0}, {"_9", "_10", "_11"});
	
	
	
	printf(" tests %i/%i ... OK\n", valid, total);
	if(invalid)
		printf(" tests %i/%i ... FAILED\n", invalid, total);
	
	return invalid;
}

