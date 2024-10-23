#ifndef VECTOR_H
#define VECTOR_H

#include <iostream>
#include <algorithm>
#include <stdexcept>

using namespace std;

template<typename Type>
class Vector {
 public:
  class ConstIterator;
  class Iterator;
  using value_type = Type;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using iterator = Vector :: Iterator;
  using const_iterator = Vector::ConstIterator;

 private:
  size_type sz; /* The number of elements stored in the Vector */
  size_type max_sz; /* Capacity of the Vector */
  value_type* values; /* data pointer */

  /*
   * Default vector capacity. Also used as the increment for allocated space
   * when adding new entries
   */
  static constexpr size_type min_capacity = 5;

 public:
  /*
   * General constructor. It creates an empty vector with reserved space for n
   * elements
   */
  Vector(size_type n)
      :
      sz{0},
      max_sz{n},
      values{nullptr} {
    /* Use 'reserve' method as a general feature to allocate memory for data */
    reserve(max_sz);
  }

  /* Constructors with the initializer list */
  Vector(std::initializer_list<value_type> init_list)
      :
      Vector(std::max(init_list.size(), min_capacity)) {

    /* Copy initializer list to the array of valuse */
    std::copy(init_list.begin(), init_list.end(), values);
    sz = init_list.size();
  }

  /*
   * Default constructor [no parameters]. It creates an empty vector with
   * default reserved space
   */
  Vector() : Vector(min_capacity) {
  }

  /* Copy constructor */
  Vector(const Vector &other)
        :
        Vector(other.size()) {
    /* Cleanup the current content */
    clear();

    /* Populate the vector with the contents of the copied one */
    for (size_type idx{0}; idx < other.size(); idx++)
      push_back(other[idx]);
  }

/* Destructor */
  ~Vector() {
    /* Just for paranoia */
    if (!values)
      return;

    /* Free dynamic memory */
    delete[] values;
    values = nullptr;
  }

  /* The this object takes a copy of the content of the parameter */
  Vector& operator=(const Vector& other) {
    if (this == &other)
      return *this;

    /* Cleanup the current content*/
    clear();

    /*
     * Populate vector. We do not inherit capacity from the copied object. The
     * capacity will be determined automatically when using 'push_back' to
     * populate vector
     */
    for (size_type idx{0}; idx < other.size(); idx++)
      push_back(other[idx]);

    return *this;
  }

  size_type size() const {
    return sz;
  }

  bool empty() const {
    return (sz == 0);
  }

  /* Remove all the elements of the vector container, thus making it size 0 */
  void clear() {
    sz = 0;
  }

  void reserve(size_type n) {
    /* Do nothing if 0 size given */
    if (!n)
      return;

    /* Do nothing if memory is already allocated and there is enough space */
    if (values && n <= max_sz)
      return;

    max_sz = n;

    /*
     * Allocate memory for array of 'max_sz' elements of type 'value_type' in
     * our vector and keep pointer to this array:
     *   [0][1][2]...[max_sz-1] - memory layout
     *    |__ pointer to our array [must bu assigned to values]
     */
    auto new_values = new value_type[max_sz];

    /*
     * If the memory is already allocated, ensure the current content to be
     * saved. Copy previous data to the new location and release the previously
     * allocated region
     */
    if (values) {
      for (size_type idx{0}; idx < sz; idx++)
        new_values[idx] = values[idx];

      delete[] values;
    }

    /* Assign new alocated memory as main one */
    values = new_values;
  }

 /*
  * Reduce the vector capacity to the required minimum [number of currently
  * stored entries]
  */
  void shrink_to_fit() {
    /* Make copy of the current vector */
    auto back = *this;

    /* Cleanup all entries and free memory */
    if (values) {
      delete[] values;
      values = nullptr;
    }

    sz = 0;
    max_sz = 0;

    /* Allocate the required capacity and fill it with saved data */
    reserve(back.size());
    for (size_type idx{0}; idx < back.size(); idx++)
      push_back(back[idx]);
  }

  void push_back(value_type x) {
    if (!values || sz >= max_sz)
      /*
       * Allocate more space than required to add a new element. To reduce the
       * number of memory relocations
       */
      reserve(sz + min_capacity);

    values[sz++] = x;
  }

  /*
   * Removes the last element from the Vector. Throws an exception of type
   * std::runtime_error if the vector was empty.
   */
  void pop_back() {
    if (empty())
      throw runtime_error("error, vector is empty");

    sz--;
  }

  /* Returns a reference to the element at the specified position */
  reference operator[](size_type index) {
    if (index >= sz)
      throw runtime_error("index is not within the permitted range");

    return values[index];
  }

  const_reference operator[](size_type index) const {
    if (index >= sz)
      throw runtime_error("index is not within the permitted range");

    return values[index];
  }

