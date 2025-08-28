#pragma once
#include "extern/visit_struct.hpp"

#include <type_traits>

#define SVH_IMGUI_INPUT(T, ...) \
VISITABLE_STRUCT_IN_CONTEXT(svh::input_type_context, T, __VA_ARGS__);

namespace svh {
    template<typename T, typename C> /* Has a visitable struct implementation */
    constexpr bool is_visitable_v = visit_struct::traits::is_visitable<T, C>::value;

    /* Requires C++ 20 */
    template<typename T>
    using key_t = std::remove_cvref_t<T>;

    struct input_type_context {};
}
