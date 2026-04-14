# basedlib

Some stuff designed to be used with [Based Engine](https://gitlab.com/trexxet/basedengine) and [Basedline](https://gitlab.com/trexxet/basedline).

### Current features

#### Containers
* Append-only ring buffer
* Constexpr-friendly static vector
* Thread-safe FIFO queue

#### Core
* Debounce timer
* Event-driven FSM without jump tables
* Function pointer wrappers for functions & non-capturing lambdas
* Dumb debug file logger

#### Metaprogramming
* Overloaded pattern
* Simple static reflection for type names & values (using `__PRETTY_FUNCTION__`)
* Enum static reflection for getting field name (simplier alternative to magic_enum)
* Helper traits & concepts (`specialization_of`, `variadic_t`, `all_same` etc.)

#### Text
* Cross-platform multibyte string <-> wide string converters

#### Other
* Macros for non-retarded move/copy constructor/assignment declaration

### Feature goals
* Autocompletion Trie (implemented in BE, will move here later)

# basedtest

Lightweight & fast test framework for value and assertion tests.

See the [test demo](demo/demoTest.cpp) for features & usage.
