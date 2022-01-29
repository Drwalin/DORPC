
#include <iostream>

#include <serialization/serializator.hpp>

int correct_results = 0;
int total_results = 0;

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

template<typename T, int id>
bool test_compare(const T& value) {
	serialization::Writer writer;
	writer << value;
	serialization::Reader reader(writer.GetBuffer());
	T other = T();
	reader >> other;
	++total_results;
	if(value == other) {
		++correct_results;
		return true;
	}
	printf(" Test %2i: FAILED!\n", id);
	fflush(stdout);
	std::cout << value << " != " << other << "\n\n" << std::flush;
	return false;
}

int main() {
	using S = std::string;
	using VS = std::vector<S>;
	using VI = std::vector<int32_t>;
	using SS = std::set<S>;
	using VSS = std::vector<SS>;
	using MSI = std::map<S, int>;
	
	test_compare<float,  -22>(3213.232f);
	test_compare<float,  -21>(-3213213.232f);
	test_compare<float,  -20>(321323123213.232f);
	test_compare<float,  -19>(-321343243.232f);
	test_compare<float,  -18>(34233213.232f);
	test_compare<double, -17>(-3216543635463.25436535643651321);
	test_compare<double, -16>(-564654363213.21321);
	test_compare<double, -15>(3213.21326546531);
	test_compare<double, -14>(3242343213.21321);
	test_compare<double, -13>(321424323.24324321321);
	
	test_compare<int16_t, -10>(12343);
	test_compare<int16_t, -9>(-12343);
	test_compare<int32_t, -8>(12354354);
	test_compare<int32_t, -7>(-123543543);
	test_compare<int64_t, -6>(123543543543545l);
	test_compare<int64_t, -5>(-1234654656546654l);
	test_compare<uint16_t, -3>(12354);
	test_compare<uint32_t, -2>(12354564);
	test_compare<uint64_t, -1>(1237657657675);
	
	
	test_compare<S, 1>("E fjnkdal fejklfhje ksawhf easjlf eh");
	test_compare<S, 2>("Te j dvfkl;ase sjfdsl; fjsklef;ji kl");
	test_compare<S, 3>("3");
	test_compare<S, 4>("hj389p hgji4uopse 4g89pa iop4gjiopji  iop4IOHG$IOP HWGUP H*(HTG$ H");
	
	test_compare<VS, 5>({
			"fdsa fdfd asfda ",
			"fsafd dasf ds fds",
			"",
			"fdsa fdasf das fdka fdjakl f;edka fjdksajfikajfkjlejkl hjfgio;aj",
			"fjdkasl; fdjkafl dj fkl;dasjfdkl ",
			"jdfkasl;fdjk f;djakl;"});
	test_compare<VS, 6>({
			"",
			"fdsa"});
	test_compare<VS, 7>({});
	
	test_compare<SS, 8>({
			"fdsa fdfd asfda ",
			"fsafd dasf ds fds",
			"3",
			"fdsa fdasf das fdka fdjakl f;edka fjdksajfikajfkjlejkl hjfgio;aj",
			"fjdkasl; fdjkafl dj fkl;dasjfdkl ",
			"jdfkasl;fdjk f;djakl;"});
	test_compare<SS, 9>({
			"",
			"fdsa"});
	test_compare<SS, 10>({});
	
	
	test_compare<VSS, 11>({{
				"fdsa fdfd asfda ",
				"fsafd dasf ds fds",
				"",
				"fdsa fdasf das fdka fdjakl f;edka fjdksajfikajfkjlejkl hjfgio;aj",
				"fjdkasl; fdjkafl dj fkl;dasjfdkl ",
				"jdfkasl;fdjk f;djakl;"
			}, {
				"",
				"fdsa"
			}});
	
	test_compare<MSI, 12>({
			{"fdsa fdfd asfda ", 1},
			{"fsafd dasf ds fds", -2045432543},
			{"", 54325},
			{"fdsa fdasf das fdka fdjakl f;edka fjdksajfikajfkjlejkl hjfgio;aj",
			456436},
			{"fjdkasl; fdjkafl dj fkl;dasjfdkl ", 64},
			{"jdfkasl;fdjk f;djakl;", 6463}});
	
	test_compare<VI, 13>({-3213,143,43,4,5435,34,5,65,67456,74,4,243,0,0,9,7,5,
			632,-12313,-2312,-423,432454545,-54325432});
	
	if(correct_results != total_results) {
		printf(" Tests %i/%i ... FAILED\n\n", total_results-correct_results,
				total_results);
	} else {
		printf(" Tests %i/%i ... OK\n\n", correct_results, total_results);
	}
	
	return 0;
}

