-module (judy).
-export ([init/0, new/0, insert/3, delete/2, get/2]).

-on_load (init/0).


% TODO - Document!
init() ->
    EbinDir = filename:dirname(code:which(?MODULE)),
    AppPath = filename:dirname(EbinDir),
    Path    = filename:join([AppPath, "priv", "judy"]),
    ok      = erlang:load_nif(Path, 0).


% TODO - Document!
new() ->
    erlang:nif_error(nif_not_loaded).


% TODO - Document!
insert(_JudyArr, _Key, _Value) ->
    erlang:nif_error(nif_not_loaded).


% TODO - Document!
delete(_JudyArr, _Key) ->
    erlang:nif_error(nif_not_loaded).


% TODO - Document!
get(_JudyArr, _Key) ->
    erlang:nif_error(nif_not_loaded).
