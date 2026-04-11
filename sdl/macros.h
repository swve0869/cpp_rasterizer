
#pragma once
#include <sycl/sycl.hpp>
#include <chrono>
#include <iostream>


//using namespace sycl;


#define PRINT_POLY 0
#define PRINT 0


static auto exception_handler = [](sycl::exception_list eList) {
	for (std::exception_ptr const& e : eList) {
		try {
			std::rethrow_exception(e);
		}
		catch (sycl::exception const& se) {
			std::cout << "SYCL Exception: " << se.what() << std::endl;
			std::terminate();
		}
		catch (std::exception const& e) {
#if _DEBUG
			std::cout << "Failure" << std::endl;
#endif
			std::terminate();
		}
	}
	};

// selects hardware device for accelleration 
auto selector = sycl::gpu_selector_v;

sycl::queue q(selector, exception_handler);



//auto selector = sycl::e

// this is the format I believe to catching asycnhronous errors when perallelized
//try {
//	queue q(selector, exception_handler);
//	… …
//}
//catch (exception const& e) {
//	… …
//}