#ifndef FVS_SCOPE_HPP
#define FVS_SCOPE_HPP

#include <utility>
#include <type_traits>
#include <functional>
#include <exception>

namespace fvs {
  namespace detail {

    // detail: constexpr compability
    constexpr int cx_uncaught_exceptions() noexcept {
      return std::is_constant_evaluated() ? 0 : std::uncaught_exceptions();
    }
    
  } // namespace fvs::detail

  // 3.3.2, Class templates scope_exit, scope_fail, and scope_success
  template <class EF> class scope_exit {
  public:
    template <class EFP>
      requires (!std::is_same_v<std::remove_cvref_t<EFP>, scope_exit> &&
                std::is_constructible_v<EF, EFP>)
      explicit constexpr scope_exit(EFP&& f) noexcept(
        std::is_nothrow_constructible_v<EF, EFP> ||
        std::is_nothrow_constructible_v<EF, EFP&>) :
        exit_function([&]() noexcept(
          std::is_nothrow_constructible_v<EF, EFP> ||
          std::is_nothrow_constructible_v<EF, EFP&>) {
          if constexpr (std::is_nothrow_constructible_v<EF, EFP>)
            return EF(std::forward<EFP>(f));
          else if constexpr (std::is_nothrow_constructible_v<EF, EFP&>)
            return EF(f);
          else {
            try {
              return EF(f);
            }
            catch (...) {
              f();
              throw;
            }
          }
        }()) {}

    constexpr scope_exit(scope_exit&& rhs) noexcept(
      std::is_nothrow_move_constructible_v<EF> ||
      std::is_nothrow_copy_constructible_v<EF>
    )
      requires (std::is_nothrow_move_constructible_v<EF> ||
                std::is_copy_constructible_v<EF>) :
      exit_function(static_cast<std::conditional_t<
                    std::is_nothrow_move_constructible_v<EF>,
                    EF&&, EF&>>(rhs.exit_function)),
      execute_on_destruction(rhs.execute_on_destruction) {
        rhs.release();
      }

    scope_exit(const scope_exit&) = delete;
    scope_exit& operator=(const scope_exit&) = delete;
    scope_exit& operator=(scope_exit&&) = delete;

    constexpr ~scope_exit () {
      if (execute_on_destruction)
        exit_function();
    }

    constexpr void release() noexcept {
      execute_on_destruction = false;
    }

  private:
    EF exit_function;
    bool execute_on_destruction{true};
    // int uncaught_on_creation{detail::cx_uncaught_exceptions()};
  };

  template <class EF>
    scope_exit(EF) -> scope_exit<EF>;
  
  template <class EF> class scope_fail {
  public:
    template <class EFP>
      requires (!std::is_same_v<std::remove_cvref_t<EFP>, scope_fail> &&
                std::is_constructible_v<EF, EFP>)
      explicit constexpr scope_fail(EFP&& f) noexcept(
        std::is_nothrow_constructible_v<EF, EFP> ||
        std::is_nothrow_constructible_v<EF, EFP&>) :
        exit_function([&]() noexcept(
          std::is_nothrow_constructible_v<EF, EFP> ||
          std::is_nothrow_constructible_v<EF, EFP&>) {
          if constexpr (std::is_nothrow_constructible_v<EF, EFP>)
            return EF(std::forward<EFP>(f));
          else if constexpr (std::is_nothrow_constructible_v<EF, EFP&>)
            return EF(f);
          else {
            try {
              return EF(f);
            }
            catch (...) {
              f();
              throw;
            }
          }
        }()) {}

    constexpr scope_fail(scope_fail&& rhs) noexcept(
      std::is_nothrow_move_constructible_v<EF> ||
      std::is_nothrow_copy_constructible_v<EF>
    )
      requires (std::is_nothrow_move_constructible_v<EF> ||
                std::is_copy_constructible_v<EF>) :
      exit_function(static_cast<std::conditional_t<
                    std::is_nothrow_move_constructible_v<EF>,
                    EF&&, EF&>>(rhs.exit_function)),
      execute_on_destruction(rhs.execute_on_destruction),
      uncaught_on_creation(rhs.uncaught_on_creation) {
        rhs.release();
      }

    scope_fail(const scope_fail&) = delete;
    scope_fail& operator=(const scope_fail&) = delete;
    scope_fail& operator=(scope_fail&&) = delete;

    constexpr ~scope_fail () {
      if (execute_on_destruction &&
          detail::cx_uncaught_exceptions() > uncaught_on_creation)
        exit_function();
    }

    constexpr void release() noexcept {
      execute_on_destruction = false;
    }

  private:
    EF exit_function;
    bool execute_on_destruction{true};
    int uncaught_on_creation{detail::cx_uncaught_exceptions()};
  };

