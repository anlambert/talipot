include(CheckCXXSourceCompiles)

if(NOT MSVC)
  set(CMAKE_REQUIRED_FLAGS -msse3)
endif()
check_cxx_source_compiles("
#include <pmmintrin.h>
int main() {
	__m128d a = _mm_set1_pd(42), b = _mm_set1_pd(23);
	_mm_hadd_pd(a, b);
	return 0;
}" has_sse3_pmmintrin)

if (NOT has_sse3_pmmintrin)
	if(NOT MSVC)
		set(CMAKE_REQUIRED_FLAGS -sse3)
	endif()
  check_cxx_source_compiles("
#include <intrin.h>
int main() {
	__m128d a = _mm_set1_pd(42), b = _mm_set1_pd(23);
	_mm_hadd_pd(a, b);
	return 0;
}" has_sse3_intrin)
endif()
