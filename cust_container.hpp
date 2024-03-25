#pragma once

#include <iterator>

#include "cust_alloc.hpp"

namespace custom {

    template<typename T>
    class Container {
    public:

        using size_type = std::size_t;

        struct Cell {
            T element;
            Cell* next = nullptr;
            Cell(T init) : element(init) {}
        };

        using resource_pointer = std::pmr::memory_resource*;
        using allocator_type = std::pmr::polymorphic_allocator<Cell>;

        using value_type = Cell;
        using reference = Cell&;
        using const_reference = const Cell&;
        using pointer = typename std::pmr::polymorphic_allocator<Cell>::value_type*;
        using const_pointer = const typename std::pmr::polymorphic_allocator<Cell>::value_type*;

        class CellIterator : public std::iterator<std::input_iterator_tag, value_type> {
            friend class Cell;
        public:
            CellIterator(pointer p) : m_ptr(p) {}
            CellIterator(const CellIterator& it) : m_ptr(it.m_ptr) {}

            bool operator!=(const CellIterator& other) const {
                return m_ptr != other.m_ptr;
            }
            bool operator==(const CellIterator & other) const {
                return m_ptr == other.m_ptr;
            }
            T& operator*() const {
                return m_ptr->element;
            }
            CellIterator& operator++() {
                m_ptr = m_ptr->next;
                return *this;
            }
        private:
            pointer m_ptr;
        };

        using iterator = CellIterator;
        using const_iterator = const CellIterator;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    private:
        allocator_type m_allocat;
        pointer m_ptr_beg = nullptr;
        pointer* m_ptr_cur = &m_ptr_beg;

    public:

        Container(resource_pointer rsrc) : m_allocat(rsrc) {

        }

        ~Container() = default;

        void append(const T& value) {
            if (!(*m_ptr_cur)) {
                (*m_ptr_cur) = m_allocat.allocate(1);
                m_allocat.construct((*m_ptr_cur), value);
                m_ptr_cur = &((*m_ptr_cur)->next);
            }
            else {
                m_allocat.construct((*m_ptr_cur), value);
            }
        }

        iterator begin() {
            return m_ptr_beg;
        }

        iterator end() {
            return (*m_ptr_cur);
        }
    };

}