  template <class EF>
    scope_fail(EF) -> scope_fail<EF>;

  template <class EF> class scope_success {
  public:
    template <class EFP>
      requires (!std::is_same_v<std::remove_cvref_t<EFP>, scope_success> &&
                std::is_constructible_v<EF, EFP>)
      explicit constexpr scope_success(EFP&& f) noexcept(
        std::is_nothrow_constructible_v<EF, EFP> ||
        std::is_nothrow_constructible_v<EF, EFP&>) :
        exit_function([&]() noexcept(
          std::is_nothrow_constructible_v<EF, EFP> ||
          std::is_nothrow_constructible_v<EF, EFP&>) {
          if constexpr (std::is_nothrow_constructible_v<EF, EFP>)
            return EF(std::forward<EFP>(f));
          else
            return EF(f); // f is not invoked when throwing
        }()) {}

    constexpr scope_success(scope_success&& rhs) noexcept(
      std::is_nothrow_move_constructible_v<EF> ||
      std::is_nothrow_copy_constructible_v<EF>
    )
      requires (std::is_nothrow_move_constructible_v<EF> ||
                std::is_copy_constructible_v<EF>) :
      exit_function(static_cast<std::conditional_t<
                    std::is_nothrow_move_constructible_v<EF>,
                    EF&&, EF&>>(rhs.exit_function)),
      execute_on_destruction(rhs.execute_on_destruction),
      uncaught_on_creation(rhs.uncaught_on_creation) {
        rhs.release();
      }

    scope_success(const scope_success&) = delete;
    scope_success& operator=(const scope_success&) = delete;
    scope_success& operator=(scope_success&&) = delete;

    // MAY BE POTENTIALLY-THROWING!!
    constexpr ~scope_success () noexcept(noexcept(exit_function())) {
      if (execute_on_destruction &&
          detail::cx_uncaught_exceptions() <= uncaught_on_creation)
        exit_function();
    }

    constexpr void release() noexcept {
      execute_on_destruction = false;
    }

  private:
    EF exit_function;
    bool execute_on_destruction{true};
    int uncaught_on_creation{detail::cx_uncaught_exceptions()};
  };

  template <class EF>
    scope_success(EF) -> scope_success<EF>;

  // 3.3.3, Class template unique_resource
  namespace detail {

    // detail: stored resource type
    template<class R>
      using stored_resource_t =
        std::conditional_t<std::is_reference_v<R>,
                           std::reference_wrapper<std::remove_reference_t<R>>,
                           R>;

    // detail: underlying_resource
    template<class R>
      constexpr auto& to_underlying_resource(stored_resource_t<R> &rs) noexcept {
        if constexpr (std::is_reference_v<R>)
          return rs.get();
        else
          return rs;
      }

    // detail: select assignment rhs
    template<class R1, class RR>
      using assign_if_noexcept_t = std::conditional_t<
        std::is_nothrow_assignable_v<R1&, RR>,
        RR&&, const RR&
      >;

    // detail: aggregate tag
    struct unique_resource_aggregate_tag_t {
      explicit unique_resource_aggregate_tag_t() = default;
    };

    // detail: workaround for clang's bug
    struct unique_resource_friend;

  } // namespace fvs::detail

