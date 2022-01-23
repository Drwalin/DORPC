
#include <iostream>
#include <ostream>

#include <rpc/FunctionRegistry.hpp>
#include <rpc/Function.hpp>

int valid=0, invalid=0, total=0;


template<auto func>
struct Call {
	template<typename T>
	struct _Do;

	template<typename Ret, typename...Args>
	struct _Do<Ret(*)(Args...)> {
		inline static bool Run(Args... args) {
			serialization::Writer preparedArgs, returned;
			if(rpc::FunctionRegistry::PrepareFunctionCall<decltype(func), func, Args...>(
						preparedArgs, args...) == false)
				return false;
			serialization::Reader argsReader(preparedArgs.GetBuffer());
			return rpc::FunctionRegistry::Call(argsReader);
		}
	};
	
	struct Do : _Do<decltype(func)> {};
};

#define CALL(F, ...) Call<F>::Do::Run(__VA_ARGS__)

void validate(int id, bool result) {
	if(id) {
		if(result) {
			valid++;
			printf(" test %i ... OK\n", id);
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

#define TEST(F, ID, R, ...) CALL(F, ID, F(0, R, __VA_ARGS__), __VA_ARGS__)

int main() {
	REGISTER_FUNCTION(functionA);
	REGISTER_FUNCTION(functionB);
	
	TEST(functionA, 1, 0, 'a', 'b', 'c');
	TEST(functionA, 2, 0, 'd', 'e', 'f');
	TEST(functionA, 3, 0, 'g', 'h', 'i');
	TEST(functionA, 4, 0, 'j', 'k', 'l');
	TEST(functionA, 5, 0, 'm', 'n', 'o');
	TEST(functionA, 6, 0, 'r', 'q', 'p');
	TEST(functionB, 7, "", "AA:", {1, 1, 0}, {"__0", "__1"});
	TEST(functionB, 8, "", "_B:", {1, 1, 0}, {"__3", "__2"});
	TEST(functionB, 9, "", "C_:", {1, 2, 0}, {"__4", "__5", "_6"});
	TEST(functionB, 10, "", "++:", {1, 2, 0}, {"_9", "_10", "_11"});
	
	printf(" tests %i/%i ... OK\n", valid, total);
	if(invalid)
		printf(" tests %i/%i ... FAILED\n", invalid, total);
	
	return invalid;
}

