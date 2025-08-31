#pragma once
#include "imgui_input.hpp"

#include "imgui.h"

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

    /* ======================== Format Settings ======================== */
    template<typename T>
    struct format_settings {
        constexpr char* get_default_format() const {
            if constexpr (std::is_integral_v<T>) return "%d";
            else if constexpr (std::is_floating_point_v<T>) return "%.3f";
            else return "%s";
        }

        std::string _prefix = "";
        std::string _format = get_default_format();
        std::string _suffix = "";

        type_settings<T>& prefix(const std::string& val) { _prefix = val; return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& suffix(const std::string& val) { _suffix = val; return static_cast<type_settings<T>&>(*this); }

        type_settings<T>& as_int(const int digits = 0) { _format = "%0" + std::to_string(digits) + "d"; return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& as_float(const int precision = 2) { _format = "%." + std::to_string(precision) + "f"; return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& as_double(const int precision = 2) { _format = "%." + std::to_string(precision) + "lf"; return static_cast<type_settings<T>&>(*this); }

        type_settings<T>& as_decimal() { _format = "%d"; return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& as_hex() { _format = "%x"; return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& as_hex_upper() { _format = "%X"; return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& as_octal() { _format = "%o"; return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& as_binary() { _format = "%b"; return static_cast<type_settings<T>&>(*this); } // Note: %b is not standard in printf, may need custom handling

        type_settings<T>& clear_format() { _format = ""; return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& default_format() { _format = get_default_format(); return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& format(const std::string& val) { _format = val; return static_cast<type_settings<T>&>(*this); }
        //TODO: maybe add more format helpers like as decimal, hex, scientific, percentage, etc.

        std::string get_format(const std::string& default_format = "") const {
            if (_format.empty()) {
                if (!default_format.empty()) return _prefix + default_format + _suffix;
                return _prefix + get_default_format() + _suffix;
            }
            return _prefix + _format + _suffix;
        }
    };

    /* ======================== Input Flags ======================== */
    template<typename T>
    struct input_flags {
        ImGuiInputTextFlags _flags = 0;

        inline void toggle_flag(const bool value, const ImGuiInputTextFlags flag) {
            if (value) _flags |= flag;
            else _flags &= ~flag;
        }

        type_settings<T>& chars_decimal(const bool enable) { toggle_flag(enable, ImGuiInputTextFlags_CharsDecimal); return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& chars_hexadecimal(const bool enable) { toggle_flag(enable, ImGuiInputTextFlags_CharsHexadecimal); return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& chars_scientific(const bool enable) { toggle_flag(enable, ImGuiInputTextFlags_CharsScientific); return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& chars_uppercase(const bool enable) { toggle_flag(enable, ImGuiInputTextFlags_CharsUppercase); return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& chars_no_blank(const bool enable) { toggle_flag(enable, ImGuiInputTextFlags_CharsNoBlank); return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& allow_tab_input(const bool enable) { toggle_flag(enable, ImGuiInputTextFlags_AllowTabInput); return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& enter_returns_true(const bool enable) { toggle_flag(enable, ImGuiInputTextFlags_EnterReturnsTrue); return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& escape_clears_all(const bool enable) { toggle_flag(enable, ImGuiInputTextFlags_EscapeClearsAll); return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& ctrl_enter_for_new_line(const bool enable) { toggle_flag(enable, ImGuiInputTextFlags_CtrlEnterForNewLine); return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& read_only(const bool enable) { toggle_flag(enable, ImGuiInputTextFlags_ReadOnly); return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& password(const bool enable) { toggle_flag(enable, ImGuiInputTextFlags_Password); return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& always_overwrite(const bool enable) { toggle_flag(enable, ImGuiInputTextFlags_AlwaysOverwrite); return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& auto_select_all(const bool enable) { toggle_flag(enable, ImGuiInputTextFlags_AutoSelectAll); return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& parse_empty_ref_val(const bool enable) { toggle_flag(enable, ImGuiInputTextFlags_ParseEmptyRefVal); return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& display_empty_ref_val(const bool enable) { toggle_flag(enable, ImGuiInputTextFlags_DisplayEmptyRefVal); return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& no_horizontal_scroll(const bool enable) { toggle_flag(enable, ImGuiInputTextFlags_NoHorizontalScroll); return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& no_undo_redo(const bool enable) { toggle_flag(enable, ImGuiInputTextFlags_NoUndoRedo); return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& elide_left(const bool enable) { toggle_flag(enable, ImGuiInputTextFlags_ElideLeft); return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& callback_completion(const bool enable) { toggle_flag(enable, ImGuiInputTextFlags_CallbackCompletion); return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& callback_history(const bool enable) { toggle_flag(enable, ImGuiInputTextFlags_CallbackHistory); return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& callback_always(const bool enable) { toggle_flag(enable, ImGuiInputTextFlags_CallbackAlways); return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& callback_char_filter(const bool enable) { toggle_flag(enable, ImGuiInputTextFlags_CallbackCharFilter); return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& callback_resize(const bool enable) { toggle_flag(enable, ImGuiInputTextFlags_CallbackResize); return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& callback_edit(const bool enable) { toggle_flag(enable, ImGuiInputTextFlags_CallbackEdit); return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& input_flag(const ImGuiInputTextFlags flags) { _flags = flags; return static_cast<type_settings<T>&>(*this); }

        const ImGuiInputTextFlags& get_input_flags() const { return _flags; }
    };


    /* ======================== Slider/Drag Flags ======================== */
    template<typename T>
    struct slider_flags {
        ImGuiSliderFlags _flags = 0;

        inline void toggle_flag(const bool value, const ImGuiSliderFlags flag) {
            if (value) _flags |= flag;
            else _flags &= ~flag;
        }

        type_settings<T>& logarithmic(const bool enable) { toggle_flag(enable, ImGuiSliderFlags_Logarithmic); return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& no_round_to_format(const bool enable) { toggle_flag(enable, ImGuiSliderFlags_NoRoundToFormat); return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& no_input(const bool enable) { toggle_flag(enable, ImGuiSliderFlags_NoInput); return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& wrap_around(const bool enable) { toggle_flag(enable, ImGuiSliderFlags_WrapAround); return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& clamp_on_input(const bool enable) { toggle_flag(enable, ImGuiSliderFlags_ClampOnInput); return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& clamp_zero_range(const bool enable) { toggle_flag(enable, ImGuiSliderFlags_ClampZeroRange); return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& no_speed_tweaks(const bool enable) { toggle_flag(enable, ImGuiSliderFlags_NoSpeedTweaks); return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& always_clamp(const bool enable) { toggle_flag(enable, ImGuiSliderFlags_AlwaysClamp); return static_cast<type_settings<T>&>(*this); }
        type_settings<T>& slider_drag_flag(const ImGuiSliderFlags flags) { _flags = flags; return static_cast<type_settings<T>&>(*this); }

        const ImGuiSliderFlags& get_slider_drag_flags() const { return _flags; }
    };
}

namespace svh {

    /* ======================== int ======================== */
    GENERATE_TYPE_SETTINGS_WITH_BASES(int, (input_type_settings<int>, input_flags<int>, slider_flags<int>, format_settings<int>, min_max_settings<int, int>),
        FIELD(int, step, 1),
        FIELD(int, step_fast, 10)
    );

    inline void tag_invoke(svh::imgui_input_lib_t, int& value, const std::string& name, imgui_context& ctx, imgui_result& result) {
        auto& settings = ctx.use<int>().settings();
        bool changed = false;
        switch (settings.get_input_type()) {
        case input_type::input:
            changed = ImGui::InputInt(name.c_str(), &value, settings.get_step(), settings.get_step_fast(), settings.get_input_flags());
            break;
        case input_type::slider:
            changed = ImGui::SliderInt(name.c_str(), &value, settings.get_min(), settings.get_max(), settings.get_format("%d").c_str(), settings.get_input_flags());
            break;
        case input_type::drag:
            changed = ImGui::DragInt(name.c_str(), &value, settings.get_step(), settings.get_min(), settings.get_max(), settings.get_format("%d").c_str(), settings.get_input_flags());
            break;
        }
        result.set_changed(changed);
    }

    /* ======================== float ======================== */
    GENERATE_TYPE_SETTINGS_WITH_BASES(float, (svh::input_type_settings<float>, input_flags<float>, slider_flags<float>, format_settings<float>, svh::min_max_settings<float, float>),
        FIELD(float, step, 0.01f),
        FIELD(float, step_fast, 0.1f)
    );

    inline void tag_invoke(svh::imgui_input_lib_t, float& value, const std::string name, imgui_context& ctx, imgui_result result) {
        auto& settings = ctx.use<float>().settings();
        bool changed = false;
        switch (settings.get_input_type()) {
        case input_type::input:
            changed = ImGui::InputFloat(name.c_str(), &value, settings.get_step(), settings.get_step_fast(), settings.get_format("%.3f").c_str(), settings.get_input_flags());
            break;
        case input_type::slider:
            changed = ImGui::SliderFloat(name.c_str(), &value, settings.get_min(), settings.get_max(), settings.get_format("%.3f").c_str(), settings.get_slider_drag_flags());
            break;
        case input_type::drag:
            changed = ImGui::DragFloat(name.c_str(), &value, settings.get_step(), settings.get_min(), settings.get_max(), settings.get_format("%.3f").c_str(), settings.get_slider_drag_flags());
            break;
        }
        result.set_changed(changed);
    }

    /* ======================== bool ======================== */
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

    /* ======================== double ======================== */
    GENERATE_TYPE_SETTINGS_WITH_BASES(double, (svh::input_type_settings<double>, svh::min_max_settings<double, double>),
        FIELD(double, step, 0.01),
        FIELD(double, step_fast, 0.1),
        FIELD(int, decimal_precision, 3)
    );

    inline void tag_invoke(svh::imgui_input_lib_t, double& value, const std::string name, imgui_context& ctx, imgui_result result) {
        auto& double_settings = ctx.use<double>().settings();
        bool changed = false;
        switch (double_settings.get_input_type()) {
        case input_type::input:
            changed = ImGui::InputDouble(name.c_str(), &value, double_settings.get_step(), double_settings.get_step_fast(), ("%." + std::to_string(double_settings.get_decimal_precision()) + "f").c_str());
            break;
        case input_type::slider:
            changed = ImGui::SliderScalar(name.c_str(), ImGuiDataType_Double, &value, &double_settings.get_min(), &double_settings.get_max(), ("%." + std::to_string(double_settings.get_decimal_precision()) + "f").c_str());
            break;
        case input_type::drag:
            changed = ImGui::DragScalar(name.c_str(), ImGuiDataType_Double, &value, double_settings.get_step(), &double_settings.get_min(), &double_settings.get_max(), ("%." + std::to_string(double_settings.get_decimal_precision()) + "f").c_str());
            break;
        }
        result.set_changed(changed);
    }
}

static_assert(svh::is_tag_invocable_v<svh::imgui_input_lib_t, int&, std::string&, svh::imgui_context&, svh::imgui_result&>, "int tag_invoke not found");