  template <class R, class D> class unique_resource {
    using R1 = detail::stored_resource_t<R>;
  public:
    // 3.3.3.2, Constructors
    constexpr unique_resource()
      requires
        (std::is_default_constructible_v<R> && std::is_default_constructible_v<D>) :
      resource(), deleter() {}
    
    template <class RR, class DD>
      constexpr unique_resource(RR&& r, DD&& d) noexcept(
      (std::is_nothrow_constructible_v<R1, RR> || std::is_nothrow_constructible_v<R1, RR&>) &&
      (std::is_nothrow_constructible_v<D , DD> || std::is_nothrow_constructible_v<D , DD&>))
      requires
        (std::is_constructible_v<R1, RR> && std::is_constructible_v<D, DD> &&
         (std::is_nothrow_constructible_v<R1, RR> || std::is_constructible_v<R1, RR&>) &&
         (std::is_nothrow_constructible_v<D , DD> || std::is_constructible_v<D , DD&>)) :
      resource([&]() noexcept(
        std::is_nothrow_constructible_v<R1, RR> || std::is_nothrow_constructible_v<R1, RR&>) {
        if constexpr (std::is_nothrow_constructible_v<R1, RR>)
          return R1(std::forward<RR>(r));
        else if constexpr (std::is_nothrow_constructible_v<R1, RR&>)
          return R1(r);
        else {
          try {
            return R1(r);
          }
          catch (...) {
            d(r);
            throw;
          }
        }
      }()),
      deleter([&, &resource = this->resource]() noexcept(
        std::is_nothrow_constructible_v<D, DD> || std::is_nothrow_constructible_v<D, DD&>) {
        if constexpr (std::is_nothrow_constructible_v<D, DD>)
          return D(std::forward<DD>(d));
        else if constexpr (std::is_nothrow_constructible_v<D, DD&>)
          return D(d);
        else {
          try {
            return D(d);
          }
          catch (...) {
            d(detail::to_underlying_resource<R>(resource));
            throw;
          }
        }
      }()) {};
    constexpr unique_resource(unique_resource&& rhs) noexcept(
      std::is_nothrow_move_constructible_v<R1> && std::is_nothrow_move_constructible_v<D>
    ) :
      resource(static_cast<std::conditional_t<
                 std::is_nothrow_move_constructible_v<R1>,
                 R1&&, R1&>>(rhs.resource)),
      deleter([&, &resource = this->resource]() noexcept(
        std::is_nothrow_move_constructible_v<D> ||
        std::is_nothrow_constructible_v<D, D&>
      ) {
        if constexpr (std::is_nothrow_move_constructible_v<D>)
          return D(std::move(rhs.deleter));
        else if constexpr (std::is_nothrow_constructible_v<D, D&> ||
                           !std::is_nothrow_move_constructible_v<R1>)
          return D(rhs.deleter);
        else if (!rhs.execute_on_reset)
          return D(rhs.deleter);
        else {
          try {
            return D(rhs.deleter);
          }
          catch (...) {
            rhs.deleter(detail::to_underlying_resource<R>(resource));
            rhs.release();
            throw;
          }
        }
      }()),
      execute_on_reset(rhs.execute_on_reset) {
      rhs.execute_on_reset = false;
    }

    // 3.3.3.3, Destructor
    constexpr ~unique_resource() { reset(); }

    // 3.3.3.4, Assignment
    constexpr unique_resource& operator=(unique_resource&& rhs) noexcept(
      std::is_nothrow_move_assignable_v<R1> && std::is_nothrow_move_assignable_v<D>
    ) {
      reset();
      if constexpr (std::is_nothrow_move_assignable_v<R1>) {
        if constexpr (std::is_nothrow_move_assignable_v<D>) {
          resource = std::move(rhs.resource);
          deleter = std::move(rhs.deleter);
        }
        else {
          deleter = rhs.deleter;
          resource = std::move(rhs.resource);
        }
      }
      else {
        if constexpr (std::is_nothrow_move_assignable_v<D>) {
          resource = rhs.resource;
          deleter = std::move(rhs.deleter);
        }
        else {
          resource = rhs.resource;
          deleter = rhs.deleter;
        }
      }
      execute_on_reset = rhs.execute_on_reset;
      rhs.execute_on_reset = false;
      return *this;
    }

    // 3.3.3.5, Other member functions
    constexpr void reset() noexcept {
      if (execute_on_reset) {
        execute_on_reset = false;
        deleter(detail::to_underlying_resource<R>(resource));
      }
    }
    template <class RR>
      requires std::is_assignable_v<R1&, detail::assign_if_noexcept_t<R1, RR>>
      constexpr void reset(RR&& r) {
        reset();
        if constexpr (std::is_nothrow_assignable_v<R1&, RR>)
          resource = std::forward<RR>(r);
        else
          resource = std::as_const(r);
        execute_on_reset = true;
      }

    constexpr void release() noexcept { execute_on_reset = false; }
    constexpr const R& get() const noexcept { return resource; }
    constexpr std::add_lvalue_reference_t<std::remove_pointer_t<R>>
      operator*() const noexcept
        requires (std::is_pointer_v<R> && !std::is_void_v<std::remove_pointer_t<R>>) {
      return *get();
    }
    constexpr R operator->() const noexcept requires std::is_pointer_v<R> {
      return get();
    }
    constexpr const D& get_deleter() const noexcept { return deleter; }

  private:
    R1 resource;
    D deleter;
    bool execute_on_reset{true};
    
    friend struct fvs::detail::unique_resource_friend;
    
    // detail: aggregate-like construction
    template<class RR = R1, class DD = D>
      requires std::is_constructible_v<R1, RR> && std::is_constructible_v<D, DD>
    constexpr unique_resource(detail::unique_resource_aggregate_tag_t, RR&& r = R1(), DD&& d = D(), bool v = false)
      noexcept(std::is_nothrow_constructible_v<R1, RR> && std::is_nothrow_constructible_v<D, DD>) :
      resource(std::forward<RR>(r)), deleter(std::forward<DD>(d)), execute_on_reset(v) {}
  };

