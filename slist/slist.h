#include <cassert>
#include <iterator>

/*
 * Simgly-linked list with low memory footprint. This class does not do any
 * memory management. It relies of the item type to provide "next" pointer to
 * the next element in the list. In the future this can be improved by providing
 * a functor returning a reference to the next pointer.
 */
template<typename T>
class slist
{
  public:
    bool empty() const { return !last; }

    /*
     * Return a reference to the first element in the list. UB when the list is
     * empty.
     */
    const T& front() const
    {
        assert(last);
        return *last->next;
    }
    T& front()
    {
        assert(last);
        return *last->next;
    }

    /*
     * Return a reference to the last element in the list. UB when the list is
     * empty.
     */
    const T& back() const
    {
        assert(last);
        return *last;
    }
    T& back()
    {
        assert(last);
        return *last;
    }

    /*
     * Add an item to the front of the list.
     */
    void push_front(T* const item)
    {
        assert(!item->next);

        if (last) {
            item->next = last->next;
            last->next = item;
        } else {
            item->next = item;
            last = item;
        }
    }

    /*
     * Remove and return the first item of the list.
     */
    T* pop_front()
    {
        if (!last) {
            return nullptr;
        }

        T* const first = last->next;

        if (first == last) {
            last = nullptr;
        } else {
            last->next = first->next;
        }

        first->next = nullptr;
        return first;
    }

    /*
     * Add an item to the back of the list.
     */
    void push_back(T* const item)
    {
        assert(!item->next);

        push_front(item);
        last = item;
    }

    class const_iterator
    {
      public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = const T*;
        using reference = const T&;

        reference operator*() const { return *previous->next; }
        pointer operator->() const { return previous->next; }

        const_iterator& operator++()
        {
            if (previous) {
                previous = previous->next;
                if (previous == container->last) {
                    previous = nullptr;
                }
            }

            return *this;
        }

        const_iterator operator++(int)
        {
            const_iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator==(const const_iterator& a, const const_iterator& b)
        {
            return a.container == b.container && a.previous == b.previous;
        };

        friend bool operator!=(const const_iterator& a, const const_iterator& b)
        {
            return !(a == b);
        };

      protected:
        friend class slist;

        const_iterator(slist<T>* container, T* previous)
          : container(container)
          , previous(previous)
        {
        }

        slist<T>* container{};
        T* previous{};
    };

    class iterator : public const_iterator
    {
      public:
        using pointer = T*;
        using reference = T&;

        reference operator*() const { return *const_iterator::previous->next; }
        pointer operator->() const { return const_iterator::previous->next; }

        iterator& operator++()
        {
            const_iterator::operator++();
            return *this;
        }

        iterator operator++(int)
        {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

      protected:
        friend class slist;

        iterator(slist<T>* container, T* previous)
          : const_iterator(container, previous)
        {
        }
    };

    iterator begin() { return iterator(this, last); }
    iterator end() { return iterator(this, nullptr); }

    const_iterator cbegin() { return const_iterator(this, last); }
    const_iterator cend() { return const_iterator(this, nullptr); }

    const_iterator begin() const { return cbegin(); }
    const_iterator end() const { return cend(); }

    void clear()
    {
        while (!empty()) {
            (void)pop_front();
        }
    }

    /*
     * Insert an item at the position and return an iterator pointing at the
     * newly inserted element.
     */
    iterator insert(const const_iterator& position, T* item)
    {
        assert(position.container == this);
        assert(!item->next);

        if (position.previous) {
            item->next = position.previous->next;
            position.previous->next = item;
            return iterator(this, position.previous);
        } else if (last) {
            T* const previous = last;
            push_back(item);
            return iterator(this, previous);
        } else {
            push_back(item);
            return iterator(this, item);
        }
    }

    /*
     * Erase an item at the position and return an iterator pointing at the next
     * element.
     */
    iterator erase(const const_iterator& position)
    {
        assert(position.container == this);

        if (position.previous) {
            T* const item = position.previous->next;
            T* const next = item->next;
            if (next == item) {
                pop_front();
            } else {
                position.previous->next = next;
                item->next = nullptr;
                if (last == item) {
                    last = position.previous;
                } else {
                    return iterator(this, position.previous);
                }
            }
        }

        return end();
    }

  private:
    friend class iterator;

    /**
     * Pointer to the last element of the list; nullptr when the list is empty.
     */
    T* last{};
};