  /* Returns the current capacity of the vector */
  size_type capacity() const {
    return max_sz;
  }

  /*
   * Output vector to ostream in the following view:
   *   [element1, element2, element3]
   */
  friend ostream& operator<<(ostream& o, const Vector& v) {
    o << "[";
    auto sep = "";
    for (auto idx{0}; idx < v.size(); idx++) {
      o << sep << v[idx];
      sep = ", ";
    }

    o << "]";
    return o;
  }

  /*
   * Returns an iterator that refers to the first element in the Vector, or is
   * equal to the end iterator if the Vector is empty.
   */
  iterator begin() {
    if (empty())
      return end();

    return Iterator(values);
  }

  /*
   * Returns an iterator that refers to the virtual element after the last
   * element contained in the vector.
   */
  iterator end() {
    return Iterator(values + sz);
  }

  const_iterator begin() const {
    return ConstIterator(values);
  }

  const_iterator end() const{
    return ConstIterator(values + sz);
  }

  iterator insert(const_iterator pos, const_reference val) {
    auto diff = pos - begin();
    if (diff < 0 || static_cast<size_type>(diff) > sz)
      throw std::runtime_error("Iterator out of bounds");

    size_type current{static_cast<size_type>(diff)};
    if (sz >= max_sz)
      /*
       * Double the available space or allocate min_capacity cells if space has
       * not been allocated yet
       */
      reserve (max_sz ? max_sz * 2 : min_capacity);

    for (auto i{sz}; i-- > current;)
      values[i + 1] = values[i];

    values[current] = val;
    ++sz;
    return iterator{values + current };
  }

  iterator erase(const_iterator pos) {
    auto diff = pos - begin ();
    if (diff < 0 || static_cast<size_type>(diff) >= sz)
      throw std::runtime_error("Iterator out of bounds");

    size_type current{static_cast<size_type>(diff)};
    for (auto i{current}; i < sz-1; ++i)
      values[i] = values[i + 1];

    --sz;
    return iterator{values+current};
  }

  class Iterator {
    public :
      using value_type = Vector::value_type;
      using reference = Vector::reference;
      using pointer = Vector::pointer;
      using difference_type = Vector::difference_type;
      using iterator_category = std::forward_iterator_tag;

    private :
      /* Instance variables */
      pointer ptr;

      /* For seured iterators */
      // pointer end;

    public : /* Member Functions */
      Iterator () : ptr{nullptr} {}

      Iterator(pointer ptr) : ptr{ptr} {}

      reference operator*() const {
        return *ptr;
      }

      /* Returns a pointer to the value referenced by the iterator */
      pointer operator->() const {
        return ptr;
      }

      /*
       * Compares whether the two pointers are the same. (A global function
       * might be a better choice.)
       */
      bool operator==(const const_iterator& other) const {
        return this->ptr == other.ptr;
      }

      bool operator!=(const const_iterator& other) const {
        return this->ptr != other.ptr;
      }

      /*
       * (Prefix) Iterator is switched to the next element in the Vector. The
       * method returns a reference to the changed iterator.
       */
      iterator& operator++() {
        /* In case of secured iterators uncomment this */
        // if(ptr==end)
        //   return *this;

        ++ptr;
        return *this;
      }

      /*
       * (Postfix) Iterator is switched to the next element in Vector. A copy of
       * the original iterator is returned.
       */
      iterator operator++(int) {
        Iterator res = *this;
        ++ptr;
        return res;
      }

      /* (type conversion) Allows the conversion of Iterator to ConstIterator */
      operator const_iterator() const {
        return ConstIterator(ptr);
      }
    };

  class ConstIterator {
    friend class Iterator;
    public :
      using value_type = Vector::value_type;
      using reference = Vector::const_reference;
      using pointer = Vector::const_pointer;
      using difference_type = Vector::difference_type;
      using iterator_category = std::forward_iterator_tag;

    private :
      /* Instance variables */
      pointer ptr;

    public :
      /* Member Functions */
      ConstIterator() : ptr{nullptr} {}
      ConstIterator(pointer ptr) : ptr{ptr} {}

      reference operator*() const {
        return *ptr;
      }

      pointer operator->() const{
        return ptr;
      }

      bool operator==(const const_iterator& other) const {
        return ptr == other.ptr;
      }

      bool operator!=(const const_iterator& other) const {
        return ptr != other.ptr;
      }

      const_iterator& operator++() {
        ++ptr;
        return *this;
      }

      const_iterator operator++(int) {
        ConstIterator res = *this;
        ++ptr;
        return res;
      }

      friend Vector::difference_type
      operator-(const Vector::ConstIterator& lop,
                const Vector::ConstIterator& rop) {
        return lop.ptr - rop.ptr;
      }
  };
};

#endif
