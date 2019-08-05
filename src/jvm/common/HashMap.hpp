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

#ifndef COLDSPOT_JVM_COMMON_HASHMAP_HPP_
#define COLDSPOT_JVM_COMMON_HASHMAP_HPP_

#include "Hashable.hpp"

namespace coldspot
{

    template<typename K, typename V>
    class HashMap
    {
    public:

        class Entry
        {
        public:

            Entry(const K &key, const V &value) : key(key), value(value),
                                                  next(0)
            {
            }

            K key;
            V value;
            Entry *next;
        };

        class Iterator
        {
        public:

            Iterator() : _map(0), _bucketIndex(0), _current(0)
            {
            }

            Iterator(HashMap *map, uint32_t bucketIndex, Entry *current) : _map(
                map), _bucketIndex(bucketIndex), _current(current)
            {
            }

            Iterator &operator++()
            {
                if (_current->next == 0)
                {
                    _current = 0;

                    while (++_bucketIndex < _map->_bucketCount)
                    {
                        if (_map->_buckets[_bucketIndex] != 0)
                        {
                            _current = _map->_buckets[_bucketIndex];
                            break;
                        }
                    }
                }
                else
                {
                    _current = _current->next;
                }

                return *this;
            }

            Entry *operator->()
            {
                return _current;
            }

            friend bool operator==(const Iterator &lhs, const Iterator &rhs)
            {
                return lhs._current == rhs._current;
            }

            friend bool operator!=(const Iterator &lhs, const Iterator &rhs)
            {
                return !(lhs == rhs);
            }

        private:

            HashMap *_map;
            uint32_t _bucketIndex;
            Entry *_current;
        };

        HashMap(uint32_t bucketCount = 16) : _size(0),
                                             _bucketCount(bucketCount),
                                             _buckets(
                                                 new Entry *[_bucketCount]())
        {
        }

        HashMap(const HashMap &rhs) = delete;

        ~HashMap()
        {
            clearBuckets(_buckets, _bucketCount);
        }

        HashMap &operator=(const HashMap &rhs) = delete;

        Iterator begin()
        {
            uint32_t bucketIndex = 0;
            Entry *current = 0;

            for (uint32_t i = 0; i < _bucketCount; ++i)
            {
                if (_buckets[i] != 0)
                {
                    bucketIndex = i;
                    current = _buckets[i];
                    break;
                }
            }
            return Iterator(this, bucketIndex, current);
        }

        Iterator end()
        {
            return Iterator();
        }

        Entry *get(const K &key) const
        {
            uint32_t bucketIndex = Hashable::hashCode(key) % _bucketCount;
            Entry *current = _buckets[bucketIndex];

            while (current != 0)
            {
                if (current->key == key)
                {
                    return current;
                }

                current = current->next;
            }
            return 0;
        }

        void put(const K &key, const V &value)
        {
            if (_size == _bucketCount)
            {
                resize(_bucketCount * 1.75);
            }

            uint32_t bucketIndex = Hashable::hashCode(key) % _bucketCount;
            Entry *current = _buckets[bucketIndex];

            if (current == 0)
            {
                _buckets[bucketIndex] = new Entry(key, value);
                ++_size;
            }
            else
            {
                for (; ;)
                {
                    if (current->key == key)
                    {
                        current->value = value;
                        break;
                    }
                    else if (current->next == 0)
                    {
                        current->next = new Entry(key, value);
                        ++_size;
                        break;
                    }

                    current = current->next;
                }
            }
        }

        void remove(const K &key)
        {
            uint32_t bucketIndex = Hashable::hashCode(key) % _bucketCount;
            Entry *current = _buckets[bucketIndex];
            Entry *previous = 0;

            while (current != 0)
            {
                if (current->key == key)
                {
                    if (previous == 0)
                    {
                        _buckets[bucketIndex] = current->next;
                    }
                    else
                    {
                        previous->next = current->next;
                    }

                    delete current;
                    --_size;
                    break;
                }

                previous = current;
                current = current->next;
            }

            if (_bucketCount / 1.75 == _size)
            {
                resize(_bucketCount / 1.75);
            }
        }

        uint32_t size() const
        {
            return _size;
        }

    private:

        uint32_t _size;
        uint32_t _bucketCount;
        Entry **_buckets;

        void clearBuckets(Entry **buckets, uint32_t bucketCount)
        {
            for (uint32_t i = 0; i < bucketCount; ++i)
            {
                Entry *current = buckets[i];
                while (current != 0)
                {
                    Entry *next = current->next;
                    delete current;
                    current = next;
                }
            }

            delete[] buckets;
        }

        void resize(uint32_t newBucketCount)
        {
            uint32_t oldBucketCount = _bucketCount;
            Entry **oldBuckets = _buckets;

            _size = 0;
            _bucketCount = newBucketCount;
            _buckets = new Entry *[_bucketCount]();

            for (uint32_t i = 0; i < oldBucketCount; ++i)
            {
                Entry *current = oldBuckets[i];
                while (current != 0)
                {
                    put(current->key, current->value);
                    current = current->next;
                }
            }

            clearBuckets(oldBuckets, oldBucketCount);
        }
    };

}

#endif
