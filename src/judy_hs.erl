-module (judy_hs).
-export ([init/0, new/0, insert/3, remove/2, get/2, mget/2, gc/1, reserve/2]).

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
insert(JudyArr, Key, Val) ->
    nif_insert(JudyArr, term_to_binary(Key), term_to_binary(Val)).


% TODO - Document!
nif_insert(_JudyArr, _Key, _Value) ->
    erlang:nif_error(nif_not_loaded).



% TODO - Document!
remove(JudyArr, Key) ->
    nif_remove(JudyArr, term_to_binary(Key)).


% TODO - Document!
nif_remove(_JudyArr, _Key) ->
    erlang:nif_error(nif_not_loaded).


% TODO - Document!
reserve(_JudyArr, _Size) ->
    erlang:nif_error(nif_not_loaded).


% TODO - Document!
gc(_JudyArr) ->
    erlang:nif_error(nif_not_loaded).


% TODO - Document!
get(JudyArr, Key) ->
    make_value(nif_get(JudyArr, term_to_binary(Key))).


% TODO - Document!
nif_get(_JudyArr, _Key) ->
    erlang:nif_error(nif_not_loaded).


% TODO - Document!
mget(JudyArr, Keys) ->
    [?MODULE:get(JudyArr, Key) || Key <- Keys].


% TODO - Document!
make_value({error, Reason}) -> {error, make_value(Reason)};

make_value(Value) -> binary_to_term(Value).
