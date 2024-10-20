#ifndef VECTOR_H
#define VECTOR_H

#include <iostream>
#include <algorithm>
#include <stdexcept>

using namespace std;

class Vector {
 public:
  class ConstIterator;
  class Iterator;
  using value_type = double;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
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

  }

  /*
   * Returns an iterator that refers to the virtual element after the last
   * element contained in the vector.
   */
  iterator end() {

  }

  // const_iterator begin() const {

  // }

  // const_iterator end() const{

  // }

  class Iterator {
    public :
      using value_type = Vector::value_type;
      using reference = Vector::reference;
      using pointer = Vector::pointer;
      using difference_type = Vector::difference_type;
      using iterator_category = std::forward_iterator_tag;

    private :
      //Instance variables
      pointer ptr;

    public : //Member Functions
      Iterator () : ptr{nullptr} {}

      // Parameter list
      Iterator(pointer ptr) : ptr{ptr} {}

      reference operator*() const {
        return *ptr;
      }

    /* Returns a pointer to the value referenced by the iterator */
      pointer operator->() {
        return ptr;
      }

      bool operator==(const const_iterator&) const {

      }
    };

  class ConstIterator {
    public :
      using value_type = Vector::value_type;
      using reference = Vector::const_reference;
      using pointer = Vector::const_pointer;
      using difference_type = Vector::difference_type;
      using iterator_category = std::forward_iterator_tag;

    private :
      //Instance variables

    public :
      //Member Functions
  };
};

#endif
