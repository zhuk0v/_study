#pragma once

#include <memory_resource>

// #include <iostream>
#include <cstring>

namespace custom {

    template <typename T, std::size_t N, bool Fix>
    struct AllocatorResource : public std::pmr::memory_resource {
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

        AllocatorResource() {
            // std::cout << "Alloc. create " << m_max_bytes << " bytes" << std::endl;
            first_cell = get_new(m_max_bytes);
            cur_cell = first_cell;
        }

        ~AllocatorResource() {
            delete_and_get_next(first_cell);
        }
    private:

        void* do_allocate(size_t bytes, size_t) override {
            // std::cout << "Size type " << sizeof(T) << " " << std::endl;
            // std::cout << "Alloc " << bytes << " bytes" << std::endl;
            // std::cout << "Num " << bytes / sizeof(T) << " " << std::endl;

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
            return return_ptr;
        }

        void do_deallocate(void* p, size_t bytes, size_t) override {
            // std::cout << "Dealloc " << bytes << " bytes" << std::endl;
            (void)p;
            (void)bytes;
        }

        bool do_is_equal(const std::pmr::memory_resource&) const noexcept override {
            // std::cout << "Check equal" << std::endl;
            return true;
        }

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
            if (c->next){
                delete_and_get_next(c->next);
            }
            free(c->mem);
            free(c);
        }
    };

} // namespace custom