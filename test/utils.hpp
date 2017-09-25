#ifndef UTILS_HPP
#define UTILS_HPP

/* This option is meant for light-weight objects that can be allocated on
 * the stack.
 */
template <typename T>
class Option {
private:
  bool is_some_;
  T obj;

public:
  Option () : is_some_(false), obj(T()) {}

  Option(const T & t) : is_some_(true), obj(t) {
  }

  Option<T> & operator=(const Option<T>& o){
    is_some_ = o.is_some_;
    obj = o.obj;
    return *this;
  }

  bool is_some() const {
    return is_some_;
  }

  const T & get() const {
    if (!is_some_) {
      throw std::exception();
    }

    return obj;
  }
};

#endif
