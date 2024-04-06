#pragma once

#include <memory>

namespace custom {

    template<typename T, typename Allocator>
    class ContainerClassic {
    public:

        using size_type = std::size_t;

        struct Cell {
            T element;
            Cell* next = nullptr;
            Cell(T init) : element(init) {}
        };

        // using allocator_type = Allocator::template rebind<Cell>::other;
        using allocator_type = typename std::allocator_traits<Allocator>::template rebind_alloc<Cell>;

        using value_type = Cell;
        using reference = Cell&;
        using const_reference = const Cell&;
        using pointer = Cell*;
        using const_pointer = const Cell*;

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

        ContainerClassic() = default;
        ~ContainerClassic() = default;

        void append(const T& value) {
            if (!(*m_ptr_cur)) {
                (*m_ptr_cur) = std::allocator_traits<allocator_type>::allocate(m_allocat, 1);
                std::allocator_traits<allocator_type>::construct(m_allocat, (*m_ptr_cur), value);
                m_ptr_cur = &((*m_ptr_cur)->next);
            }
            else {
                std::allocator_traits<allocator_type>::construct(m_allocat, (*m_ptr_cur), value);
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