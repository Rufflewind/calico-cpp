#ifndef FZ_ITERATOR_HH
#define FZ_ITERATOR_HH
#include "cxx11.hh"
namespace fz {

/// An iterator adapter that automatically counts the offset as the iterator
/// moves.
template<class Iterator>
class counted_iterator
    : public std::iterator<
          typename std::iterator_traits<Iterator>::iterator_category,
          typename std::iterator_traits<Iterator>::value_type,
          typename std::iterator_traits<Iterator>::difference_type,
          typename std::iterator_traits<Iterator>::pointer,
          typename std::iterator_traits<Iterator>::reference> {
public:
    typedef std::iterator<
        typename std::iterator_traits<Iterator>::iterator_category,
        typename std::iterator_traits<Iterator>::value_type,
        typename std::iterator_traits<Iterator>::difference_type,
        typename std::iterator_traits<Iterator>::pointer,
        typename std::iterator_traits<Iterator>::reference> base_type;
    typedef typename base_type::value_type value_type;
    typedef typename base_type::difference_type difference_type;
    typedef typename base_type::pointer pointer;
    typedef typename base_type::reference reference;
    typedef counted_iterator this_type;
    typedef Iterator iterator_type;
    counted_iterator() {}
    counted_iterator(iterator_type i,
                     difference_type count = difference_type())
        : _i(i),
          _count(count) {}
    template<class OtherIterator>
    counted_iterator(const counted_iterator<OtherIterator>& other)
        : _i(other._i),
          _count(other._count) {}
    template<class OtherIterator>
    this_type& operator=(const counted_iterator<OtherIterator>& other) {
        _i = other._i;
        _count = other._count;
    }
    iterator_type& base() const {
        return _i;
    }
    reference operator*() const {
        return *_i;
    }
    pointer operator->() const {
        return &**this;
    }
    this_type& operator++() {
        ++_i;
        ++_count;
        return *this;
    }
    this_type& operator--() {
        --_i;
        --_count;
        return *this;
    }
    this_type operator++(int) {
        return this_type(_i++, _count++);
    }
    this_type operator--(int) {
        return this_type(_i--, _count--);
    }
    this_type operator+(difference_type n) const {
        return this_type(_i + n, _count + n);
    }
    this_type operator-(difference_type n) const {
        return this_type(_i - n, _count - n);
    }
    this_type& operator+=(difference_type n) const {
        _i += n;
        _count += n;
        return *this;
    }
    this_type& operator-=(difference_type n) const {
        _i -= n;
        _count -= n;
        return *this;
    }
    difference_type count() const {
        return _count;
    }
    void set_count(difference_type value) {
        _count = value;
    }
    template<class OtherIterator>
    bool operator==(const counted_iterator<OtherIterator>& other) const {
        return _i == other._i;
    }
    template<class OtherIterator>
    bool operator!=(const counted_iterator<OtherIterator>& other) const {
        return _i != other._i;
    }
    template<class OtherIterator>
    bool operator<=(const counted_iterator<OtherIterator>& other) const {
        return _i <= other._i;
    }
    template<class OtherIterator>
    bool operator>=(const counted_iterator<OtherIterator>& other) const {
        return _i >= other._i;
    }
    template<class OtherIterator>
    bool operator<(const counted_iterator<OtherIterator>& other) const {
        return _i < other._i;
    }
    template<class OtherIterator>
    bool operator>(const counted_iterator<OtherIterator>& other) const {
        return _i > other._i;
    }
    friend this_type operator+(difference_type n, const this_type& i) {
        return i + n;
    }
#ifdef HAVE_TRAILING_RETURN
    template<class OtherIterator>
    auto operator-(const counted_iterator<OtherIterator>& i)
      -> decltype(_i - i._i) {
        return _i - i._i;
    }
#else
    difference_type operator-(const this_type& i) {
        return _i - i._i;
    }
#endif
private:
    Iterator _i;
    difference_type _count;
};

/// Convenience function to construct a `counted_iterator`.
template<class Iterator>
counted_iterator<Iterator> iterator_counter(
    Iterator i,
    typename counted_iterator<Iterator>::difference_type count
        = typename counted_iterator<Iterator>::difference_type()
) {
    return counted_iterator<Iterator>(i, count);
}

}
#endif
