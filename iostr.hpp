// -*- mode: c++; coding: utf-8 -*-
#pragma once
#ifndef WTL_IOSTR_HPP_
#define WTL_IOSTR_HPP_

#include <cstring>
#include <cerrno>
#include <cstdarg>
#include <ctime>

#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iterator>

#include <string>
#include <vector>
#include <valarray>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <algorithm>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
// prior declaration

namespace wtl {

struct identity {
    template<class T>
    constexpr T operator()(T&& x) const noexcept {
        return std::forward<T>(x);
    }
};

namespace detail {
    template <class Iter, class Char, class Func> class JoinHelper;
}

template <class Iter, class Char, class Func> extern std::ostream&
operator<<(std::ostream& ost, wtl::detail::JoinHelper<Iter, Char, Func>&& j);

namespace detail {
    template <class Iter, class Char, class Func>
    class JoinHelper {
      public:
        JoinHelper(Iter b, Iter e, const Char* d, Func f):
            begin(b), end(e), delim(d), func(f) {}
      private:
        Iter begin;
        const Iter end;
        const Char* delim;
        Func func;

        friend std::ostream& operator<< <Iter, Char, Func>
        (std::ostream& ost, wtl::detail::JoinHelper<Iter, Char, Func>&& j);
    };
}  // namespace detail

template <class Iter, class Char=char, class Func=identity> extern
detail::JoinHelper<Iter, Char, Func> join(Iter begin, const Iter end, const Char* delim="\t", Func func=Func());

}  // namespace wtl

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
// global operator<< for containers

namespace std{

template <class T> inline
std::ostream& operator<< (std::ostream& ost, const std::valarray<T>& v) {
    return ost << '[' << wtl::join(std::begin(v), std::end(v), ", ") << ']';
}

template <class T> inline
std::ostream& operator<< (std::ostream& ost, const std::vector<T>& v) {
    return ost << '[' << wtl::join(std::begin(v), std::end(v), ", ") << ']';
}

template <> inline
std::ostream& operator<< (std::ostream& ost, const std::vector<std::string>& v) {
    if (v.empty()) {return ost << "[]";}
    return ost << "['" << wtl::join(v.begin(), v.end(), "', '") << "']";
}

template <class T> inline
std::ostream& operator<< (std::ostream& ost, const std::set<T>& v) {
    return ost << "set([" << wtl::join(v.begin(), v.end(), ", ") << "])";
}

template <class T, class Pred> inline
std::ostream& operator<< (std::ostream& ost, const std::unordered_set<T, std::hash<T>, Pred>& v) {
    return ost << "set([" << wtl::join(v.begin(), v.end(), ", ") << "])";
}

template <class F, class S> inline
std::ostream& operator<< (std::ostream& ost, const std::pair<F, S>& p) {
    return ost << '['  << std::get<0>(p)
               << ", " << std::get<1>(p) << ']';
}

namespace wtl { namespace detail {
    template <class Map> inline
    std::ostream& operator_ost_map(std::ostream& ost, const Map& m) {
        ost << '{';
        if (!m.empty()) {
            auto it(begin(m));
            ost << std::get<0>(*it) << ": " << std::get<1>(*it);
            while (++it != end(m)) {
                ost << ", " << std::get<0>(*it) << ": " << std::get<1>(*it);
            }
        }
        return ost << '}';
    }
}} // namespace

// map
template <class Key, class T, class Comp> inline
std::ostream& operator<< (std::ostream& ost, const std::map<Key, T, Comp>& m) {
    return wtl::detail::operator_ost_map(ost, m);
}

// multimap
template <class Key, class T, class Comp> inline
std::ostream& operator<< (std::ostream& ost, const std::multimap<Key, T, Comp>& m) {
    return wtl::detail::operator_ost_map(ost, m);
}

// unordered_map
template <class Key, class T, class Hash> inline
std::ostream& operator<< (std::ostream& ost, const std::unordered_map<Key, T, Hash>& m) {
    return wtl::detail::operator_ost_map(ost, m);
}

}  // namespace std

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

template <class Iter, class Char, class Func> inline std::ostream&
operator<<(std::ostream& ost, wtl::detail::JoinHelper<Iter, Char, Func>&& x) {
    if (x.begin == x.end) return ost;
    ost << x.func(*x.begin);
    while (++x.begin != x.end) {ost << x.delim << x.func(*x.begin);}
    return ost;
}

