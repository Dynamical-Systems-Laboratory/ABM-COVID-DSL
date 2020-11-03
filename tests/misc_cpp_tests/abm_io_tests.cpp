#include "../common/test_utils.h"
#include <string>
#include <functional>
#include "../../include/io_operations/abm_io.h"

/*************************************************************** 
 * Suite for testing AbmIO class for I/O operations 
 **************************************************************/

// Aliases
template <typename T>
using vec2D = std::vector<std::vector<T>>; 

template <typename T>
using vec1D = std::vector<T>;

// Supporting functions
template <typename T>
bool read_test(const vec2D<T>, const std::string, const std::function<bool(vec2D<T>, vec2D<T>)>);

template <typename T> 
bool write_read_test(const vec2D<T>, const AbmIO&, const std::function<bool(vec2D<T>, vec2D<T>)>);

template <typename T> 
bool write_read_test(const vec1D<T>, const AbmIO&, const std::function<bool(vec2D<T>, vec2D<T>)>);

// Supporting classes

struct TestStruct
{
	friend std::ostream& operator<< (std::ostream& os, const TestStruct& test);

	bool bval = false;
	int ival = 10;
	std::string sval = {"Hello!"};
	double dval = 200.3;
};

std::ostream& operator<< (std::ostream& os, const TestStruct& test){
		os << test.bval << " " << test.ival << " " << test.sval << " " << test.dval;
		return os;
}

// Tests
bool read_test_suite();
bool custom_test_suite();
bool default_test_suite();
bool write_1D_vector_test_suite();
bool write_object_test_suite(); 

// Files required to be present
// ./test_data/r_bool.txt
// ./test_data/r_int.txt
// ./test_data/r_string.txt
// ./test_data/r_double.txt

// Files required to be deleted
// No error if not but may bias the test
// ./test_data/wr_bool.txt
// ./test_data/wr_int.txt
// ./test_data/wr_string.txt
// ./test_data/wr_double.txt
// ./test_data/wr_bool_1D.txt
// ./test_data/wr_int_1D.txt
// ./test_data/wr_string_1D.txt
// ./test_data/wr_double_1D.txt

int main()
{
	test_pass(read_test_suite(), "AbmIO read only");	
	test_pass(custom_test_suite(), "AbmIO write and read custom properties");
	test_pass(default_test_suite(), "AbmIO default properties");
	test_pass(write_1D_vector_test_suite(), "AbmIO write and read for 1D vectors");
	test_pass(write_object_test_suite(), "AbmIO for classes with ostream overload");
}

/**
 * \brief Series of read tests with various data types
 */
bool read_test_suite()
{
	using namespace std::placeholders;
	
	//
	// Types that can be compared directly/exactly
	//
	
	// bool
	std::function<bool(vec2D<bool>, vec2D<bool>)> is_equal_bool = is_equal_exact<bool>;
	vec2D<bool> bool_exp = {{0, 0}, {1, 1, 0}, {0, 1}, {0, 0}, {0, 1}};
	if (!read_test(bool_exp, "./test_data/r_bool.txt", is_equal_bool))
		return false;

	// int
 	std::function<bool(vec2D<int>, vec2D<int>)>is_equal_int = is_equal_exact<int>;
	vec2D<int> int_exp = {{2, 3, 2}, {8, 5}, {4, 1, 10, 3}};
	if (!read_test(int_exp, "./test_data/r_int.txt", is_equal_int))
		return false;
	
	// std::string
 	std::function<bool(vec2D<std::string>, vec2D<std::string>)>is_equal_str = is_equal_exact<std::string>;
	vec2D<std::string> str_exp = {{"int", "bool"}, {"float", "double", "string"}};
	if (!read_test(str_exp, "./test_data/r_string.txt", is_equal_str))
		return false;

	//
	// Types that need indirect comparison
	//	

	// double
	// We need one more argument to call is_equal_floats so to maintain function signature in 
	// the test we will wrap the call in a lambda
	double tol = 1e-5;
	// Has access to tol and calls is_equal_floats with it
	auto is_equal_wrap = [tol](vec2D<double> orig_v, vec2D<double> new_v)
		{ return is_equal_floats<double>(orig_v, new_v, tol); };	
	std::function<bool(vec2D<double>, vec2D<double>)>is_equal_double = is_equal_wrap;
	//
	vec2D<double> dbl_exp = {{0.6477, 0.2963, -1.0e5, 6.868e5, 0.6256}, {0.4509e10, 0.7447, 0.1835, 0.7802},
		   					 {0.5470, 0.1890e-7, 0.3685, 0.0811}};
	if (!read_test(dbl_exp, "./test_data/r_double.txt", is_equal_double))
		return false;
	
	// All passed
	return true;
}

