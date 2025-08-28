#pragma once
#include "defines.hpp"
#include "tag_invoke.hpp"

#include <string>
#include <unordered_map>
#include <typeindex>
#include <any>

/* Imgui Translations */
namespace svh {
    enum class imgui_input_type {
        input,
        slider,
        drag
    };
}

/* Per-type settings (only what that types need) */
template <class T>
struct type_settings {};

/* Public builder */
namespace svh {
    struct imgui_result {
    private:
        bool changed = false;
    public:
        void set_changed(const bool val) { changed |= val; }
        bool has_changed() const { return changed; }
    };

    struct default_imgui_settings {
        int decimal_precision = 3;
        imgui_input_type default_type = imgui_input_type::input;
    };

    class builder {
    public:

    };

    struct imgui_context {
    private:
        default_imgui_settings settings;
        std::unordered_map<std::type_index, std::any> by_type;

    public: /* default setting API */
        imgui_context& decimal_precision(const int val) { settings.decimal_precision = val; return *this; }
        imgui_context& default_type(const imgui_input_type val) { settings.default_type = val; return *this; }

        int get_decimal_precision() const { return settings.decimal_precision; }
        imgui_input_type get_default_type() const { return settings.default_type; }

    public: /* per-type setting API */
        // mutable get(): ensure + return
        template<typename T>
        type_settings<key_t<T>>& get() {
            using U = key_t<T>;
            static_assert(std::is_default_constructible_v<type_settings<U>>, "type_settings<U> must be default-constructible");
            auto key = std::type_index(typeid(U));
            auto it = by_type.find(key);
            if (it == by_type.end()) {
                it = by_type.emplace(key, type_settings<U>{}).first;
            }
            return std::any_cast<type_settings<U>&>(it->second);
        }

        // const get(): no insertion; return const default instance if absent
        template<typename T>
        const type_settings<key_t<T>>& get() const {
            using U = key_t<T>;
            static_assert(std::is_default_constructible_v<type_settings<U>>, "type_settings<U> must be default-constructible");
            auto key = std::type_index(typeid(U));
            auto it = by_type.find(key);
            if (it == by_type.end()) {
                static const type_settings<U> default_instance{};
                return default_instance;
            }
            return std::any_cast<const type_settings<U>&>(it->second);
        }

        template<typename T>
        type_settings<T>& add() {
            return get<T>();
        }

        template<typename T>
        bool has() const {
            using U = key_t<T>;
            return by_type.find(std::type_index(typeid(U))) != by_type.end();
        }

        template<typename T>
        imgui_context& remove() {
            using U = key_t<T>;
            by_type.erase(std::type_index(typeid(U)));
            return *this;
        }
    };

    class imgui_input {
    public:
        imgui_input() = delete;
        imgui_input(const imgui_input&) = delete;

        template<typename T>
        static imgui_result submit(T& value, const std::string& name, imgui_context& ctx);
    };
}



/* Reflection based input system */
namespace svh {
    struct imgui_input_t { /* Public CPO */ };
    inline constexpr imgui_input_t input{};

    struct imgui_input_lib_t { /* Library defaults */ };
    inline constexpr imgui_input_lib_t input_lib{};

    // Standalone function to replace the lambda in visit_struct::context<input_type_context>::for_each
    template<typename T>
    void imgui_input_visit_field(T& value, const std::string&, imgui_context& ctx, imgui_result& result) {
        visit_struct::context<input_type_context>::for_each(value, [&](const char* name, auto& field) {
            imgui_input_impl(field, name, ctx, result); // recurse
            });
    }

    template<typename T>
    void imgui_input_impl(T& value, const std::string& name, imgui_context& ctx, imgui_result& result) {
        if constexpr (is_tag_invocable_v<imgui_input_t, T&, std::string&, imgui_context&, imgui_result&>) {
            // 1) user
            tag_invoke(input, value, name, ctx, result);
        } else if constexpr (is_tag_invocable_v<imgui_input_lib_t, T&, std::string&, imgui_context&, imgui_result&>) {
            // 2) library defaults
            tag_invoke(input_lib, value, name, ctx, result);
        } else if constexpr (is_visitable_v<T, input_type_context>) {
            // 3) reflect
            imgui_input_visit_field(value, name, ctx, result);
        } else {
            // 4) error
            static_assert(always_false<T>::value, "No tag_invoke function found for this type/args.");
        }
    }
}

/* Implementation */
namespace svh {
    template<typename T>
    inline imgui_result imgui_input::submit(T& value, const std::string& name, imgui_context& ctx) {
        imgui_result result;
        imgui_input_impl<T>(value, name, ctx, result);
        return result;
    }
}
