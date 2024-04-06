#pragma once

#include <cstddef>

namespace custom {

    template <class T, std::size_t N, bool Fix>
    struct AllocatorClassic {
        using value_type = T;



        const std::size_t m_max_bytes = N * sizeof(T);

        struct Cell {
            std::uint8_t* mem = nullptr;
            int fill = 0;
            std::size_t size = 0;
            Cell* next = nullptr;
        };

        Cell* first_cell = nullptr;
        Cell* cur_cell = nullptr;




        AllocatorClassic() noexcept {
            // std::cout << "Alloc. create " << m_max_bytes << " bytes" << std::endl;
            first_cell = get_new(m_max_bytes);
            cur_cell = first_cell;
        }

        ~AllocatorClassic() {
            delete_and_get_next(first_cell);
        }

        T* allocate(std::size_t n) {
            // std::cout << "Size type " << sizeof(T) << " " << std::endl;
            // std::cout << "Alloc " << n * sizeof(T) << " bytes" << std::endl;
            // std::cout << "Num " << n << " " << std::endl;

            auto bytes = n * sizeof(T);
            auto new_busy_mem_size = cur_cell->fill + bytes;

            if (new_busy_mem_size > cur_cell->size) {
                if (Fix) {
                    throw std::bad_alloc();
                }
                else {
                    cur_cell->next = get_new(bytes > m_max_bytes ? bytes : m_max_bytes);
                    cur_cell = cur_cell->next;
                }
            }
            auto* return_ptr = cur_cell->mem + cur_cell->fill;
            cur_cell->fill = new_busy_mem_size;
            return (T*)return_ptr;
        }

        void deallocate(T* p, std::size_t n) {
            // std::cout << "Dealloc " << n * sizeof(T) << " bytes" << std::endl;
            (void)p;
            (void)n;
        }

        template<class U>
        struct rebind {
            typedef AllocatorClassic<U, N, Fix> other;
        };

        using propagate_on_container_copy_assignment = std::false_type;
        using propagate_on_container_move_assignment = std::false_type;
        using propagate_on_container_swap = std::false_type;

    private:

        Cell* get_new(size_t bytes) {
            Cell* ptr_cell = (Cell*)malloc(sizeof(Cell));
            if (!ptr_cell) {
                throw std::bad_alloc();
            }
            ptr_cell->mem = (std::uint8_t*)malloc(bytes);
            if (!ptr_cell->mem) {
                throw std::bad_alloc();
            }
            ptr_cell->size = bytes;
            return ptr_cell;
        }

        void delete_and_get_next(Cell* c) {
            if (c->next) {
                delete_and_get_next(c->next);
            }
            free(c->mem);
            free(c);
        }
    };

    template <class T, class U, std::size_t N, bool Fix>
    constexpr bool operator== (const AllocatorClassic<T, N, Fix>& a1, const AllocatorClassic<U, N, Fix>& a2) noexcept {
        return (a1.m_max_bytes == a2.m_max_bytes) && 
            (a1.first_cell == a2.first_cell) && 
            (a1.cur_cell == a2.cur_cell);
    }

    template <class T, class U, std::size_t N, bool Fix>
    constexpr bool operator!= (const AllocatorClassic<T, N, Fix>& a1, const AllocatorClassic<U, N, Fix>& a2) noexcept {
        return (a1.m_max_bytes != a2.m_max_bytes) || 
            (a1.first_cell != a2.first_cell) || 
            (a1.cur_cell != a2.cur_cell);
    }
}