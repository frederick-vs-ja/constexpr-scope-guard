# Constexpr scope guards
Constexpr-ized version of scope guards in LFTS v3. Single-header implementation.

Currently referenced version: [N4873](https://wg21.link/n4873)

Other documentation(s):
* [cppreference](https://en.cppreference.com/w/cpp/experimental/lib_extensions_3#Scope_guard)

Requires:
* C++20 `std::is_constant_evaluated` ([reference](https://en.cppreference.com/w/cpp/types/is_constant_evaluated))
* C++20 constexpr destructors ([reference](https://en.cppreference.com/w/cpp/language/constexpr))
* Numerous C++17/14/11 features

Not sufficiently tested for now (2020-12-24)