  template<class R, class D>
    unique_resource(R, D) -> unique_resource<R, D>;

  namespace detail {

    // detail: workaround for clang's bug
    struct unique_resource_friend {
      template <class R2, class D2, class S = std::decay_t<R2>>
        static constexpr unique_resource<std::decay_t<R2>, std::decay_t<D2>>
          make(R2&& resource, const S& invalid, D2&& d)
          noexcept(std::is_nothrow_constructible_v<std::decay_t<R2>, R2> &&
                   std::is_nothrow_constructible_v<std::decay_t<D2>, D2>) {
            using DR = std::decay_t<R2>;
            using DD = std::decay_t<D2>;
            if (resource == invalid ? true : false) {
              return unique_resource<DR, DD>(detail::unique_resource_aggregate_tag_t{},
                                             std::forward<R2>(resource), std::forward<D2>(d), false);
            }
            else if constexpr (std::is_nothrow_constructible_v<DR, R2> &&
                               std::is_nothrow_constructible_v<DD, D2>)
              return unique_resource<DR, DD>(detail::unique_resource_aggregate_tag_t{},
                                             std::forward<R2>(resource), std::forward<D2>(d), true);
            else {
              try {
                return unique_resource<DR, DD>(detail::unique_resource_aggregate_tag_t{},
                                               std::forward<R2>(resource), std::forward<D2>(d), true);
              }
              catch (...) {
                d(resource);
                throw;
              }
            }
          }
    };

  } // namespace fvs::detail

  // 3.3.3.6, unique_resource creation
  template <class R2, class D2, class S = std::decay_t<R2>>
    constexpr unique_resource<std::decay_t<R2>, std::decay_t<D2>>
      make_unique_resource_checked(R2&& resource, const S& invalid, D2&& d)
      noexcept(std::is_nothrow_constructible_v<std::decay_t<R2>, R2> &&
               std::is_nothrow_constructible_v<std::decay_t<D2>, D2>) {
        return detail::unique_resource_friend::make(std::forward<R2>(resource), invalid, std::forward<D2>(d));
    }

} // namespace fvs
#endif
