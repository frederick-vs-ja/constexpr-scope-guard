# Constexpr scope guards
Constexpr-ized version of scope guards in LFTS v3. Single-header implementation.

Currently referenced version: [N4908](https://wg21.link/n4908)

Other documentation(s):
* [cppreference](https://en.cppreference.com/w/cpp/experimental/lib_extensions_3#Scope_guard)

Requires:
* C++20 constraints (requires-clause) ([reference](https://en.cppreference.com/w/cpp/language/constraints))
* C++20 `std::is_constant_evaluated` ([reference](https://en.cppreference.com/w/cpp/types/is_constant_evaluated))
* C++20 constexpr destructors ([reference](https://en.cppreference.com/w/cpp/language/constexpr))
* C++20 `[[no_unique_address]]` ([reference](https://en.cppreference.com/w/cpp/language/attributes/no_unique_address))
* Numerous C++17/14/11 features

Modifications from the LFTS v3 version:
* utilities are in namespace `::fvs` instead of `::std::experimental`
* all member and non-member functions are constexpr-ized

Test cases are still missing for now. (2022-09-05)