template <class Iter, class Char, class Func>
detail::JoinHelper<Iter, Char, Func>
join(Iter begin, const Iter end, const Char* delim, Func func) {
    return detail::JoinHelper<Iter, Char, Func>(begin, end, delim, func);
}
template <class T, class Char=char, class Func=identity>
detail::JoinHelper<typename T::const_iterator, Char, Func>
join(const T& v, const Char* delim="\t", Func func=Func()) {
    return detail::JoinHelper<typename T::const_iterator, Char, Func>
        (std::begin(v), std::end(v), delim, func);
}

inline std::ostringstream
make_oss(const unsigned int precision=std::cout.precision(),
         const std::ios_base::fmtflags fmtfl=std::cout.flags()) {
    std::ostringstream oss;
    oss.precision(precision);
    oss.setf(fmtfl);
    return oss;
}

template <class Iter, class Func=identity> inline
std::string str_join(Iter begin_, Iter end_, const char* sep=",",
                     std::ostringstream&& oss=make_oss(), Func func=Func()) {
    oss << join(begin_, end_, sep, func);
    return oss.str();
}

template <class T, class Func=identity> inline
std::string str_join(const T& v, const char* sep=",",
                     std::ostringstream&& oss=make_oss(), Func func=Func()) {
    return str_join(begin(v), end(v), sep, std::move(oss), func);
}

template <class T, class Func=identity> inline
std::string str_matrix(const T& m, const char* sep=",",
                       std::ostringstream&& oss=make_oss(), Func func=Func()) {
    for (const auto& row: m) {
        oss << join(row, sep, func) << '\n';
    }
    return oss.str();
}

template <int N, class Iter> inline
std::string str_pairs(Iter begin_, const Iter end_, const char* sep=",",
                      std::ostringstream&& oss=make_oss()) {
    return str_join(begin_, end_, sep, std::move(oss),
        [](const typename Iter::value_type& p) {return std::get<N>(p);});
}
template <int N, class Map> inline
std::string str_pairs(const Map& m, const char* sep=",",
                      std::ostringstream&& oss=make_oss()) {
    return str_pairs<N>(begin(m), end(m), sep, std::move(oss));
}

template <class Map> inline
std::string str_pair_rows(const Map& m, const char* sep=",",
                          std::ostringstream&& oss=make_oss()) {
    auto oss1 = make_oss(oss.precision(), oss.flags());
    std::string s = str_pairs<0>(m, sep, std::move(oss));
    s += '\n';
    s += str_pairs<1>(m, sep, std::move(oss1));
    return s += '\n';
}

