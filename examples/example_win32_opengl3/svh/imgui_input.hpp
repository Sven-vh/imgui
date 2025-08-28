#pragma once
#include "defines.hpp"
#include "tag_invoke.hpp"

#include <string>
#include <unordered_map>
#include <typeindex>
#include <any>
#include <memory>
#include <stdexcept>

/* Imgui Translations */
namespace svh {
    enum class imgui_input_type {
        input,
        slider,
        drag
    };
}

namespace svh {

    // 1) Forward decl
    template<typename T> struct scope;
    struct scope_handle;

    // 2) Base node (unchanged idea)
    struct scope_base : std::enable_shared_from_this<scope_base> {
        std::weak_ptr<scope_base> parent;
        std::unordered_map<std::type_index, std::shared_ptr<scope_base>> children;

        std::shared_ptr<scope_base> parent_ptr() const {
            auto p = parent.lock();
            if (!p) throw std::runtime_error("Cannot pop root scope");
            return p;
        }

        template<typename U>
        std::shared_ptr<scope_base> ensure_child() {
            auto key = std::type_index(typeid(U));
            auto it = children.find(key);
            if (it == children.end()) {
                auto child = std::make_shared<scope<U>>();
                child->parent = this->shared_from_this();
                it = children.emplace(key, child).first;
            }
            return it->second;
        }
    };

    // 3) Settings base that provides .pop(), as you wanted
    template<typename T>
    struct settings_with_pop {
    private:
        scope<T>* __owner = nullptr;
        friend struct scope<T>;
        void __attach(scope<T>* o) { __owner = o; }
    public:
        // pop() returns an untyped handle at the parent
        scope_handle pop();
    };
}

/* Per-type settings (only what that types need) */
template <class T>
struct type_settings : svh::settings_with_pop<T> {};

namespace svh {

    // 4) Node with attached settings (your specializations inherit settings_with_pop<T>)
    template<typename T>
    struct scope : scope_base {
        type_settings<T> settings;
        scope() { settings.__attach(this); }
    };

    // 5) Untyped handle
    struct scope_handle {
        std::shared_ptr<scope_base> n;

        template<typename U>
        struct typed_scope_handle {
            std::shared_ptr<scope<U>> n;

            type_settings<U>& settings() { return n->settings; }

            // ctor wires the reference to the node's settings
            explicit typed_scope_handle(std::shared_ptr<scope<U>> node)
                : n(std::move(node)) {
            }

            template<typename V>
            typed_scope_handle<V> push() {
                auto child = n->scope_base::ensure_child<V>();
                return typed_scope_handle<V>{ std::static_pointer_cast<scope<V>>(child) };
            }
            scope_handle pop() { return scope_handle{ n->scope_base::parent_ptr() }; }
        };

        template<typename U>
        typed_scope_handle<U> push() {
            auto child = n->ensure_child<U>();
            return typed_scope_handle<U>{ std::static_pointer_cast<scope<U>>(child) };
        }

        scope_handle pop() { return scope_handle{ n->parent_ptr() }; }
    };

    // 6) Now wire settings_with_pop<T>::pop() to return scope_handle
    template<typename T>
    inline scope_handle settings_with_pop<T>::pop() {
        if (!__owner) throw std::runtime_error("Settings not attached to a scope node");
        return scope_handle{ __owner->scope_base::parent_ptr() };
    }

    // A small helper to create a root node
    inline scope_handle make_root() {
        struct root_tag {};
        auto root = std::make_shared<scope<root_tag>>();
        return scope_handle{ root };
    }
}

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
