/*
 * Copyright (c) Fraunhofer ITWM - <http://www.itwm.fraunhofer.de/>, 2016
 * 
 * This file is part of GaspiLS.
 * 
 * GaspiLS is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * version 3 as published by the Free Software Foundation.
 * 
 * GaspiLS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GaspiLS. If not, see <http://www.gnu.org/licenses/>.
 *
 * Scheduler.hpp
 *
 */

#ifndef DEVICE_SCHEDULER_HPP_
#define DEVICE_SCHEDULER_HPP_

//http://www.goldsborough.me/cpp/2018/05/22/00-32-43-type_erasure_for_unopinionated_interfaces_in_c++/

#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <typeindex>
#include <vector>

#include <ACE/device/SchedulerInterface.hpp>
#include <ACE/device/Types.hpp>
#include <ACE/device/numa/Scheduler.hpp>
#include <ACE/schedule/Schedule.hpp>
#include <ACE/utility/Macros.hpp>

//class Object {
//
//public:
//
//    template <typename T>
//    Object(const T& obj)
//    : object(std::make_shared<Model<T>>(std::move(obj)))
//    {}
//
//    std::string getName() const {
//        return object->getName();
//    }
//
//   struct Concept {
//       virtual ~Concept() {}
//       virtual std::string getName() const = 0;
//   };
//
//   template< typename T >
//   struct Model : Concept {
//       Model(const T& t)
//       : object(t)
//       {}
//
//       std::string getName() const override {
//           return object.getName();
//       }
//
//     private:
//       T object;
//   };
//
//   std::shared_ptr<const Concept> object;
//};

//class AnyPerson {
// public:
//  template<
//    typename P,
//    typename = std::enable_if_t<std::is_base_of<Library::Person, std::decay_t<P>>::value>>
//  /* implicit */ AnyPerson(P&& person)
//    : content_(make_holder(std::forward<P>(person), &std::remove_reference_t<P>::work)) {}
//
//  AnyPerson(const AnyPerson& other) : content_(other.content_->clone()) { }
//  AnyPerson(AnyPerson&& other) noexcept { swap(other); }
//  AnyPerson& operator=(AnyPerson other) { swap(other); return *this; }
//  void swap(AnyPerson& other) noexcept { content_.swap(other.content_); }
//  ~AnyPerson() = default;
//
//  template<typename... Args>
//  void work(Args&&... arguments) {
//    std::vector<Any> any_arguments;
//    // replace collect_any_vector with fold expression in C++17.
//    detail::collect_any_vector(any_arguments, std::forward<Args>(arguments)...);
//    return content_->invoke_work(std::move(any_arguments));
//  }
//
//  const std::string& name() const noexcept {
//    return content_->name();
//  }
//
//  template<typename P>
//  P& get() {
//    if (std::type_index(typeid(P)) == std::type_index(content_->type_info())) {
//      return static_cast<Holder<P>&>(*content_).value_;
//    }
//    throw std::bad_cast();
//  }
//
// private:
//  struct Placeholder {
//    virtual ~Placeholder() = default;
//    virtual const std::type_info& type_info() const = 0;
//    virtual std::unique_ptr<Placeholder> clone() = 0;
//    virtual const std::string& name() const noexcept = 0;
//    virtual void invoke_work(std::vector<Any>&& arguments) = 0;
//  };
//
//  template<typename P, typename... Args>
//  struct Holder : public Placeholder {
//    template<typename Q>
//    explicit Holder(Q&& person) : person_(std::forward<Q>(person)) { }
//
//    const std::type_info& type_info() const override { return typeid(P); }
//
//    std::unique_ptr<Placeholder> clone() override {
//      return std::make_unique<Holder<P, Args...>>(person_);
//    }
//
//    const std::string& name() const noexcept override {
//      return person_.name();
//    }
//
//    void invoke_work(std::vector<Any>&& arguments) override {
//      assert(arguments.size() == sizeof...(Args));
//      invoke_work(std::move(arguments), std::make_index_sequence<sizeof...(Args)>());
//    }
//
//    template<size_t... Is>
//    void invoke_work(std::vector<Any>&& arguments, std::index_sequence<Is...>) {
//      // Expand the index sequence to access each `Any` stored in `arguments`,
//      // and cast to the type expected at each index. Also note we move each
//      // value out of the `Any`.
//      return person_.work(std::move(arguments[Is].get<Args>())...);
//    }
//
//    P person_;
//  };
//
//  template<typename P, typename... Args>
//  std::unique_ptr<Placeholder> make_holder(P&& person, void(std::remove_reference_t<P>::*)(Args...)) {
//    return std::make_unique<Holder<P, Args...>>(std::forward<P>(person));
//  }
//
//  std::unique_ptr<Placeholder> content_;
//};


