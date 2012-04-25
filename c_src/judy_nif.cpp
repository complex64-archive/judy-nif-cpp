#include "judy-cpp.hpp"

#include <erl_nif.h>
#include <boost/pool/poolfwd.hpp>
#include <boost/pool/pool_alloc.hpp>

#include <memory>
#include <exception>


#ifdef __cplusplus
extern "C" {
#endif


/** TODO - Document. */
static ErlNifResourceType* JUDY_HS_ARR;

typedef void* any_ref;

/** TODO - Document. */
typedef judy::hs<
    ErlNifBinary,
    ErlNifBinary,
    boost::fast_pool_allocator<ErlNifBinary>
> judy_hs_arr;



/** TODO - Document. */
static ERL_NIF_TERM
judy_hs_new(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    judy_hs_arr* arr = reinterpret_cast<judy_hs_arr*>(
        enif_alloc_resource(JUDY_HS_ARR, sizeof(judy_hs_arr))
    );

    ERL_NIF_TERM arr_res = enif_make_resource(env, arr);
    enif_release_resource(arr);

    return arr_res;
}


/** TODO - Document. */
static ERL_NIF_TERM
judy_hs_insert(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    judy_hs_arr::key_type key;
    judy_hs_arr::value_type value;
    any_ref obj;

    if (enif_inspect_binary(env, argv[1], &key) &&
        enif_inspect_binary(env, argv[2], &value) &&
          enif_get_resource(env, argv[0], JUDY_HS_ARR, &obj))
    {
        judy_hs_arr* arr = reinterpret_cast<judy_hs_arr*>(obj);

        bool new_insert = arr->insert(key, value);
        const char* return_val = (new_insert ? "true" : "false");

        return enif_make_atom(env, return_val);
    }
    else {
        return enif_make_badarg(env);
    }
}


/** TODO - Document. */
static ERL_NIF_TERM
judy_hs_remove(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    judy_hs_arr::key_type key;
    any_ref obj;

    if (enif_inspect_binary(env, argv[1], &key) &&
        enif_get_resource(env, argv[0], JUDY_HS_ARR, &obj))
    {
        judy_hs_arr* arr = reinterpret_cast<judy_hs_arr*>(obj);

        bool was_removed = arr->remove(key);
        const char* return_val = (was_removed ? "true" : "false");

        return enif_make_atom(env, return_val);
    }
    else {
        return enif_make_badarg(env);
    }
}


/** TODO - Document. */
static ERL_NIF_TERM
judy_hs_get(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    judy_hs_arr::key_type key;
    any_ref obj;

    if (enif_inspect_binary(env, argv[1], &key) &&
        enif_get_resource(env, argv[0], JUDY_HS_ARR, &obj))
    {
        judy_hs_arr* arr = reinterpret_cast<judy_hs_arr*>(obj);

        try {
            judy_hs_arr::value_type value = arr->get(key);
            return enif_make_binary(env, &value);
        }
        catch (int) {
            return enif_make_tuple2(env,
                enif_make_atom(env, "error"),  enif_make_binary(env, &key)
            );
        }
    }
    else {
        return enif_make_badarg(env);
    }
}



/** Called both for existing instances as well as new instances not yet created
    by the calling NIF library. */
void
hs_dtor(ErlNifEnv* env, any_ref obj)
{
    // Get a handle to the array, then explicitly deconstruct.
    judy_hs_arr* arr = reinterpret_cast<judy_hs_arr*>(obj);
    arr->~hs();

    // Force freeing the system memory.
    boost::singleton_pool<
        boost::pool_allocator_tag, sizeof(judy_hs_arr::value_type)
    >::release_memory();

    // Then free the memory allocated for the pointer.
    enif_release_resource(obj);
}


/** Called when the NIF library is loaded and there is no previously loaded
    library for this module. */
static int
hs_load(ErlNifEnv* env, any_ref* priv_data, ERL_NIF_TERM load_info)
{
    // Create or open the JUDY_HS_ARR resource type.
    ErlNifResourceFlags flags = (ErlNifResourceFlags)
        (ERL_NIF_RT_CREATE | ERL_NIF_RT_TAKEOVER);

    // Create or takeover a resource type identified by the string name and
    // give it the destructor function pointed to by dtor.
    JUDY_HS_ARR =
        enif_open_resource_type(env, NULL, "judy_hs_arr", &hs_dtor, flags, 0);

    return 0;
}


/** Called when the NIF library is loaded and there is already a previously
    loaded library for this module code. */
static int
hs_reload(ErlNifEnv* env, any_ref* priv_data, ERL_NIF_TERM load_info)
{
    // Works the same as load. The only difference is that *priv_data already
    // contains the value set by the previous call to load or reload.
    return 0;
}


/** Called when the NIF library is loaded and there is old code of this module
    with a loaded NIF library. */
static int
hs_upgrade(ErlNifEnv* env, any_ref* priv, any_ref* old_priv, ERL_NIF_TERM load_info)
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
