#pragma once
// taken from https://gist.github.com/engelmarkus/fc1678adbed1b630584c90219f77eb48

/*
When using std::bind, all parameters have to be bound either to a value or to a placeholder.
This makes it impossible to use std::bind with variadic functions if you do not know the number
of parameters they take in advance.
This gist shows how to allow binding the first parameters of variadic functions.
The original idea can be found here: http://stackoverflow.com/questions/21192659/variadic-templates-and-stdbind#21193316
*/

#include <functional>

template <int>
struct variadic_placeholder {};

namespace std {
    template <int N>
    struct is_placeholder<variadic_placeholder<N>>
        : integral_constant<int, N + 1>
    {
    };
}


namespace variadic_bind
{
    template <typename Ret, typename Class, typename... Args, size_t... Is, typename... Args2>
    auto bind(std::index_sequence<Is...>, Ret(Class::* fptr)(Args...), Args2&&... args) {
        return std::bind(fptr, std::forward<Args2>(args)..., variadic_placeholder<Is>{}...);
    }

    template <typename Ret, typename Class, typename... Args, typename... Args2>
    auto bind(Ret(Class::* fptr)(Args...), Args2&&... args) {
        return bind(std::make_index_sequence<sizeof...(Args) - sizeof...(Args2) + 1>{}, fptr, std::forward<Args2>(args)...);
    }
}
