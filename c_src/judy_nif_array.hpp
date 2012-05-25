#ifndef JUDY_NIF_ARRAY_HPP
#define JUDY_NIF_ARRAY_HPP JUDY_NIF_ARRAY_HPP

#include <cstring>

#include <boost/pool/poolfwd.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <boost/pool/object_pool.hpp>

#include "judy-cpp.hpp"


namespace judy_nif
{

// Type of a single element of an ErlNifBinary.
typedef unsigned char erl_binary_type;

// Type used to describe the size of ErlNifBinaries.
typedef unsigned erl_binary_size_type;


template<typename K,
    typename V,
    typename S >
class basic_array
{
public:
    /// Type of a pointer to `this`.
    typedef basic_array<K, V, S>* pointer_type;

    /// Type of key used for entries in the JudyHS array.
    typedef K key_type;
    typedef key_type& key_reference;
    typedef key_type* key_pointer;

    /// Type of data stored in an erlang binary.
    typedef V value_type;
    typedef value_type& value_reference;
    typedef value_type* value_pointer;

    /// Type of the sizes of elements stored in this array.
    typedef S size_type;


protected:
    /// A single element in the array.
    typedef struct {
        size_type value_size;
        size_type key_size;
        key_pointer   key;
        value_pointer value;
    } element_type;
    typedef element_type* element_pointer;


    /// Type of the underlying to a JudyHS array.
    typedef judy::hs<key_pointer, element_pointer> array_type;
    typedef array_type* array_pointer;


    /// Allocator used for elements.
    typedef boost::object_pool<
        element_type, boost::default_user_allocator_new_delete >
        element_allocator_type;

    /// Allocator used for elements.
    typedef boost::pool<boost::default_user_allocator_malloc_free>
        binary_allocator_type;


public:
    basic_array(const std::size_t grow_by = 1024)
        : elem_alloc_(element_allocator_type()),
          binary_alloc_(binary_allocator_type(sizeof(value_type), grow_by))
    {}


    bool
    insert(const key_reference key0, const value_reference value0)
    {
        const size_type value_size = value0.size;
        const size_type key_size   = key0.size;

        // Allocate memory for the new element.
        element_pointer elem = elem_alloc_.construct();
        // FIXME - Handle out-of-memory, value == 0.
        elem->value_size = value_size;
        elem->key_size = key_size;

        // Copy over the key.
        key_pointer key =
            static_cast<key_pointer>(binary_alloc_.ordered_malloc(key_size));
        // FIXME - Handle out-of-memory, value == 0.

        std::copy(key0.data, key0.data + key_size, key);
        elem->key = key;

        // And copy over the value.
        value_pointer value =
            static_cast<value_pointer>(
                binary_alloc_.ordered_malloc(value_size));
        // FIXME - Handle out-of-memory, value == 0.

        std::copy(value0.data, value0.data + value_size, value);
        elem->value = value;

        // Insert new element and retrieve old value (if any).
        element_pointer old_value =
            judy_->insert(elem->key, key_size, elem);

        // In case a value was present and thus replaced, free.
        if (old_value != 0) {
            binary_alloc_.ordered_free(old_value->key, old_value->key_size);
            binary_alloc_.ordered_free(old_value->value, old_value->value_size);
            elem_alloc_.destroy(old_value);
        }

        // Return whether the value was newly inserted.
        return enif_make_atom(env, (old_value == 0) ? "true" : "false");
    }


    ERL_NIF_TERM
    get(const key_reference key)
    {
        // Retrieve back the element's pointer.
        element_pointer elem = judy_->get(key.data, key.size);
        const size_type value_size = elem->value_size;

        // Create and return a new Erlang binary from the element.
        ERL_NIF_TERM term;
        value_pointer binary = enif_make_new_binary(env, value_size, &term);
        std::copy(elem->value, (elem->value + value_size), binary);

        return term;
    }


    bool
    remove(const key_reference key)
    {
        // FIXME - Deallocate!
        return judy_->remove(key.data, key.size);
    }


private:
    array_type judy_;
    element_allocator_type elem_alloc_;
    binary_allocator_type binary_alloc_;
};


// Types of a single entity in a ErlNifBinary.
typedef unsigned char erl_binary_type;

// Type of the sizes of ErlNifBinaries.
typedef unsigned erl_binary_size_type;

// Types of arrays used by the judy_nif.
typedef basic_array<
    erl_binary_type,
    erl_binary_type,
    erl_binary_size_type > array_type;
typedef array_type* array_pointer;

} // namespace

#endif
