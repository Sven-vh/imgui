#pragma once
#include "imgui.h"
#include "imgui_input.hpp"
#include "svh.hpp"

namespace svh {

    /* ====================== Input Type ====================== */
    enum class input_type {
        input,
        slider,
        drag
    };

    template<typename T>
    struct input_type_settings {
        input_type _type = input_type::input;

        type_settings<T>& as_input() { _type = input_type::input; return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& as_slider() { _type = input_type::slider; return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& as_drag() { _type = input_type::drag; return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& type(const input_type val) { _type = val; return static_cast<type_settings<T>&>(*this); }

        const input_type& get_input_type() const { return _type; }
    };

    /* ====================== Min/Max Settings ====================== */
    template<typename T, typename V>
    struct min_max_settings {
        V _min = std::numeric_limits<V>::min();
        V _max = std::numeric_limits<V>::max();

        type_settings<T>& min(const V val) { _min = val; return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& max(const V val) { _max = val; return static_cast<type_settings<T>&>(*this); }

        const V& get_min() const { return _min; }
        const V& get_max() const { return _max; }
    };

    /* ======================== Bool Type ======================== */
    enum class bool_type {
        checkbox,
        button,
        radio,
        dropdown
    };

    template<typename T>
    struct bool_type_settings {
        bool_type _type = bool_type::checkbox;

        type_settings<T>& as_checkbox() { _type = bool_type::checkbox; return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& as_button() { _type = bool_type::button; return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& as_radio() { _type = bool_type::radio; return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& as_dropdown() { _type = bool_type::dropdown; return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& type(const bool_type val) { _type = val; return static_cast<type_settings<T>&>(*this); }

        const bool_type& get_bool_type() const { return _type; }
    };
}

namespace svh {

    /* ======================== int ======================== */

    GENERATE_TYPE_SETTINGS_WITH_BASES(int, (input_type_settings<int>, min_max_settings<int, int>),
        FIELD(int, step, 1),
        FIELD(int, step_fast, 10)
    );
    // Bascially generates:

    //template<>
    //struct type_settings<int> : svh::settings_with_pop<int>, svh::input_type_settings<int> {
    //    int _min = 0;
    //    int _max = 100;
    //    int _step = 1;
    //    int _step_fast = 10;

    //    type_settings& min(const int val) { _min = val; return *this; }
    //    type_settings& max(const int val) { _max = val; return *this; }
    //    type_settings& step(const int val) { _step = val; return *this; }
    //    type_settings& step_fast(const int val) { _step_fast = val; return *this; }

    //    int get_min() const { return _min; }
    //    int get_max() const { return _max; }
    //    int get_step() const { return _step; }
    //    int get_step_fast() const { return _step_fast; }
    //};

    inline void tag_invoke(svh::imgui_input_lib_t, int& value, const std::string& name, imgui_context& ctx, imgui_result& result) {
        auto& int_settings = ctx.use<int>().settings();
        bool changed = false;
        switch (int_settings.get_input_type()) {
        case input_type::input:
            changed = ImGui::InputInt(name.c_str(), &value, int_settings.get_step(), int_settings.get_step_fast());
            break;
        case input_type::slider:
            changed = ImGui::SliderInt(name.c_str(), &value, int_settings.get_min(), int_settings.get_max());
            break;
        case input_type::drag:
            changed = ImGui::DragInt(name.c_str(), &value, int_settings.get_step(), int_settings.get_min(), int_settings.get_max());
            break;
        }
        result.set_changed(changed);
    }

    /* ======================== float ======================== */
    //template<>
    //struct type_settings<float> : svh::settings_with_pop<float> {
    //    float _min = std::numeric_limits<float>::min();
    //    float _max = std::numeric_limits<float>::max();
    //    float _step = 0.01f;
    //    float _step_fast = 0.1f;
    //    int _decimal_precision = 3;

    //    type_settings& min(const float val) { _min = val; return *this; }
    //    type_settings& max(const float val) { _max = val; return *this; }
    //    type_settings& step(const float val) { _step = val; return *this; }
    //    type_settings& step_fast(const float val) { _step_fast = val; return *this; }
    //    type_settings& decimal_precision(const int val) { _decimal_precision = val; return *this; }

    //    float get_min() const { return _min; }
    //    float get_max() const { return _max; }
    //    float get_step() const { return _step; }
    //    float get_step_fast() const { return _step_fast; }
    //    int get_decimal_precision() const { return _decimal_precision; }
    //};

    GENERATE_TYPE_SETTINGS_WITH_BASES(float, (svh::input_type_settings<float>, svh::min_max_settings<float, float>),
        FIELD(float, step, 0.01f),
        FIELD(float, step_fast, 0.1f),
        FIELD(int, decimal_precision, 3)
    );

    inline void tag_invoke(svh::imgui_input_lib_t, float& value, const std::string name, imgui_context& ctx, imgui_result result) {
        auto& float_settings = ctx.use<float>().settings();
        bool changed = false;
        switch (float_settings.get_input_type()) {
        case input_type::input:
            changed = ImGui::InputFloat(name.c_str(), &value, float_settings.get_step(), float_settings.get_step_fast(), ("%." + std::to_string(float_settings.get_decimal_precision()) + "f").c_str());
            break;
        case input_type::slider:
            changed = ImGui::SliderFloat(name.c_str(), &value, float_settings.get_min(), float_settings.get_max(), ("%." + std::to_string(float_settings.get_decimal_precision()) + "f").c_str());
            break;
        case input_type::drag:
            changed = ImGui::DragFloat(name.c_str(), &value, float_settings.get_step(), float_settings.get_min(), float_settings.get_max(), ("%." + std::to_string(float_settings.get_decimal_precision()) + "f").c_str());
            break;
        }
        result.set_changed(changed);
    }

    /* ======================== bool ======================== */
    //template<>
    //struct type_settings<bool> : svh::settings_with_pop<bool> {
    //    enum class bool_type {
    //        checkbox,
    //        button,
    //        dropdown
    //    };
    //    bool_type _type = bool_type::checkbox;
    //    std::string _true_label = "True";
    //    std::string _false_label = "False";

    //    type_settings& type(const bool_type val) { _type = val; return *this; }
    //    type_settings& true_label(const std::string& val) { _true_label = val; return *this; }
    //    type_settings& false_label(const std::string& val) { _false_label = val; return *this; }

    //    bool_type get_type() const { return _type; }
    //    const std::string& get_true_label() const { return _true_label; }
    //    const std::string& get_false_label() const { return _false_label; }
    //};

    GENERATE_TYPE_SETTINGS_WITH_BASES(bool, (svh::bool_type_settings<bool>),
        FIELD(std::string, true_label, "True"),
        FIELD(std::string, false_label, "False")
    );

    inline void tag_invoke(svh::imgui_input_lib_t, bool& value, const std::string name, imgui_context& ctx, imgui_result result) {
        auto& bool_settings = ctx.use<bool>().settings();
        bool changed = false;
        switch (bool_settings.get_bool_type()) {
        case bool_type::checkbox:
            changed = ImGui::Checkbox(name.c_str(), &value);
            break;
        case bool_type::button:
            if (ImGui::Button((value ? bool_settings.get_true_label() : bool_settings.get_false_label()).c_str())) {
                value = !value;
                changed = true;
            }
            ImGui::SameLine();
            ImGui::Text("%s", name.c_str());
            break;
        case bool_type::radio:
            ImGui::Text("%s", name.c_str());
            ImGui::SameLine();
            if (ImGui::RadioButton(bool_settings.get_true_label().c_str(), value)) {
                value = true;
                changed = true;
            }
            ImGui::SameLine();
            if (ImGui::RadioButton(bool_settings.get_false_label().c_str(), !value)) {
                value = false;
                changed = true;
            }
            break;
        case bool_type::dropdown:
            if (ImGui::BeginCombo(name.c_str(), (value ? bool_settings.get_true_label() : bool_settings.get_false_label()).c_str())) {
                if (ImGui::Selectable(bool_settings.get_true_label().c_str(), value)) {
                    value = true;
                    changed = true;
                }
                if (ImGui::Selectable(bool_settings.get_false_label().c_str(), !value)) {
                    value = false;
                    changed = true;
                }
                ImGui::EndCombo();
            }
            break;
        }
        result.set_changed(changed);
    }

    /* ======================== std::string ======================== */
    //template<>
}

static_assert(svh::is_tag_invocable_v<svh::imgui_input_lib_t, int&, std::string&, svh::imgui_context&, svh::imgui_result&>, "int tag_invoke not found");
