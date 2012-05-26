#include <erl_nif.h>
#include "judy_nif_array.hpp"


extern "C" {

typedef judy_nif::binary_array array_type;
typedef array_type* array_pointer;

/// Erlang NIF resource instance for JudyHS arrays.
static ErlNifResourceType* JUDY_NIF_RES;


// Native implementation of judy:new/0.
static ERL_NIF_TERM
judy_new(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    // Get memory for the new array object from the VM
    array_pointer arr =
        static_cast<array_pointer>(
            enif_alloc_resource(JUDY_NIF_RES, sizeof(array_type)));

    // Construct the object.
    new (arr) array_type;

    // Return the array to the VM
    ERL_NIF_TERM arr_res = enif_make_resource(env, arr);
    enif_release_resource(arr);

    return arr_res;
}


// Native implementation of judy:nif_insert/3.
static ERL_NIF_TERM
judy_insert(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ErlNifBinary key, value;
    void* arr0 = 0;
    array_pointer arr = 0;

    if (enif_inspect_binary(env, argv[1], &key) &&
            enif_inspect_binary(env, argv[2], &value) &&
            enif_get_resource(env, argv[0], JUDY_NIF_RES, &arr0))
        arr = static_cast<array_pointer>(arr0);
    else enif_make_badarg(env);

    // Return whether the value was newly inserted.
    bool newly_inserted = arr->insert(key, value);
    return enif_make_atom(env, newly_inserted ? "true" : "false");
}


// Native implementation of judy:nif_remove/2.
static ERL_NIF_TERM
judy_remove(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ErlNifBinary key;
    void* arr0 = 0;
    array_pointer arr = 0;

    if (enif_inspect_binary(env, argv[1], &key) &&
            enif_get_resource(env, argv[0], JUDY_NIF_RES, &arr0))
        arr = static_cast<array_pointer>(arr0);
    else return enif_make_badarg(env);

    // Return whether the value was actually removed.
    bool was_removed = arr->remove(key);
    return enif_make_atom(env, was_removed ? "true" : "false");
}


// Native implementation of judy:nif_get/2.
static ERL_NIF_TERM
judy_get(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ErlNifBinary key;
    void* arr0 = 0;
    array_pointer arr = 0;

    if (enif_inspect_binary(env, argv[1], &key) &&
            enif_get_resource(env, argv[0], JUDY_NIF_RES, &arr0))
        arr = static_cast<array_pointer>(arr0);
    else return enif_make_badarg(env);

    ERL_NIF_TERM term = arr->get(key, env);

    if (term != 0) return term;
    else return enif_make_tuple2(env,
        enif_make_atom(env, "error"), enif_make_binary(env, &key));
}


// Native implementation of judy:size/1.
static ERL_NIF_TERM
judy_size(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    void* arr0 = 0;
    array_pointer arr = 0;

    if (enif_get_resource(env, argv[0], JUDY_NIF_RES, &arr0))
        arr = static_cast<array_pointer>(arr0);
    else return enif_make_badarg(env);

    return enif_make_int64(env, arr->size());
}



/** Called both for existing instances as well as new instances not yet created
    by the calling NIF library. */
void
dtor(ErlNifEnv* env, void* obj)
{
    // Get a handle to the array, then explicitly deconstruct.
    array_pointer arr = static_cast<array_pointer>(obj);
    arr->~array_type();

    // Then free the memory allocated for the pointer.
    enif_release_resource(obj);
}


/** Called when the NIF library is loaded and there is no previously loaded
    library for this module. */
static int
load(ErlNifEnv* env, void** priv_data, ERL_NIF_TERM load_info)
{
    // Create or open the JUDY_NIF_RES resource type.
    ErlNifResourceFlags flags = (ErlNifResourceFlags)
        (ERL_NIF_RT_CREATE | ERL_NIF_RT_TAKEOVER);

    // Create or takeover a resource type identified by the string name and
    // give it the destructor function pointed to by dtor.
    JUDY_NIF_RES =
        enif_open_resource_type(env, NULL, "judy_nif_res", &dtor, flags, 0);

    return 0;
}


/** Called when the NIF library is loaded and there is already a previously
    loaded library for this module code. */
static int
reload(ErlNifEnv* env, void** priv_data, ERL_NIF_TERM load_info)
{
    // Works the same as load. The only difference is that *priv_data already
    // contains the value set by the previous call to load or reload.
    return 0;
}


/** Called when the NIF library is loaded and there is old code of this module
    with a loaded NIF library. */
static int
upgrade(ErlNifEnv* env, void** priv, void** old_priv, ERL_NIF_TERM load_info)
{
    // Works the same as load. The only difference is that *old_priv_data
    // already contains the value set by the last call to load or reload for
    // the old module code. *priv_data will be initialized to NULL when upgrade
    // is called. It is allowed to write to both *priv_data and *old_priv_data.
    *priv = *old_priv;
    return 0;
}


/// Array of function descriptors for all the implemented NIFs in this library.
static ErlNifFunc judy_funs[] =
{
    {"new",         0, judy_new},
    {"nif_insert",  3, judy_insert},
    {"nif_remove",  2, judy_remove},
    {"nif_get",     2, judy_get},
    {"size",        1, judy_size}
};


// Initialize the NIF library.
ERL_NIF_INIT(judy, judy_funs, &load, &reload, &upgrade, NULL);

}
