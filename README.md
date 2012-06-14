## judy-nif-cpp

Erlang [Native Implemented Functions](http://www.erlang.org/doc/tutorial/nif.html) around [Judy arrays](http://judy.sourceforge.net/), written in C++.

### Features
Given enough system memory,
- Arbitary number of elements,
- Arbitary size of keys and elements,
- Memory allocation using [pools](http://www.boost.org/libs/pool) for less memory fragmentation

### Non-Features
- Nested arrays.

## Implemented Functions
All NIF functions accessing the underlying array are wrapped with normal functions doing `term()` to `binary()`, and vice versa, translation prior storage or retrieval.

The array is modified in-place, and thus _not a functional data structure_.
<br>


##### `judy:new()`
Creates a new Judy array resource.
```erlang
1> judy:new().
<<>>
```
****


##### `judy:insert(Key, Val, JudyArr)`
Insert a new value into the array.
- Returns whether the value was newly inserted.

```erlang
1> J = judy:new().
<<>>
2> judy:insert(key, value, J).
true
3> judy:insert(key, value, J).
false
```
****


##### `judy:remove(Key, JudyArr)`
Remove a key/value from the array.
- Returns whether the value was actually removed.

```erlang
1> J = judy:new().
<<>>
2> judy:insert(key, value, J).
true
3> judy:remove(key, J).
true
4> judy:remove(key, J).
false
```
****


##### `judy:get(Key, JudyArr)`
Retrieve a value from the array.
- Returns the stored value or `{error, Key}` in case the key was not present.

```erlang
1> J = judy:new().
<<>>
2> judy:insert(key, value, J).
true
3> judy:get(key, J).
value
4> judy:get(foo, J).
{error,foo}
```
****


##### `judy:mget(Keys, JudyArr)`
Retrieve multiple values from the array.

```erlang
1> J = judy:new().
<<>>
2> judy:insert(key, value, J).
<<>>
3> judy:insert(key2, value2, J).
<<>>
4> judy:mget([key, key2, key3], J).
[value,value2,{error,key3}]
```


## License
This project, and all contributed code, are licensed under the FreeBSD License. A copy of the license can be found in the repository.
