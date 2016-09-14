// -*- mode: c++; coding: utf-8 -*-
#pragma once
#ifndef WTL_EIGEN_HPP_
#define WTL_EIGEN_HPP_

#include <vector>
#include <valarray>
#include <string>
#include <fstream>
#include <Eigen/Core>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl { namespace eigen {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

Eigen::IOFormat tsv(const std::string& sep="\t") {
    return {Eigen::StreamPrecision, Eigen::DontAlignCols, sep, "", "", "\n"};
}

template <class T>
std::vector<T> as_vector(const Eigen::Matrix<T, Eigen::Dynamic, 1>& vec) {
    return std::vector<T>(vec.data(), vec.data() + vec.size());
}

template <class T>
std::valarray<T> as_valarray(const Eigen::Matrix<T, Eigen::Dynamic, 1>& vec) {
    return std::valarray<T>(vec.data(), vec.size());
}

template <class T>
Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> read_matrix(const std::string& path, const char sep='\t') {
    std::ifstream fin0(path.c_str());
    std::string buffer;
    std::getline(fin0, buffer, '\n');
    size_t ncol = std::count(buffer.begin(), buffer.end(), sep) + 1;
    std::ifstream fin(path.c_str());
    std::vector<T> vec{std::istream_iterator<T>(fin), std::istream_iterator<T>()};
    assert(vec.size() % ncol == 0);
    return Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>::Map(vec.data(), vec.size() / ncol, ncol);
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
}} // namespace wtl::eigen
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif // WTL_EIGEN_HPP_