/**
 * \brief Tests reading a 2D vector of type T
 * @param expected - 2D vector of expected values, each inner vector 
 * 						is a single line of file 
 * @param fname - name of the file to read from
 * @param is_equal - function object for comparing expected with 
 * 						read values of 2D vectors of type T - needs complete comparison
 * 						of the 2D vector; returns true if equal
 * @return true if is_equal returned true, false otherwise
 */
template <typename T>
bool read_test(const vec2D<T> expected, const std::string fname, 
	const std::function<bool(vec2D<T>, vec2D<T>)> is_equal)
{
	// Unused options of the AbmIO object
	std::string delim(" ");
	bool one_file = true;
	std::vector<size_t> dims = {0,0,0};

	// Read new_vec 
	vec2D<T> new_vec;
	AbmIO io_int(fname, delim, one_file, dims);
	new_vec = io_int.read_vector<T>();

	// Return the result of comparison
	return is_equal(new_vec, expected);
}

/**
 * \brief Tests writing and reading 2D vector 
 * \details Writes and then reads from file, compares
 * 		the read vector to expected (written)   
 */
bool custom_test_suite()
{
	// Common and unused AbmIO properties
	bool one_file = true;
	std::vector<size_t> dims = {0,0,0};

	//
	// Types that can be compared directly/exactly
	//	

	// bool
	AbmIO io_bool("./test_data/wr_bool.txt", ",", one_file, dims);
	std::function<bool(vec2D<bool>, vec2D<bool>)> is_equal_bool = is_equal_exact<bool>;
	vec2D<bool> bool_exp = {{1, 0}, {1, 0}, {0, 1, 0, 0}, {1, 1}, {0}};
	if (!write_read_test(bool_exp, io_bool, is_equal_bool))
		return false;

	// int
 	AbmIO io_int("./test_data/wr_int.txt", ",", one_file, dims);
	std::function<bool(vec2D<int>, vec2D<int>)>is_equal_int = is_equal_exact<int>;
	vec2D<int> int_exp = {{1, 5, 2}, {8}, {4, 6, 10}};
	if (!write_read_test(int_exp, io_int, is_equal_int))
		return false;
	
	// std::string
 	AbmIO io_str("./test_data/wr_string.txt", ",", one_file, dims);
	std::function<bool(vec2D<std::string>, vec2D<std::string>)>is_equal_str = is_equal_exact<std::string>;
	vec2D<std::string> str_exp = {{"long", "short", "longlong"}, {"unsigned", "double"}};
	if (!write_read_test(str_exp, io_str, is_equal_str))
		return false;
	
	//
	// Types that need indirect comparison
	//	

	// double
	// We need one more argument to call is_equal_floats so to maintain function signature in 
	// the test we will wrap the call in a lambda
	double tol = 1e-5;
	// Has access to tol and calls is_equal_floats with it
	auto is_equal_wrap = [tol](vec2D<double> orig_v, vec2D<double> new_v)
		{ return is_equal_floats<double>(orig_v, new_v, tol); };	
	std::function<bool(vec2D<double>, vec2D<double>)>is_equal_double = is_equal_wrap;
	//
 	AbmIO io_dbl("./test_data/wr_double.txt", ",", one_file, dims);
	vec2D<double> dbl_exp = {{0.64677, 2.963, 6.868e5, 0.6256}, {0.4509e10, -0.75447, 0.7802},
		   					 {0.54701, 0.01890e-7, 0.3685, -0.0811}};
	if (!write_read_test(dbl_exp, io_dbl, is_equal_double))
		return false;
	
	// All passed
	return true;
}

