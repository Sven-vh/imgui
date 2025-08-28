#pragma once
#include "imgui.h"
#include "imgui_input.hpp"
#include "svh.hpp"

namespace svh {

    /* ======================== int ======================== */
    template<>
    struct type_settings<int> : svh::settings_with_pop<int> {
        int _min = 0;
        int _max = 100;
        int _step = 1;
        int _step_fast = 10;

        type_settings& min(const int val) { _min = val; return *this; }
        type_settings& max(const int val) { _max = val; return *this; }
        type_settings& step(const int val) { _step = val; return *this; }
        type_settings& step_fast(const int val) { _step_fast = val; return *this; }

        int get_min() const { return _min; }
        int get_max() const { return _max; }
        int get_step() const { return _step; }
        int get_step_fast() const { return _step_fast; }
    };

    inline void tag_invoke(svh::imgui_input_lib_t, int& value, const std::string& name, imgui_context& ctx, imgui_result& result) {
        //auto& settings = ctx.get<int>();
        //bool changed = false;
        //switch (ctx.get_default_type()) {
        //case imgui_input_type::input:
        //    changed = ImGui::InputInt(name.c_str(), &value, settings.get_step(), settings.get_step_fast());
        //    break;
        //case imgui_input_type::slider:
        //    changed = ImGui::SliderInt(name.c_str(), &value, settings.get_min(), settings.get_max());
        //    break;
        //case imgui_input_type::drag:
        //    changed = ImGui::DragInt(name.c_str(), &value, (float)settings.get_step(), settings.get_min(), settings.get_max());
        //    break;
        //}
        //result.set_changed(changed);
    }

    /* ======================== float ======================== */
    template<>
    struct type_settings<float> : svh::settings_with_pop<float> {
        float _min = 0.0f;
        float _max = 1.0f;
        float _step = 0.01f;
        float _step_fast = 0.1f;

        type_settings& min(const float val) { _min = val; return *this; }
        type_settings& max(const float val) { _max = val; return *this; }
        type_settings& step(const float val) { _step = val; return *this; }
        type_settings& step_fast(const float val) { _step_fast = val; return *this; }

        float get_min() const { return _min; }
        float get_max() const { return _max; }
        float get_step() const { return _step; }
        float get_step_fast() const { return _step_fast; }
    };

    inline void tag_invoke(svh::imgui_input_lib_t, float& value, const std::string name, imgui_context& ctx, imgui_result result) {
        //auto& settings = ctx.get<float>();
        //bool changed = false;
        //switch (ctx.get_default_type()) {
        //case imgui_input_type::input:
        //    changed = ImGui::InputFloat(name.c_str(), &value, settings.get_step(), settings.get_step_fast(), ("%" + std::to_string(ctx.get_decimal_precision()) + "f").c_str());
        //    break;
        //case imgui_input_type::slider:
        //    changed = ImGui::SliderFloat(name.c_str(), &value, settings.get_min(), settings.get_max(), ("%" + std::to_string(ctx.get_decimal_precision()) + "f").c_str());
        //    break;
        //case imgui_input_type::drag:
        //    changed = ImGui::DragFloat(name.c_str(), &value, settings.get_step(), settings.get_min(), settings.get_max(), ("%" + std::to_string(ctx.get_decimal_precision()) + "f").c_str());
        //    break;
        //}
        //result.set_changed(changed);
    }

    /* ======================== bool ======================== */
    template<>
    struct type_settings<bool> : svh::settings_with_pop<bool> {
        enum class bool_type {
            checkbox,
            button,
            dropdown
        };
        bool_type _type = bool_type::checkbox;
        std::string _true_label = "True";
        std::string _false_label = "False";

        type_settings& type(const bool_type val) { _type = val; return *this; }
        type_settings& true_label(const std::string& val) { _true_label = val; return *this; }
        type_settings& false_label(const std::string& val) { _false_label = val; return *this; }

        bool_type get_type() const { return _type; }
        const std::string& get_true_label() const { return _true_label; }
        const std::string& get_false_label() const { return _false_label; }
    };

    inline void tag_invoke(svh::imgui_input_lib_t, bool& value, const std::string name, imgui_context& ctx, imgui_result result) {
        //auto& settings = ctx.get<bool>();
        //bool changed = false;
        //switch (settings.get_type()) {
        //case type_settings<bool>::bool_type::checkbox:
        //    changed = ImGui::Checkbox(name.c_str(), &value);
        //    break;
        //case type_settings<bool>::bool_type::button:
        //    if (ImGui::Button(value ? settings.get_true_label().c_str() : settings.get_false_label().c_str())) {
        //        value = !value;
        //        changed = true;
        //    }
        //    break;
        //case type_settings<bool>::bool_type::dropdown:
        //    if (ImGui::BeginCombo(name.c_str(), value ? settings.get_true_label().c_str() : settings.get_false_label().c_str())) {
        //        if (ImGui::Selectable(settings.get_true_label().c_str(), value)) {
        //            value = true;
        //            changed = true;
        //        }
        //        if (ImGui::Selectable(settings.get_false_label().c_str(), !value)) {
        //            value = false;
        //            changed = true;
        //        }
        //        ImGui::EndCombo();
        //    }
        //    break;
        //}
        //result.set_changed(changed);
    }

    /* ======================== std::string ======================== */
    //template<>
}

static_assert(svh::is_tag_invocable_v<svh::imgui_input_lib_t, int&, std::string&, svh::imgui_context&, svh::imgui_result&>, "int tag_invoke not found");