template <class Map> inline
std::string str_pair_cols(const Map& m, const char* sep=",",
                          std::ostringstream&& oss=make_oss()) {
    for (const auto& p: m) {
        oss << std::get<0>(p) << sep << std::get<1>(p) << '\n';
    }
    return oss.str();
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
// std::string manipulation

inline std::vector<std::string> split_algorithm(const std::string& src, const std::string& delimiter=" \t\n") {
    std::vector<std::string> dst;
    for (auto delim_pos=begin(src), start=delim_pos; delim_pos!=end(src); ) {
        delim_pos = std::find_first_of(start, end(src), begin(delimiter), end(delimiter));
        dst.push_back(std::string(start, delim_pos));
        ++(start = delim_pos);
    }
    return dst;
}

inline std::vector<std::string>
split(const std::string& src, const std::string& delimiter=" \t\n") {
    std::vector<std::string> dst;
    size_t start = 0, offset = 0;
    while (true) {
        offset = src.find_first_of(delimiter, start);
        offset -= start;
        dst.push_back(src.substr(start, offset));
        start += offset;
        if (start == src.npos) break;
        ++start;
    }
    return dst;
}

inline std::string rstrip(std::string src, const std::string& chars=" ") {
    size_t pos(src.find_last_not_of(chars));
    if (pos == std::string::npos) {
        src.clear();
    } else {
        src.erase(++pos);
    }
    return src;
}

inline std::string lstrip(std::string src, const std::string& chars=" ") {
    const size_t pos(src.find_first_not_of(chars));
    if (pos == std::string::npos) {
        src.clear();
    } else {
        src.erase(0, pos);
    }
    return src;
}

inline std::string strip(std::string src, const std::string& chars=" ") {
    return rstrip(lstrip(src, chars), chars);
}

inline bool startswith(const std::string& str, const std::string& prefix) {
    return (str.size() >= prefix.size()) &&
           (str.compare(0, prefix.size(), prefix) == 0);
}

inline bool endswith(const std::string& str, const std::string& suffix) {
    size_t str_size = str.size();
    return (str_size >= suffix.size()) &&
           (str.compare(str_size -= suffix.size(), suffix.size(), suffix) == 0);
}

inline std::string replace_all(const std::string& patt, const std::string& repl, const std::string& src) {
    std::string result;
    std::string::size_type pos_before(0);
    std::string::size_type pos(0);
    std::string::size_type len(patt.size());
    while ((pos = src.find(patt, pos)) != std::string::npos) {
        result.append(src, pos_before, pos-pos_before);
        result.append(repl);
        pos += len;
        pos_before = pos;
    }
    result.append(src, pos_before, src.size()-pos_before);
    return result;
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

inline std::string strprintf(const char* const format, ...) {
    va_list args;
    std::string buffer;
    va_start(args, format);
    const int length = std::vsnprintf(nullptr, 0, format, args) ;
    va_end(args);
    if (length < 0) throw std::runtime_error(format);
    buffer.resize(length + 1);
    va_start(args, format);
    const int result = std::vsnprintf(&buffer[0], length + 1, format, args);
    va_end(args);
    if (result < 0) throw std::runtime_error(format);
    buffer.pop_back();
    return buffer;
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

// default is the same as ctime(): Thu Aug 23 14:55:02 2001
// which is equivalent to "%a %b %d %T %Y"
inline std::string strftime(const std::string& format="%c") {
    char cstr[80];
    const time_t rawtime = time(nullptr);
    const struct tm* t = localtime(&rawtime);
    std::strftime(cstr, sizeof(cstr), format.c_str(), t);
    return std::string(cstr);
}
inline std::string iso8601date(const std::string& sep="-") {
    std::ostringstream oss;
    oss << "%Y" << sep << "%m" << sep << "%d";
    return strftime(oss.str());
}
inline std::string iso8601time(const std::string& sep=":") {
    std::ostringstream oss;
    oss << "%H" << sep << "%M" << sep << "%S";
    return strftime(oss.str());
}
inline std::string iso8601datetime() {return strftime("%FT%T%z");}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
// Pythonic fstream wrapper with exceptions and binary mode

class ipfstream: public std::ifstream {
  public:
    explicit ipfstream(const std::string& filepath,
                       const std::ios::openmode mode=std::ios::in)
        : std::ifstream(filepath, mode | std::ios::binary) {
        exceptions(std::ios::failbit | std::ios::badbit);
    }

    std::string readline(const char delimiter='\n') {
        std::string buffer;
        std::getline(*this, buffer, delimiter);
        return buffer;
    }

    std::vector<std::string> readlines(const char delimiter='\n') {
        std::vector<std::string> lines;
        std::string buffer;
        while (std::getline(*this, buffer, delimiter)) {
            lines.push_back(buffer);
        }
        return lines;
    }

    std::string read(const char delimiter='\0') {return readline(delimiter);}
};

class opfstream: public std::ofstream {
  public:
    explicit opfstream(const std::string& filepath,
                       const std::ios::openmode mode=std::ios::out)
        : std::ofstream(filepath, mode | std::ios::binary) {
        exceptions(std::ios::failbit | std::ios::badbit);
        precision(std::cout.precision());
    }

    template <class Iter>
    opfstream& writelines(Iter begin_, const Iter end_, const char sep='\n') {
        if (begin_ == end_) return *this;
        *this << *begin_;
        while (++begin_ != end_) {*this << sep << *begin_;}
        return *this;
    }

    template <class V>
    opfstream& writelines(const V& lines, const char sep='\n') {
        return writelines(begin(lines), end(lines), sep);
    }
};

inline std::vector<std::pair<std::string, std::string> >
read_ini(const std::string& filename) {
    std::vector<std::string> lines = ipfstream(filename).readlines();
    std::vector<std::pair<std::string, std::string> > dst;
    dst.reserve(lines.size());
    for (auto line_: lines) {
        line_ = strip(line_);
        if (startswith(line_, "[")) {continue;} // TODO
        auto pair_ = split(line_, ":="); // TODO
        if (pair_.size() < 2) {continue;}
        dst.emplace_back(rstrip(pair_[0]), lstrip(pair_[1]));
    }
    return dst;
}

inline std::vector<std::string>
read_header(std::istream& fin, const char* sep="\t") {
    std::string buffer;
    std::getline(fin, buffer, '\n');
    return split(buffer, sep);
}

template <class T> inline std::vector<std::valarray<T>>
read_valarrays(std::istream& ist) {
    std::vector<std::valarray<T>> matrix;
    std::string buffer;
    std::istream_iterator<T> end;
    while (std::getline(ist, buffer)) {
        std::istringstream iss(buffer);
        std::vector<T> vec(std::istream_iterator<T>(iss), end);
        matrix.emplace_back(vec.data(), vec.size());
    }
    return matrix;
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
} // namespace wtl
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif /* WTL_IOSTR_HPP_ */