/**
 * \brief Tests default constructor settings and behavior  
 */
bool default_test_suite()
{
	// Common default AbmIO object
	AbmIO io_dflt;
	
	//
	// Types that can be compared directly/exactly
	//	

	// bool
	std::function<bool(vec2D<bool>, vec2D<bool>)> is_equal_bool = is_equal_exact<bool>;
	vec2D<bool> bool_exp = {{1}, {1}, {0, 0, 1}, {1, 1}, {0, 1}};
	if (!write_read_test(bool_exp, io_dflt, is_equal_bool))
		return false;

	// int
	std::function<bool(vec2D<int>, vec2D<int>)>is_equal_int = is_equal_exact<int>;
	vec2D<int> int_exp = {{1, 5, 2}, {8, 9}, {4, 6, 10, 12}};
	if (!write_read_test(int_exp, io_dflt, is_equal_int))
		return false;
	
	// std::string
 	std::function<bool(vec2D<std::string>, vec2D<std::string>)>is_equal_str = is_equal_exact<std::string>;
	vec2D<std::string> str_exp = {{"long", "short"}, {"unsigned", "double", "int"}};
	if (!write_read_test(str_exp, io_dflt, is_equal_str))
		return false;

	//
	// Types that need indirect comparison
	//	

	// double
	// We need one more argument to call is_equal_floats so to maintain function signature in 
	// the test we will wrap the call in a lambda
	double tol = 1e-5;
	// Has access to tol and calls is_equal_floats with it
	auto is_equal_wrap = [tol](vec2D<double> orig_v, vec2D<double> new_v)
		{ return is_equal_floats<double>(orig_v, new_v, tol); };	
	std::function<bool(vec2D<double>, vec2D<double>)>is_equal_double = is_equal_wrap;
	//
 	vec2D<double> dbl_exp = {{0.64677, 2.963, 6.868e5, 0.6256}, {-0.75447, 0.1835, 0.7802},
		   					 {0.54701, 0.01890e-7, 0.3685, -0.0811, 1.9}};
	if (!write_read_test(dbl_exp, io_dflt, is_equal_double))
		return false;

	// All passed
	return true;
}

/**
 * \brief Tests writing and reading 1D vector 
 * \details Writes and then reads from file, compares
 * 		the read vector to expected (written)   
 */
bool write_1D_vector_test_suite()
{
	// Common and unused AbmIO properties
	bool one_file = true;
	std::vector<size_t> dims = {0,0,0};

	//
	// Types that can be compared directly/exactly
	//	

	// bool
	AbmIO io_bool("./test_data/wr_bool_1D.txt", ",", one_file, dims);
	std::function<bool(vec2D<bool>, vec2D<bool>)> is_equal_bool = is_equal_exact<bool>;
	vec1D<bool> bool_exp = {1, 0, 0, 1, 0};
	if (!write_read_test(bool_exp, io_bool, is_equal_bool))
		return false;

	// int
 	AbmIO io_int("./test_data/wr_int_1D.txt", ",", one_file, dims);
	std::function<bool(vec2D<int>, vec2D<int>)>is_equal_int = is_equal_exact<int>;
	vec1D<int> int_exp = {1, 8, 4};
	if (!write_read_test(int_exp, io_int, is_equal_int))
		return false;
	
	// std::string
 	AbmIO io_str("./test_data/wr_string_1D.txt", ",", one_file, dims);
	std::function<bool(vec2D<std::string>, vec2D<std::string>)>is_equal_str = is_equal_exact<std::string>;
	vec1D<std::string> str_exp = {"long", "unsigned"};
	if (!write_read_test(str_exp, io_str, is_equal_str))
		return false;
	
	//
	// Types that need indirect comparison
	//	

	// double
	// We need one more argument to call is_equal_floats so to maintain function signature in 
	// the test we will wrap the call in a lambda
	double tol = 1e-5;
	// Has access to tol and calls is_equal_floats with it
	auto is_equal_wrap = [tol](vec2D<double> orig_v, vec2D<double> new_v)
		{ return is_equal_floats<double>(orig_v, new_v, tol); };	
	std::function<bool(vec2D<double>, vec2D<double>)>is_equal_double = is_equal_wrap;
	//
 	AbmIO io_dbl("./test_data/wr_double_1D.txt", ",", one_file, dims);
	vec1D<double> dbl_exp = {0.64677, 0.4509e10, -0.0811};
	if (!write_read_test(dbl_exp, io_dbl, is_equal_double))
		return false;
	
	// All passed
	return true;
}

