#pragma once
#include "defines.hpp"
#include "tag_invoke.hpp"

#include <string>
#include <unordered_map>
#include <typeindex>
#include <any>
#include <memory>
#include <stdexcept>

namespace svh {

    template<typename T> struct scope;
    struct scope_handle;

    struct scope_base : std::enable_shared_from_this<scope_base> {
        std::weak_ptr<scope_base> parent;
        std::unordered_map<std::type_index, std::shared_ptr<scope_base>> children;

        std::shared_ptr<scope_base> parent_ptr() const {
            auto p = parent.lock();
            if (!p) throw std::runtime_error("Cannot pop root scope");
            return p;
        }

        template<typename U>
        std::shared_ptr<scope_base> ensure_child(const bool check_ancestors) {
            auto key = std::type_index(typeid(U));
            auto it = children.find(key);
            if (it != children.end()) return it->second;

            auto child = std::make_shared<scope<U>>();

            if (check_ancestors) {
                if (auto seed = find_in_ancestors<U>()) {
                    child->copy_settings_from(*seed);
                }
            }

            child->parent = this->shared_from_this();
            it = children.emplace(key, child).first;
            return it->second;
        }

        template<typename U>
        std::shared_ptr<scope<U>> find_local_child() const {
            auto key = std::type_index(typeid(U));
            auto it = children.find(key);
            if (it == children.end()) return nullptr;
            return std::static_pointer_cast<scope<U>>(it->second);
        }

        template<typename U>
        std::shared_ptr<scope<U>> find_in_ancestors() const {
            auto p = parent.lock();
            while (p) {
                if (auto found = p->find_local_child<U>()) return found;
                p = p->parent.lock();
            }
            return nullptr;
        }

        template<typename U>
        std::shared_ptr<scope<U>> find_nearest() const {
            if (auto here = find_local_child<U>()) return here;
            return find_in_ancestors<U>();
        }
    };

    template<typename T>
    struct settings_with_pop {
    private:
        scope<T>* __owner = nullptr;
        friend struct scope<T>;
        void __attach(scope<T>* o) { __owner = o; }
    public:
        scope_handle pop();
    };
}


namespace svh {
    template <class T>
    struct type_settings : svh::settings_with_pop<T> {};

    template<typename T>
    struct scope : scope_base {
        type_settings<T> settings;

        scope() { settings.__attach(this); }

        void copy_settings_from(const scope<T>& other) {
            settings = other.settings;
            settings.__attach(this);
        }
    };

    struct scope_handle {
        std::shared_ptr<scope_base> n;

        template<typename U>
        struct typed_scope_handle {
            std::shared_ptr<scope<U>> n;

            operator scope_handle() const { return scope_handle{ n }; }
            operator scope_handle& () { return *reinterpret_cast<scope_handle*>(this); }
            scope_handle as_handle()  const { return scope_handle{ n }; }

            type_settings<U>& settings() { return n->settings; }
            const type_settings<U>& settings() const { return n->settings; }

            explicit typed_scope_handle(std::shared_ptr<scope<U>> node)
                : n(std::move(node)) {
            }

            template<typename V>
            typed_scope_handle<V> push() {
                auto child = n->scope_base::ensure_child<V>(true);
                return typed_scope_handle<V>{ std::static_pointer_cast<scope<V>>(child) };
            }

            template<typename V>
            typed_scope_handle<V> push_default() {
                auto child = n->scope_base::ensure_child<V>(false);
                return typed_scope_handle<V>{ std::static_pointer_cast<scope<V>>(child) };
            }

            scope_handle pop() { return scope_handle{ n->scope_base::parent_ptr() }; }

            template<typename V>
            typed_scope_handle<V> use() const {
                auto child = n->scope_base::find_local_child<V>();
                if (!child) throw std::runtime_error("use<V>: child not found");
                return typed_scope_handle<V>{ child };
            }

            template<typename V>
            typed_scope_handle<V> nearest() const {
                auto found = n->scope_base::find_nearest<V>();
                if (!found) throw std::runtime_error("nearest<V>: not found in current or ancestors");
                return typed_scope_handle<V>{ found };
            }
        };

        template<typename U>
        typed_scope_handle<U> push() {
            auto child = n->ensure_child<U>(true);
            return typed_scope_handle<U>{ std::static_pointer_cast<scope<U>>(child) };
        }

        template<typename U>
        typed_scope_handle<U> push_default() {
            auto child = n->ensure_child<U>(false);
            return typed_scope_handle<U>{ std::static_pointer_cast<scope<U>>(child) };
        }

        scope_handle pop() { return scope_handle{ n->parent_ptr() }; }

        template<typename U>
        typed_scope_handle<U> use() const {
            auto found = n->find_nearest<U>();
            if (!found) throw std::runtime_error("nearest<U>: not found in current or ancestors");
            return typed_scope_handle<U>{ found };
        }

        template<typename U>
        typed_scope_handle<U> use() {
            auto found = n->find_nearest<U>();
            if (!found) {
                return push_default<U>();
            }
            return typed_scope_handle<U>{ found };
        }

        template<typename U>
        typed_scope_handle<U> as() const {
            auto cur = std::dynamic_pointer_cast<scope<U>>(n);
            if (!cur) throw std::runtime_error("as<U>: current node is not requested type");
            return typed_scope_handle<U>{ cur };
        }
    };

    template<typename T>
    inline scope_handle settings_with_pop<T>::pop() {
        if (!__owner) throw std::runtime_error("Settings not attached to a scope node");
        return scope_handle{ __owner->scope_base::parent_ptr() };
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

    struct default_imgui_settings {};

    template <>
    struct type_settings<default_imgui_settings> : svh::settings_with_pop<default_imgui_settings> {
    };

    using imgui_context = svh::scope_handle;

    inline scope_handle make_ctx() {
        auto root = std::make_shared<scope<default_imgui_settings>>();
        return scope_handle{ root };
    }

    class imgui_input {
    public:
        imgui_input() = delete;
        imgui_input(const imgui_input&) = delete;

        template<typename T>
        static imgui_result submit(T& value, const std::string& name, imgui_context& ctx);

        // visit struct only
        template<typename T>
        static auto submit(T& value, imgui_context& ctx)
            -> std::enable_if_t<is_visitable_v<T, input_type_context>, imgui_result> {
            auto name = visit_struct::context<input_type_context>::get_name<T>();
            name = name ? name : "UNKNOWN";
            return submit(value, name, ctx);
        }
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
    void imgui_input_visit_field(T& value, const std::string& name, imgui_context& ctx, imgui_result& result) {
        ImGui::Text("%s", name.c_str());
        visit_struct::context<input_type_context>::for_each(value, [&](const char* name, auto& field) {
            imgui_input_impl(field, name, ctx, result); // recurse
            });
    }

    template<typename T>
    void imgui_input_impl(T& value, const std::string& name, imgui_context& ctx, imgui_result& result) {
        //TODO: let the context know we are entering a new scope
        auto new_ctx = ctx.use<T>();
        if constexpr (is_tag_invocable_v<imgui_input_t, T&, std::string&, imgui_context&, imgui_result&>) {
            // 1) user
            tag_invoke(input, value, name, new_ctx, result);
        } else if constexpr (is_tag_invocable_v<imgui_input_lib_t, T&, std::string&, imgui_context&, imgui_result&>) {
            // 2) library defaults
            tag_invoke(input_lib, value, name, new_ctx, result);
        } else if constexpr (is_visitable_v<T, input_type_context>) {
            // 3) reflect
            imgui_input_visit_field(value, name, new_ctx, result);
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
