#pragma once

#include <iterator>

#include <tuple>
#include <array>
#include <map>

#include <functional>

namespace matr {

    namespace tpl {
        // generate type
        template<typename VAL_T, typename T, unsigned N, typename... REST>
        struct gen_type {
            using type = typename gen_type<VAL_T, T, N - 1, T, REST...>::type;
        };

        template<typename VAL_T, typename T, typename... REST>
        struct gen_type<VAL_T, T, 0, REST...> {
            using type = std::tuple<REST..., VAL_T>;
        };

        // convert array into a tuple
        template<typename Array, std::size_t... I>
        auto a2t_impl(Array& a, std::index_sequence<I...>) {
            return std::make_tuple(std::ref(a[I])...);
        }

        template<typename T, std::size_t N, typename Indices = std::make_index_sequence<N>>
        auto a2t(std::array<T, N>& a) {
            return a2t_impl(a, Indices{});
        }


    } // namespace tpl


    template<typename T, std::size_t N>
    class Matrix {
    public:

        using size_type = std::size_t;

        using value_type = T;
        using reference = T&;
        using const_reference = const T&;
        using pointer = T*;
        using const_pointer = const T*;

        using IndexType = std::array<int, N>;

        using StorageType = std::map<IndexType, value_type>;
        using StorageTypeIterator = typename std::map<IndexType, value_type>::iterator;

        class Cell {
        private:
            IndexType m_idx;
            reference m_value;
        public:

            Cell(IndexType idx, reference value) :
                m_idx(idx),
                m_value(value) {
            }
            ~Cell() = default;

            operator typename tpl::gen_type<reference, int&, N>::type () {
                return std::tuple_cat(tpl::a2t(m_idx), std::tuple<reference>(m_value));
            }
        };

        class CellIterator : public std::iterator<std::input_iterator_tag, value_type> {
            StorageTypeIterator m_storage_iterator;

        public:
            CellIterator(StorageTypeIterator storage_iterator) : m_storage_iterator(storage_iterator) {}
            CellIterator(const CellIterator& it) : m_storage_iterator(it.m_storage_iterator) {}

            bool operator!=(const CellIterator& other) const {
                return m_storage_iterator != other.m_storage_iterator;
            }
            bool operator==(const CellIterator& other) const {
                return m_storage_iterator == other.m_storage_iterator;
            }
            auto operator*() const {
                return Cell(m_storage_iterator->first, m_storage_iterator->second);
            }

            CellIterator& operator++() {
                m_storage_iterator++;
                return *this;
            }
        };

        class SubMatrix {
        private:
            size_type m_depth = 0;
            IndexType m_idx;
            StorageType& m_dat_storage;
            const_reference m_default_value;
        public:

            SubMatrix(StorageType& dat_storage, const_reference default_value, size_type idx) :
                m_dat_storage(dat_storage),
                m_default_value(default_value) {
                m_idx[m_depth] = idx;
                m_depth++;
            }

            ~SubMatrix() {}

            auto value() const {
                return m_dat_storage[m_idx];
            }

            SubMatrix& operator[] (size_type idx) {
                m_idx[m_depth] = idx;
                m_depth++;
                return *this;
            }

            SubMatrix& operator = (value_type val) {
                m_dat_storage[m_idx] = val;
                return *this;
            }

            operator value_type() const {
                if (m_dat_storage.find(m_idx) != m_dat_storage.end()) {
                    return m_dat_storage[m_idx];
                }
                return m_default_value;
            }
        };


    private:

        const value_type m_default_value;
        std::map<IndexType, value_type> m_dat_store;

    public:

        Matrix(value_type default_value = value_type{}) : m_default_value(default_value) {}

        ~Matrix() = default;

        auto begin() {
            return CellIterator(m_dat_store.begin());
        }

        auto end() {
            return CellIterator(m_dat_store.end());
        }

        auto size() const {
            return m_dat_store.size();
        }

        SubMatrix operator[] (size_type i) {
            return SubMatrix(m_dat_store, m_default_value, i);
        }
    };

}