/// Uses object's overloaded ostream operator to write, then reads, and checks
bool write_object_test_suite()
{
	// Common and unused AbmIO properties
	bool one_file = true;
	std::vector<size_t> dims = {0,0,0};

	AbmIO abm_io("./test_data/wr_struct.txt", ",", one_file, dims);
	TestStruct test_struct;

	// Write to file	
	abm_io.write_object(test_struct);
	
	// Read and compare
	bool tbval = false;
	int tival = 0;
	std::string tsval;
	double tdval = 0.0;

	std::ifstream input("./test_data/wr_struct.txt");	
	input >> tbval >> tival >> tsval >> tdval;

	if (tbval != test_struct.bval)
		return false;
	if (tival != test_struct.ival)
		return false;
	if (tsval != test_struct.sval)
		return false;
	if (!float_equality<double>(tdval, test_struct.dval, 1e-5))
		return false;

	return true;	
}

/** 
 * \brief Checks writing and reading under different AbmIO settings
 * \details Writes to and then reads from the same file, compares 
 * 		the read 2D vector of type T to expected
 * @param orig - 2D vector of expected values, each inner vector 
 * 						is a single line of file 
 * @param io_obj - AbmIO object 
 * @param is_equal - function object for comparing expected with 
 * 						read values of 2D vectors of type T - needs complete comparison
 * 						of the 2D vector; returns true if equal
 * @return true if is_equal returned true, false otherwise
 */
template <typename T> 
bool write_read_test(const vec2D<T> orig_vec, const AbmIO& io_obj, 
	const std::function<bool(vec2D<T>, vec2D<T>)> is_equal)
{
	// Write then read
	vec2D<T> new_vec;
	io_obj.write_vector<T>(orig_vec);
	new_vec = io_obj.read_vector<T>();

	// True if vectors are identical
	return is_equal(orig_vec, new_vec);
}

/** 
 * \brief Checks writing and reading under different AbmIO settings - 1D version
 * \details Writes to and then reads from the same file, compares 
 * 		the read 2D vector of type T to expected
 * @param orig - 1D vector of expected values, each element
 * 						is a single line of file 
 * @param io_obj - AbmIO object 
 * @param is_equal - function object for comparing expected with 
 * 						read values of 2D vectors of type T - needs complete comparison
 * 						of the 2D vector; returns true if equal
 * @return true if is_equal returned true, false otherwise
 */
template <typename T> 
bool write_read_test(const vec1D<T> orig_vec, const AbmIO& io_obj, 
	const std::function<bool(vec2D<T>, vec2D<T>)> is_equal)
{
	// Write then read
	vec2D<T> new_vec;
	io_obj.write_vector<T>(orig_vec);
	new_vec = io_obj.read_vector<T>();

	// Convert 1D input vector into a 2D one
	vec2D<T> orig_vec_2D;
	for (const auto& v : orig_vec){
		vec1D<T> vtemp;
		vtemp.push_back(v);
		orig_vec_2D.push_back(vtemp);
	}

	// True if vectors are identical
	return is_equal(orig_vec_2D, new_vec);
}