namespace ace {
namespace device {


//
//class Any {
// public:
//   // Disable the constructor for `Any`, otherwise trying to copy a
//   // non-const `Any` actually constructs an `Any` containing an `Any`.
//  template <
//      typename T,
//      typename = std::enable_if_t<!std::is_same<Any, std::decay_t<T>>::value>>
//  /* implicit */ Any(T&& value)
//  : content_(std::make_unique<Holder<T>>(std::forward<T>(value))) {}
//
//  // We use Scott Meyer's copy-and-swap idiom to implement special member functions.
//
//  // `clone()` gives us a way to copy-construct a value through a type-agnostic virtual interface.
//  Any(const Any& other)
//  : content_(other.content_->clone())
//  {}
//
//  Any(Any&& other) noexcept
//  {
//    swap(other);
//  }
//
//  Any& operator=(Any other) {
//    swap(other);
//    return *this;
//  }
//  void swap(Any& other) noexcept { content_.swap(other.content_); }
//  ~Any() = default;
//
//  template <typename T>
//  T& get() {
//    if (std::type_index(typeid(T)) == std::type_index(content_->type_info())) {
//      return static_cast<Holder<T>&>(*content_).value_;
//    }
//    throw std::bad_cast();
//  }
//
// private:
//  struct Placeholder {
//    virtual ~Placeholder() = default;
//    virtual const std::type_info& type_info() const = 0;
//    virtual std::unique_ptr<Placeholder> clone() = 0;
//  };
//
//  template <typename T>
//  struct Holder : public Placeholder {
//    template <typename U>
//    explicit Holder(U&& value) : value_(std::forward<U>(value)) {}
//    const std::type_info& type_info() const override { return typeid(T); }
//    std::unique_ptr<Placeholder> clone() override {
//      return std::make_unique<Holder<T>>(value_);
//    }
//    T value_;
//  };
//
//  std::unique_ptr<Placeholder> content_;
//};
//
//
//namespace detail {
//
//void
//collect_any_vector(std::vector<Any>&)
//{ }
//
//template<typename Head, typename... Tail>
//void collect_any_vector(std::vector<Any>& vector, Head&& head, Tail&&... tail) {
//  vector.push_back(std::forward<Head>(head));
//  collect_any_vector(vector, std::forward<Tail>(tail)...);
//}
//} // namespace detail
//
//class AnyScheduler {
// public:
//
//  template
//    < typename S
//    , typename = std::enable_if_t
//        < std::is_base_of
//           < SchedulerBase
//           , std::decay_t<S>
//           >::value
//        >
//    >
//  AnyScheduler(S&& scheduler) /* implicit */
//    : content_
//      ( make_holder
//          ( std::forward<S>(scheduler)
//          , &std::remove_reference_t<S>::execute
//          )
//      )
//    {}
//
//  AnyScheduler(const AnyScheduler& other)
//  : content_(other.content_->clone())
//  { }
//
//  AnyScheduler(AnyScheduler&& other) noexcept {
//    swap(other);
//  }
//
//  AnyScheduler&
//  operator=(AnyScheduler other) {
//    swap(other);
//    return *this;
//  }
//
//  void
//  swap(AnyScheduler& other) noexcept {
//    content_.swap(other.content_);
//  }
//
//  ~AnyScheduler() = default;
//
//  template<typename... Args>
//  void execute(Args&&... arguments) {
//    std::vector<Any> any_arguments;
//    // replace collect_any_vector with fold expression in C++17.
//    detail::collect_any_vector(any_arguments, std::forward<Args>(arguments)...);
//    return content_->invoke_execute(std::move(any_arguments));
//  }
//
//  const std::string& name() const noexcept {
//    return content_->name();
//  }
//
//  template<typename S>
//  S& get() {
//    if (std::type_index(typeid(S)) == std::type_index(content_->type_info())) {
//      return static_cast<Holder<S>&>(*content_).value_;
//    }
//    throw std::bad_cast();
//  }
//
// private:
//
//  struct Placeholder {
//    virtual ~Placeholder() = default;
//    virtual const std::type_info& type_info() const = 0;
//    virtual std::unique_ptr<Placeholder> clone() = 0;
//    virtual const std::string& name() const noexcept = 0;
//    virtual void invoke_execute(std::vector<Any>&& arguments) = 0;
//  };
//
//  template<typename S, typename... Args>
//  struct Holder
//      : public Placeholder {
//
//    template<typename Q>
//    explicit Holder(Q&& scheduler)
//      : scheduler_(std::forward<Q>(scheduler)) { }
//
//    const std::type_info&
//    type_info() const override {
//      return typeid(S);
//    }
//
//    std::unique_ptr<Placeholder> clone() override {
//      return std::make_unique<Holder<S, Args...>>(scheduler_);
//    }
//
//    const std::string& name() const noexcept override {
//      return scheduler_.name();
//    }
//
//    void invoke_execute
//      (std::vector<Any>&& arguments) override {
//      assert(arguments.size() == sizeof...(Args));
//      invoke_execute(std::move(arguments), std::make_index_sequence<sizeof...(Args)>());
//    }
//
//    template<size_t... Is>
//    void invoke_execute(std::vector<Any>&& arguments, std::index_sequence<Is...>) {
//      // Expand the index sequence to access each `Any` stored in `arguments`,
//      // and cast to the type expected at each index. Also note we move each
//      // value out of the `Any`.
//      return scheduler_.execute(std::move(arguments[Is].get<Args>())...);
//    }
//
//    S scheduler_;
//  };
//
//  template<typename S, typename... Args>
//  std::unique_ptr<Placeholder>
//  make_holder
//    ( S&& scheduler
//    , void(std::remove_reference_t<S>::*)(Args...)
//    ) {
//    return std::make_unique<Holder<S, Args...>>(std::forward<S>(scheduler));
//  }
//
//  std::unique_ptr<Placeholder> content_;
//};


//class Scheduler {
// public:
//
//  explicit Scheduler(AnyScheduler scheduler)
//  : scheduler_(std::move(scheduler))
//  { }
//
//  template<typename... Args>
//  void
//  execute(Args&&... args) {
//    std::cout << scheduler_.name() << " is working ... " << std::endl;
//    scheduler_.execute(std::forward<Args>(args)...);
//  }
//
// private:
//  AnyScheduler scheduler_;
//};
//
//
//
//template <typename State>
//class AnyScheduler {
//public:
// // Disable the constructor for `Any`, otherwise trying to copy a
// // non-const `Any` actually constructs an `Any` containing an `Any`.
//template
//  < typename SchedulerType
//  , typename = std::enable_if_t
//      <!std::is_same<AnyScheduler, std::decay_t<SchedulerType>>::value>
//  >
//AnyScheduler(SchedulerType&& value) /* implicit */
//: content_
//  ( std::make_unique<Holder<SchedulerType>>
//     (std::forward<SchedulerType>(value))
//  )
//{}
//
//
//// We use Scott Meyer's copy-and-swap idiom to implement special member functions.
//
//// `clone()` gives us a way to copy-construct a value through a type-agnostic virtual interface.
//AnyScheduler
//  (const AnyScheduler& other)
//: content_
//  (other.content_->clone())
//{}
//
//AnyScheduler
//  (AnyScheduler&& other) noexcept
//{
//  swap(other);
//}
//
//template <typename OtherState>
//AnyScheduler
//  (AnyScheduler<OtherState> const & other)
//: content_
//
//~AnyScheduler() = default;
//
//AnyScheduler& operator=(AnyScheduler other) {
//  swap(other);
//  return *this;
//}
//
//void swap(AnyScheduler& other) noexcept
//{
//  content_.swap(other.content_);
//}
//
//template <typename SchedulerType>
//SchedulerType& get()
//{
//  if ( std::type_index(typeid(SchedulerType))
//        == std::type_index(content_->type_info())
//     )
//  {
//    return static_cast<Holder<SchedulerType>&>(*content_).value_;
//  }
//  throw std::bad_cast();
//}
//
//private:
//struct Placeholder {
//  virtual ~Placeholder() = default;
//  virtual const std::type_info& type_info() const = 0;
//  virtual std::unique_ptr<Placeholder> clone() = 0;
//};
//
//template <typename SchedulerType>
//struct Holder
//    : public Placeholder
//{
//  template <typename U>
//  explicit Holder(U&& value)
//  : value_(std::forward<U>(value))
//  {}
//
//  const std::type_info&
//  type_info() const override
//  {
//    return typeid(SchedulerType);
//  }
//
//  std::unique_ptr<Placeholder> clone() override
//  {
//    return std::make_unique<Holder<SchedulerType>>(value_);
//  }
//
//  SchedulerType value_;
//};
//
//template
//  < typename OrigStateType
//  , typename CastStateType
//  , template<typename State> class SchedulerType
//  >
//struct CastHolder
//    : public Placeholder
//{
//  template <typename U>
//  explicit Holder(U&& value)
//  : value_(std::forward<U>(value))
//  {}
//
//  const std::type_info&
//  type_info() const override
//  {
//    return typeid(SchedulerType<OrigStateType>);
//  }
//
//  std::unique_ptr<Placeholder>
//  clone() override
//  {
//    return std::make_unique
//        < Holder
//          < SchedulerType<CastStateType> >
//        >(value_);
//  }
//
//  SchedulerType<OrigStateType> value_;
//};
//
//std::unique_ptr<Placeholder> content_;
//};
//
//
//template <typename State, template<typename State> class Schedule>
//class Scheduler {
//
//public:
//
//    explicit Scheduler
//      ( AnyScheduler<State> scheduler )
//      : _scheduler(std::move(scheduler))
//    { }
//
//    template
//      <template<typename OtherState> class Schedule>
//    Scheduler
//    ( Scheduler<Schedule<OtherState> > const & other )
//    : _scheduler()
//    {
//
//
//
//    }
//
//private:
//
//    AnyScheduler<State> _scheduler;
//
//};

////  //// Any device scheduler implementation needs to
////  //// be derived
////  //// from SchedulerBase
//class SchedulerRuntime {
//  public:
//
//  explicit SchedulerRuntime(std::string name)
//  : name_(std::move(name))
//  { }
//
//  virtual ~SchedulerRuntime() = default;
//
//  const std::string& name() const noexcept
//  { return name_; }
//
//  // no virtual execute() method!
//
//   protected:
//    std::string name_;
//};
//
//template <typename State>
//class Scheduler {
//
//public:
//
//    virtual void
//    execute
//      (Schedule<State> & schedule) = 0;
//};
//
//
//template <typename State>
//class SchedulerFactory {
//
//public:
//    virtual std::unique_ptr<Scheduler<State>>
//    construct(SchedulerRuntime const & ) = 0;
//
//};


// This is going to be part of the device implementation
template <typename State>
class Scheduler : public SchedulerInterface<State>{

public:

