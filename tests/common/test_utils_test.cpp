#include "test_utils.h"

/*************************************************************** 
 * Tests of general testing utility functions
 **************************************************************/

// Aliases
template <typename T>
using vec2D = std::vector<std::vector<T>>; 

void test_pass_test();
bool exception_wrapper_test(bool verbose=false);
bool exception_wrapper_member_functions_test(bool verbose=false);
int exception_helper_v1(double, double);
void exception_helper_v2(bool);
bool equal_doubles_test(bool verbose=false);
bool exact_equal_2D_vectors_test();
bool floating_equal_2D_vectors_test();

// Supporting functions
template <typename T>
bool compare_vectors_helper(vec2D<T> v1, vec2D<T> v2, vec2D<T> v3, vec2D<T> v4);

// For member function exception testing
class TestClass{
public:
	// Member function - two args, two types of error
	// one inherited from another
	bool throwing_f(double d, int i)
	{ 
		if ((d > 0.0) && (i > 0))
			throw std::runtime_error("d and i larger than 0");
		if (i == 0)
			throw std::range_error("i equal to 0");
		return true;
	}
	// Member function with differnt args and error type
	void throwing_bcast(bool b)
	{
		if (!b)
			throw std::bad_cast();
	}
};

int main()
{
	bool verbose = true;
	test_pass_test();
	test_pass(exception_wrapper_test(verbose), "Exception wrapper");
	test_pass(exception_wrapper_member_functions_test(verbose), "Exception wrapper - member functions");
	test_pass(equal_doubles_test(verbose), "Equality of doubles");
	test_pass(exact_equal_2D_vectors_test(), "Exact equality of 2D vectors");
	test_pass(floating_equal_2D_vectors_test(), "Floating point equality of 2D vectors");
	return 0;
}

// Verifies if printing right messages
void test_pass_test()
{
	test_pass(false, "Should fail");
	test_pass(true, "Should work");
}

// Verifies correct behavior of exception test with
// exceptions and without them
bool exception_wrapper_test(bool verbose)
{
	const std::runtime_error rtime("Run time error");
	const std::range_error range("Run time - Range error");
	const std::bad_cast cast;

	// No exception
	if (exception_test(verbose, nullptr, exception_helper_v1, 0.5, 0.1))
		return false;
	// Correct exception
	if (!exception_test(verbose, &rtime, exception_helper_v1, 2000.0, 0.1))
		return false;
	// Correct exception different than above
	if (!exception_test(verbose, &range, exception_helper_v1, 2.0, 0.1))
		return false;
	// Correct exception, different function and arguments
	if (!exception_test(verbose, &cast, exception_helper_v2, false))
		return false;
	// No exception, different function and arguments
	if (exception_test(verbose, nullptr, exception_helper_v2, true))
		return false;
	return true;
}

// Functions to be called by exception wrapper
// The only meaning in choice of exception types here is to 
// test/demonstrate that the wrapper correctly captures their
// types and polymorphism
int exception_helper_v1(double d1, double d2)
{
	if (d1 > 1.0)
		if (d1 > 10.0)
			throw std::runtime_error("d1 larger than 10.0");
		else
			throw std::range_error("d1 larger than 1.0, smaller than 10.0");
 	else
		return (int)(d1+d2);	
}

void exception_helper_v2(bool b)
{
	if (!b)
		throw std::bad_cast();
}

// Verifies correct behavior of exception test with
// exceptions and without them - version for classes,
// i.e. member functions
bool exception_wrapper_member_functions_test(bool verbose)
{
	const std::runtime_error rtime("Run time error");
	const std::range_error range("Run time - Range error");
	const std::bad_cast cast;
	TestClass tclass; 	

	// No exception
	if (exception_test(verbose, nullptr, &TestClass::throwing_f, tclass, -0.5, -1))
		return false;
	// Correct exception
	if (!exception_test(verbose, &rtime, &TestClass::throwing_f, tclass, 2000.0, 1))
		return false;
	// Correct exception different than above
	if (!exception_test(verbose, &range, &TestClass::throwing_f, tclass, 2.0, 0))
		return false;
	// Correct exception, different function and arguments
	if (!exception_test(verbose, &cast, &TestClass::throwing_bcast, tclass, false))
		return false;
	// No exception, different function and arguments
	if (exception_test(verbose, nullptr, &TestClass::throwing_bcast, tclass, true))
		return false;
	return true;
}

