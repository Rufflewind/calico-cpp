
/// X
template<class Applicable, class Tuple>
tuple_foldl(Applicable f, Tuple&& t) {
    return tuple_foldl_with_index(
        wrap(f),
        std::forward<T>(x),
        std::forward<Tuple>(t)
    );
}
template<class Applicable, class Tuple>
tuple_foldl1(Applicable f, Tuple&& t) {
    return tuple_foldl_with_index(
        wrap(f),
        std::forward<T>(x),
        std::forward<Tuple>(t)
    );
}
/// X
template<class Applicable, class Tuple>
tuple_foldl_with_index(Applicable f, T&& x, Tuple&& t) {
    typedef typename std::remove_reference<Tuple>::type tuple_type;
    typedef typename std::remove_reference<Tuple>::type Tuple_;
    return _tuple_foldl<..., std::tuple_size<tuple_type>::value>::apply(
        f,
        std::forward<T>(x),
        std::forward<Tuple>(t)
    );
}

template<std::size_t N1, std::size_t I = 0>
struct _tuple_foldl {
    template<class Func, class T, class Tuple>
    static void apply(Func f, T&& x, Tuple&& t) {
        return _tuple_foldl<N1, I + 1>::apply(
            f,
            f(std::forward<T>(x), I, std::get<I>(std::forward<Tuple>(t))),
            std::forward<Tuple>(t)
        );
    }
};
template<std::size_t N1>
struct _tuple_foldl<T, N1, N1> {
    template<class Func, class T, class Tuple>
    static void apply(T& x) {
        return _tuple_foldl<N1, I + 1>::apply(
            f(std::forward<T>(x), I, std::forward<Tuple>(t)),
            std::forward<Tuple>(t)
        );
    }
};
