-module (judy_hs).
-export ([init/0, new/0, insert/3, delete/2, get/2]).

-on_load (init/0).


% TODO - Document!
init() ->
    EbinDir = filename:dirname(code:which(?MODULE)),
    AppPath = filename:dirname(EbinDir),
    Path    = filename:join([AppPath, "priv", "judy_nif"]),
    ok      = erlang:load_nif(Path, 0).



% TODO - Document!
new() ->
    erlang:nif_error(nif_not_loaded).


% TODO - Document!
insert(JudyArr, Key, Val) when is_binary(Key) andalso is_binary(Val) ->
    nif_insert(JudyArr, Key, Val);


% TODO - Document!
insert(JudyArr, Key, Val) ->
    insert(JudyArr, term_to_binary(Key), term_to_binary(Val)).


% TODO - Document!
nif_insert(_JudyArr, _Key, _Value) ->
    erlang:nif_error(nif_not_loaded).



% TODO - Document!
delete(JudyArr, Key) when is_binary(Key) ->
    nif_delete(JudyArr, Key);


% TODO - Document!
delete(JudyArr, Key) ->
    nif_delete(JudyArr, term_to_binary(Key)).


% TODO - Document!
nif_delete(_JudyArr, _Key) ->
    erlang:nif_error(nif_not_loaded).



% TODO - Document!
get(JudyArr, Key) when is_binary(Key) ->
    nif_get(JudyArr, Key);


% TODO - Document!
get(JudyArr, Key) ->
    get(JudyArr, term_to_binary(Key)).


% TODO - Document!
nif_get(_JudyArr, _Key) ->
    erlang:nif_error(nif_not_loaded).