// Test the double equality function
// Add capability as needed 
bool equal_doubles_test(bool verbose)
{
	double tol = 1.0e-5;
	if (verbose)
		print_msg("Comparing 1.0 and 1.0...");
	if (!float_equality(1.0, 1.0, tol))
		return false;
	//
	if (verbose)
		print_msg("Comparing 1.0 and 1.01 with tol 0.1...");
	if (!float_equality(1.0, 1.01, 1e-1))
		return false;
	//
	if (verbose)
		print_msg("Comparing 1.0 and 0.9999998...");
	if (!float_equality(1.0, 0.9999998, tol))
		return false;
	//
	if (verbose)
		print_msg("Comparing 1.0 and 0.99998...");
	if (float_equality(1.0, 0.99998, tol))
		return false;
	//
	if (verbose)
		print_msg("Comparing 1.0e10 and 1.001e10... - need to expand for large numbers");
	if (float_equality(1.0e10, 1.001e10, tol))
		return false;
	//
	if (verbose)
		print_msg("Comparing 1.0 and -1.0...");
	if (float_equality(1.0, -1.0, tol))
		return false;
	return true;
}

// Performs exact comparison of 2D vectors
bool exact_equal_2D_vectors_test()
{
	// Bool
	vec2D<bool> bvec_1 = {{true, false, true}, {true, true}};
	vec2D<bool> bvec_2 = {{true, false, true}, {true, true, true}, {false}};
	vec2D<bool> bvec_3 = {{true, false, true}, {true, true}, {false, false}};	
	vec2D<bool> bvec_4 = {{true, false, false}, {true, true}, {false, false}};

	if (!compare_vectors_helper(bvec_1, bvec_2, bvec_3, bvec_4))
		return false;

	// Int
	vec2D<int> ivec_1 = {{1, 2, 3}, {4, 5}};
	vec2D<int> ivec_2 = {{1, 2, 2}, {3, 4, 5}, {6}};
	vec2D<int> ivec_3 = {{1, 2, 2}, {3, 4, 5}, {6, 6}};	
	vec2D<int> ivec_4 = {{1, 2, 1}, {3, 4, 5}, {6, 6}};

	if (!compare_vectors_helper(ivec_1, ivec_2, ivec_3, ivec_4))
		return false;

	// String
	vec2D<std::string> svec_1 = {{"Radinovic", "Belodedic", "Marovic"}, {"Jugovic", "Pancev"}};
	vec2D<std::string> svec_2 = {{"Radinovic", "Belodedic", "Marovic"}, {"Jugovic", "Pancev"}, {"Mihajlovic", "Savicevic", "Stojanovic"}};
	vec2D<std::string> svec_3 = {{"Radinovic", "Belodedic", "Marovic"}, {"Jugovic", "Pancev"}, {"Mihajlovic", "Savicevic"}};	
	vec2D<std::string> svec_4 = {{"Binic", "Belodedic", "Marovic"}, {"Jugovic", "Pancev"}, {"Mihajlovic", "Savicevic"}};

	if (!compare_vectors_helper(svec_1, svec_2, svec_3, svec_4))
		return false;

	return true;
}

// Performs inexact comparison of 2D vectors
bool floating_equal_2D_vectors_test()
{
	vec2D<double> v1 = {{1.5, 2, 3}, {4, 5}};
	vec2D<double> v2 = {{1.5, 2, 2}, {3, 4, 5}, {6}};
	vec2D<double> v3 = {{1.5, 2, 2}, {3, 4, 5}, {6, 6}};	
	vec2D<double> v4 = {{1.5, 2, 1.502}, {3, 4, 5}, {6, 6}};

	if (is_equal_floats(v1, v2, 1e-5))
		return false;
	if (is_equal_floats(v2, v3, 1e-5))
		return false;
	if (is_equal_floats(v3, v4, 1e-5))
		return false;
	if (!is_equal_floats(v1, v1, 1e-5))
		return false;

	return true;
}

// Checks the vectors for different conditions, true and false, for the exact
// vector comparison functionality. The order of specific types of vectors is important.
template <typename T>
bool compare_vectors_helper(vec2D<T> v1, vec2D<T> v2, vec2D<T> v3, vec2D<T> v4)
{
	if (is_equal_exact(v1, v2))
		return false;
	if (is_equal_exact(v2, v3))
		return false;
	if (is_equal_exact(v3, v4))
		return false;
	if (!is_equal_exact(v1, v1))
		return false;

	return true;
}


