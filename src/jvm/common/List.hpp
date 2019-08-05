////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//              ColdSpot, a Java virtual machine implementation.              //
//                    Copyright (C) 2014, Mario Morgenthum                    //
//                                                                            //
//                                                                            //
//  This program is free software: you can redistribute it and/or modify      //
//  it under the terms of the GNU General Public License as published by      //
//  the Free Software Foundation, either version 3 of the License, or         //
//  (at your option) any later version.                                       //
//                                                                            //
//  This program is distributed in the hope that it will be useful,           //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of            //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             //
//  GNU General Public License for more details.                              //
//                                                                            //
//  You should have received a copy of the GNU General Public License         //
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.     //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef COLDSPOT_JVM_COMMON_LIST_HPP_
#define COLDSPOT_JVM_COMMON_LIST_HPP_

#include <cstdint>

#include "Memory.hpp"

namespace coldspot
{

    template<typename T>
    class List
    {
    public:

        class Node
        {
        public:

            T data;
            Node *next;
            Node *previous;

            Node(T data) : data(data), next(0), previous(0)
            {
            }
        };

        class Iterator
        {
        public:

            Iterator(Node *node) : current(node)
            {
            }

            T &operator*()
            {
                return current->data;
            }

            Iterator &operator++()
            {
                current = current->next;
                return *this;
            }

            Iterator &operator--()
            {
                current = current->previous;
                return *this;
            }

            friend bool operator==(const List<T>::Iterator &lhs,
                const List<T>::Iterator &rhs)
            {
                if (lhs.current == rhs.current)
                {
                    return true;
                }
                if (lhs.current == 0 || rhs.current == 0)
                {
                    return false;
                }
                return lhs.current->data == rhs.current->data;
            }

            friend bool operator!=(const List<T>::Iterator &lhs,
                const List<T>::Iterator &rhs)
            {
                return !(lhs == rhs);
            }

            Node *current;
        };

        static void move(List<T> &source, List<T> &target)
        {
            auto iterator = source.begin();
            while (iterator != source.end())
            {
                target.addBack(*iterator);
                iterator = source.erase(iterator);
            }
        }

        List() : _first(0), _last(0), _size(0)
        {
        }

        List(const List &rhs) : _first(0), _last(0), _size(0)
        {
            copyFrom(rhs);
        }

        ~List()
        {
            clear();
        }

        List &operator=(const List &rhs)
        {
            clear();
            copyFrom(rhs);

            return *this;
        }

        void addBack(const T &element)
        {
            Node *node = new Node(element);

            if (_last)
            {
                _last->next = node;
                node->previous = _last;
            }
            else
            {
                _first = node;
            }

            _last = node;
            ++_size;
        }

        void addFront(const T &element)
        {
            Node *node = new Node(element);

            if (_first)
            {
                _first->previous = node;
                node->next = _first;
            }
            else
            {
                _last = node;
            }

            _first = node;
            ++_size;
        }

        T &back() const
        {
            return _last->data;
        }

        Iterator begin() const
        {
            return Iterator(_first);
        }

        void clear()
        {
            auto iter = begin();
            while (iter != end())
            {
                iter = erase(iter);
            }
        }

        bool empty() const
        {
            return size() == 0;
        }

        Iterator end() const
        {
            return Iterator(0);
        }

        Iterator erase(Iterator iterator)
        {
            Node *node = iterator.current;
            Iterator nextPosition = ++iterator;

            if (node->previous)
            {
                node->previous->next = node->next;
            }
            else
            {
                _first = node->next;
            }

            if (node->next)
            {
                node->next->previous = node->previous;
            }
            else
            {
                _last = node->previous;
            }

            DELETE_OBJECT(node);
            --_size;

            return nextPosition;
        }

        Iterator find(const T &element)
        {
            auto iter = begin();
            while (iter != end())
            {
                if (*iter == element)
                {
                    return iter;
                }
                ++iter;
            }

            return Iterator(0);
        }

        T &front() const
        {
            return _first->data;
        }

        uint32_t size() const
        {
            return _size;
        }

    private:

        Node *_first;
        Node *_last;
        uint32_t _size;

        void copyFrom(const List<T> &rhs)
        {
            for (auto &element : rhs)
            {
                addBack(element);
            }
        }
    };

}

#endif
