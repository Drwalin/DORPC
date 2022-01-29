
#include <iostream>
#include <ostream>

#include <rpc/FunctionRegistry.hpp>
#include <rpc/Function.hpp>
#include <rpc/RpcNetworkingContext.hpp>

int valid=0, invalid=0, total=0;

void validate(int id, bool result) {
	if(id) {
		if(result) {
			valid++;
		} else {
			invalid++;
			printf(" test %i ... FAILED\n", id);
		}
		++total;
	}
}

int functionA(int id, int res, int32_t a, float b, long long c) {
	int ret = c + (int64_t)(a*b) + 13;
	validate(id, ret == res);
	return ret;
}

std::string functionB(int id, std::string res, std::string a,
		std::vector<uint32_t> b, std::vector<std::string> c) {
	a = std::string("<begin>") + a;
	for(int i : b) {
		a += ".." + c[i%c.size()];
	}
	a += "<end>";
	validate(id, a == res);
	return a;
}

#define TEST(F, NODE, ID, R, ...) RPC(F, NODE, ID, F(NODE, 0, R, __VA_ARGS__), \
		__VA_ARGS__)

int main() {
	REGISTER_FUNCTION(functionA);
	REGISTER_FUNCTION(functionB);
	
	TEST(functionA, 1, 1, 0, 'a', 'b', 'c');
	TEST(functionA, 1, 2, 0, 'd', 'e', 'f');
	TEST(functionA, 1, 3, 0, 'g', 'h', 'i');
	TEST(functionA, 1, 4, 0, 'j', 'k', 'l');
	TEST(functionA, 1, 5, 0, 'm', 'n', 'o');
	TEST(functionA, 1, 6, 0, 'r', 'q', 'p');
	TEST(functionB, 1, 7, "", "AA:", {1, 1, 0}, {"__0", "__1"});
	TEST(functionB, 1, 8, "", "_B:", {1, 1, 0}, {"__3", "__2"});
	TEST(functionB, 1, 9, "", "C_:", {1, 2, 0}, {"__4", "__5", "_6"});
	TEST(functionB, 1, 10, "", "++:", {1, 2, 0}, {"_9", "_10", "_11"});
	
	
	
	if(invalid)
		printf(" tests %i/%i ... FAILED\n\n", invalid, total);
	else
		printf(" tests %i/%i ... OK\n\n", valid, total);
	
	return invalid;
}

