#pragma once
#include "extern/visit_struct.hpp"

#include <type_traits>

#define SVH_IMGUI_INPUT(T, ...) \
VISITABLE_STRUCT_IN_CONTEXT(svh::input_type_context, T, __VA_ARGS__);


namespace svh {
    template<typename T, typename C> /* Has a visitable struct implementation */
    constexpr bool is_visitable_v = visit_struct::traits::is_visitable<T, C>::value;

    struct input_type_context {};
}

// Generated using ChatGPT
// ======================= type_settings generator =======================
// Usage 1:
//```
//   GENERATE_TYPE_SETTINGS(float,
//       FIELD(float, min,        0.0f),
//       FIELD(float, max,        1.0f),
//       FIELD(float, step,       0.01f),
//       FIELD(float, step_fast,  0.1f),
//       FIELD(bool,  logarithmic, false)
//   );
//```
// 
// Usage 2 (with extra bases):
//```
// Usage 2 (with extra bases):
//   GENERATE_TYPE_SETTINGS_WITH_BASES(int, (svh::input_type_settings<int>, MyBaseClass<int>),
//       FIELD(int, min,  std::numeric_limits<int>::min()),
//       FIELD(int, max,  std::numeric_limits<int>::max()),
//       FIELD(int, step, 1),
//       FIELD(int, step_fast, 10)
//   );
// ```
// 
// Produces a full specialization:
//   template<> struct type_settings<float> : svh::settings_with_pop<float> { ... };
//
// Note: Supports up to 12 fields by default (extend easily if you need more).

// --- small helpers
#define TS_EXPAND(x) x
#define TS_CAT(a,b) a##b
#define FIELD(Type, Name, Default) (Type, Name, Default)
#define TS_UNPAREN(...) __VA_ARGS__

// Turn (T, name, def) into declarations + fluent setter + getter
#define TS_DECL_ONE_(Type, Name, Default)                                    \
    Type TS_CAT(_, Name) = Default;                                           \
    type_settings& Name(const Type& v) { TS_CAT(_, Name) = v; return *this; } \
    const Type& TS_CAT(get_, Name)() const { return TS_CAT(_, Name); }

#define TS_DECL_ONE(Tuple) TS_DECL_ONE_ Tuple

// ---- FOR_EACH over up to 12 items (extend if you need more)
#define TS_FE_1(M, X1)            M(X1)
#define TS_FE_2(M, X1, X2)        M(X1) TS_EXPAND(TS_FE_1(M, X2))
#define TS_FE_3(M, X1, X2, X3)    M(X1) TS_EXPAND(TS_FE_2(M, X2, X3))
#define TS_FE_4(M, X1, X2, X3, X4) M(X1) TS_EXPAND(TS_FE_3(M, X2, X3, X4))
#define TS_FE_5(M, X1, X2, X3, X4, X5) M(X1) TS_EXPAND(TS_FE_4(M, X2, X3, X4, X5))
#define TS_FE_6(M, X1, X2, X3, X4, X5, X6) M(X1) TS_EXPAND(TS_FE_5(M, X2, X3, X4, X5, X6))
#define TS_FE_7(M, X1, X2, X3, X4, X5, X6, X7) M(X1) TS_EXPAND(TS_FE_6(M, X2, X3, X4, X5, X6, X7))
#define TS_FE_8(M, X1, X2, X3, X4, X5, X6, X7, X8) M(X1) TS_EXPAND(TS_FE_7(M, X2, X3, X4, X5, X6, X7, X8))
#define TS_FE_9(M, X1, X2, X3, X4, X5, X6, X7, X8, X9) M(X1) TS_EXPAND(TS_FE_8(M, X2, X3, X4, X5, X6, X7, X8, X9))
#define TS_FE_10(M, X1, X2, X3, X4, X5, X6, X7, X8, X9, X10) M(X1) TS_EXPAND(TS_FE_9(M, X2, X3, X4, X5, X6, X7, X8, X9, X10))
#define TS_FE_11(M, X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11) M(X1) TS_EXPAND(TS_FE_10(M, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11))
#define TS_FE_12(M, X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12) M(X1) TS_EXPAND(TS_FE_11(M, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12))

#define TS_GET_MACRO(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12, NAME, ...) NAME
#define TS_FOR_EACH(M, ...) TS_EXPAND(TS_GET_MACRO(__VA_ARGS__, \
        TS_FE_12, TS_FE_11, TS_FE_10, TS_FE_9, TS_FE_8, TS_FE_7, TS_FE_6, TS_FE_5, TS_FE_4, TS_FE_3, TS_FE_2, TS_FE_1) (M, __VA_ARGS__))

// ---- main generators
#define GENERATE_TYPE_SETTINGS(Type, /* tuples... */ ...)                     \
template<>                                                                    \
struct type_settings<Type> : svh::settings_with_pop<Type> {                   \
    TS_FOR_EACH(TS_DECL_ONE, __VA_ARGS__)                                     \
};

#define GENERATE_TYPE_SETTINGS_WITH_BASES(Type, BASES /* (B1, B2, ...) */, /* tuples... */ ...) \
template<>                                                                    \
struct type_settings<Type> : svh::settings_with_pop<Type>, TS_UNPAREN BASES { \
    TS_FOR_EACH(TS_DECL_ONE, __VA_ARGS__)                                     \
};
// ===================== end: type_settings generator (v2) =====================