    Scheduler() = delete;

    template <typename OtherState>
    Scheduler
      (SchedulerInterface<OtherState> const & scheduler)
    : _pScheduler(make_copy(&scheduler))
    {}

    Scheduler
      (Scheduler const & scheduler)
    : _pScheduler(make_copy(scheduler._pScheduler.get()))
    {}

    template <typename OtherState>
    Scheduler
      (Scheduler<OtherState> const & scheduler)
    : _pScheduler(make_copy(scheduler._pScheduler.get()))
    {}

    Type
    device_type() const override
    {
      return _pScheduler->device_type();
    }

    void
    execute
      (schedule::Schedule<State> & schedule) override
    {
      return _pScheduler->execute(schedule);
    }

private:

    template <typename OtherState>
    std::unique_ptr<SchedulerInterface<State>>
    make_copy
      (SchedulerInterface<OtherState> const * const pScheduler )
    {
      switch (pScheduler->device_type())
        {
          case Type::NUMA:
            return std::unique_ptr<SchedulerInterface<State>>
                ( new numa::Scheduler<OtherState>
                  ( dynamic_cast<const numa::Scheduler<OtherState>&>
                   (*pScheduler)
                  )
                );
        }

        throw std::runtime_error (CODE_ORIGIN + "failed");
    }

    std::unique_ptr<SchedulerInterface<State>> _pScheduler;

};

}
}

#endif /* DEVICE_SCHEDULER_HPP_ */
