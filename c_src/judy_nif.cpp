#include "judy-cpp.hpp"

#include <erl_nif.h>

#ifdef __cplusplus
extern "C" {
#endif


namespace judy_nif
{

/// Type of key used for entries in the JudyHS array.
typedef unsigned char hs_key_type;
typedef hs_key_type*  hs_key_pointer;

/// Type of data stored in an erlang binary.
typedef unsigned char  hs_value_type;
typedef hs_value_type* hs_value_pointer;

/// A single element in the array.
typedef struct {
    hs_key_pointer   key;
    hs_value_pointer value;
    std::size_t      value_size;
} hs_element_type;

typedef hs_element_type* hs_element_pointer;


/// Type alias for an JudyHS array instance.
typedef judy::hs<hs_key_pointer, hs_element_pointer> hs_array_type;

/// Pointer to an JudyHS array.
typedef hs_array_type* hs_array_pointer;


/// Allocator used for new elements in the array.
typedef boost::fast_pool_allocator<hs_element_type> hs_allocator_type;
typedef hs_allocator_type::rebind<hs_key_type>      hs_key_allocator_type;
typedef hs_allocator_type::rebind<hs_value_type>    hs_value_allocator_type;

static hs_allocator_type hs_allocator;

} // namespace



/// Erlang NIF resource instance for JudyHS arrays.
static ErlNifResourceType* JUDY_HS_RES;



static ERL_NIF_TERM
judy_hs_new(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    using namespace judy_nif;

    // Get memory for the new array object from the VM
    void* resource =
        enif_alloc_resource(JUDY_HS_RES, sizeof(hs_array_type));

    // Create the array itself
    hs_array_pointer arr_p = static_cast<hs_array_pointer>(resource);
    arr_p = new (arr_p) hs_array_type;

    // Return the array to the VM
    ERL_NIF_TERM arr_res = enif_make_resource(env, arr_p);
    enif_release_resource(arr_p);

    return arr_res;
}


static ERL_NIF_TERM
judy_hs_insert(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    using namespace judy_nif;

    ErlNifBinary key0;
    ErlNifBinary value0;
    void* arr0;

    if (enif_inspect_binary(env, argv[1], &key0) &&
        enif_inspect_binary(env, argv[2], &value0) &&
          enif_get_resource(env, argv[0], JUDY_HS_RES, &arr0))
    {
        hs_array_pointer arr_p = static_cast<hs_array_pointer>(arr0);

        const std::size_t value_size = value0.size;
        const std::size_t key_size   = key0.size;

        // Allocate memory for the new element.
        hs_element_pointer elem_p =
            new (hs_allocator.allocate(1)) hs_element_type;
        elem_p->value_size = value_size;

        // Copy over the key.
        hs_key_pointer key_p =
            hs_key_allocator_type::other(hs_allocator).allocate(key_size);
        std::copy(key0.data, key0.data + key_size, key_p);
        elem_p->key = key_p;

        // And copy over the value.
        hs_value_pointer value_p =
            hs_value_allocator_type::other(hs_allocator).allocate(value_size);
        std::copy(value0.data, (value0.data + value_size), value_p);
        elem_p->value = value_p;

        // Insert new element and retrieve old value (if any).
        hs_element_pointer old_value =
            arr_p->insert(elem_p->key, key_size, elem_p);

        // In case a value was present and thue replaced,
        // deallocate the old value.
        if (old_value != 0) {
            hs_key_allocator_type::other(hs_allocator).deallocate(old_value->key);
            hs_value_allocator_type::other(hs_allocator).deallocate(old_value->value);
            hs_allocator.deallocate(old_value);
        }

        // Return whether the value was newly inserted.
        return enif_make_atom(env, (old_value == 0) ? "true" : "false");
    }
    else {
        return enif_make_badarg(env);
    }
}


static ERL_NIF_TERM
judy_hs_remove(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    using namespace judy_nif;

    // ErlNifBinary key;
    // void* arr0;

    // if (enif_inspect_binary(env, argv[1], &key) &&
    //     enif_get_resource(env, argv[0], JUDY_HS_RES, &arr0))
    // {
    //     hs_array_pointer arr = static_cast<hs_array_pointer>(arr0);

    //     bool was_removed = arr->remove(key.data, key.size);
    //     const char* return_val = (was_removed ? "true" : "false");

    //     return enif_make_atom(env, return_val);
    // }
    // else {
        return enif_make_badarg(env);
    // }
}


static ERL_NIF_TERM
judy_hs_get(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    using namespace judy_nif;

    ErlNifBinary key;
    void* arr0;

    if (enif_inspect_binary(env, argv[1], &key) &&
        enif_get_resource(env, argv[0], JUDY_HS_RES, &arr0))
    {
        hs_array_pointer arr_p = static_cast<hs_array_pointer>(arr0);

        try {
            // Retrieve back the element's pointer.
            hs_element_pointer elem_p = arr_p->get(key.data, key.size);

            const std::size_t value_size = elem_p->value_size;

            ERL_NIF_TERM term;
            hs_value_pointer binary = enif_make_new_binary(env, value_size, &term);
            std::copy(elem_p->value, (elem_p->value + value_size), binary);

            return term;
        }
        catch (bool) {
            return enif_make_tuple2(env,
                enif_make_atom(env, "error"), enif_make_binary(env, &key));
        }
    }
    else {
        return enif_make_badarg(env);
    }
}



/** Called both for existing instances as well as new instances not yet created
    by the calling NIF library. */
void
hs_dtor(ErlNifEnv* env, void* obj)
{
    using namespace judy_nif;

    // Get a handle to the array, then explicitly deconstruct.
    hs_array_type* arr = static_cast<hs_array_type*>(obj);
    arr->~hs();

    // FIXME - Deallocate properly.

    // Then free the memory allocated for the pointer.
    enif_release_resource(obj);
}


/** Called when the NIF library is loaded and there is no previously loaded
    library for this module. */
static int
hs_load(ErlNifEnv* env, void** priv_data, ERL_NIF_TERM load_info)
{
    // Create or open the JUDY_HS_RES resource type.
    ErlNifResourceFlags flags = (ErlNifResourceFlags)
        (ERL_NIF_RT_CREATE | ERL_NIF_RT_TAKEOVER);

    // Create or takeover a resource type identified by the string name and
    // give it the destructor function pointed to by dtor.
    JUDY_HS_RES =
        enif_open_resource_type(env, NULL, "hs_array_type", &hs_dtor, flags, 0);

    return 0;
}


/** Called when the NIF library is loaded and there is already a previously
    loaded library for this module code. */
static int
hs_reload(ErlNifEnv* env, void** priv_data, ERL_NIF_TERM load_info)
{
    // Works the same as load. The only difference is that *priv_data already
    // contains the value set by the previous call to load or reload.
    return 0;
}


/** Called when the NIF library is loaded and there is old code of this module
    with a loaded NIF library. */
static int
hs_upgrade(ErlNifEnv* env, void** priv, void** old_priv, ERL_NIF_TERM load_info)
{
    // Works the same as load. The only difference is that *old_priv_data
    // already contains the value set by the last call to load or reload for
    // the old module code. *priv_data will be initialized to NULL when upgrade
    // is called. It is allowed to write to both *priv_data and *old_priv_data.
    *priv = *old_priv;
    return 0;
}



/// Array of function descriptors for all the implemented NIFs in this library.
static ErlNifFunc judy_hs_funs[] =
{
    {"new",         0, judy_hs_new},
    {"nif_insert",  3, judy_hs_insert},
    {"nif_remove",  2, judy_hs_remove},
    {"nif_get",     2, judy_hs_get}
};

// Initialize the NIF library.
ERL_NIF_INIT(judy_hs, judy_hs_funs, &hs_load, &hs_reload, &hs_upgrade, NULL);


#ifdef __cplusplus
}
#endif
