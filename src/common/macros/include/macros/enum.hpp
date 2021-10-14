#pragma once

#include <macros/macros/common.hpp>

// #include <ostream>

/**
 * Generates an enum class with stringify functions.
 *
 * ```c++
 * $enum_class(lava, Power,
 *      Low,
 *      High,
 * );
 * ```
 *
 * will generate:
 *
 * ```c++
 * namespace lava {
 *     enum class Power {
 *         Low,
 *         High,
 *     };
 * };
 *
 * constexpr const char* stringify(lava::Power value)
 * {
 *     switch (value) {
 *     case lava::Power::Low: {
 *         return "lava::Power::Low";
 *     };
 *     case lava::Power::High: {
 *         return "lava::Power::High";
 *     };
 *     default: break;
 *     }
 *     return "lava::Power::<Unknown>";
 * };
 *
 * inline std::ostream& operator<<(std::ostream& os, lava::Power value)
 * {
 *     os << stringify(value);
 *     return os;
 * };
 * ```
 *
 */
#define $enum_class_inheritance(Namespace, BaseEnum, Enum, ...)                          \
    $enum_class_inheritance_(Namespace, Enum, __VA_ARGS__);                              \
    $enum_stringify(Namespace, Enum, __VA_ARGS__);                                       \
    $enum_ostream(Namespace, Enum);

#define $enum_class_inheritance_(Namespace, BaseEnum, Enum, ...)                         \
    namespace Namespace {                                                                \
    enum class Enum : public BaseEnum { __VA_ARGS__ };                                   \
    }

#define $enum_class_nonamespace(Namespace, Enum, ...)                                    \
    $enum_class_nonamespace_(Enum, __VA_ARGS__);                                         \
    $enum_stringify(Namespace, Enum, __VA_ARGS__);                                       \
    $enum_ostream(Namespace, Enum);

#define $enum_class_nonamespace_(Enum, ...) enum class Enum { __VA_ARGS__ };

#define $enum_class(Namespace, Enum, ...)                                                \
    $enum_class_(Namespace, Enum, __VA_ARGS__);                                          \
    $enum_stringify(Namespace, Enum, __VA_ARGS__);                                       \
    $enum_ostream(Namespace, Enum);
#define $enum_class_(Namespace, Enum, ...)                                               \
    namespace Namespace {                                                                \
    enum class Enum { __VA_ARGS__ };                                                     \
    }

#define $enum_stringify(Namespace, Enum, ...)                                            \
    constexpr const char* stringify(Namespace::Enum value) {                             \
        switch (value) {                                                                 \
            $enum_stringify_cases(Namespace, Enum, __VA_ARGS__);                         \
        default: break;                                                                  \
        }                                                                                \
        return #Enum "::<Unknown>";                                                      \
    }

#define $enum_stringify_cases(Namespace, Enum, ...)                                      \
    $eval($enum_stringify_cases_(Namespace, Enum, __VA_ARGS__))
#define $enum_stringify_cases_(Namespace, Enum, ...)                                     \
    $enum_stringify_case_($are_arguments_empty(__VA_ARGS__), Namespace, Enum, __VA_ARGS__)
#define $enum_stringify_cases_indirect() $enum_stringify_cases_

#define $enum_stringify_case_(n, Namespace, Enum, ...)                                   \
    $cat($enum_stringify_case_, n)(Namespace, Enum, __VA_ARGS__)
#define $enum_stringify_case_0(Namespace, Enum, ...) /* Empty end case */
#define $enum_stringify_case_1(Namespace, Enum, Value, ...)                              \
    $enum_stringify_case(Namespace, Enum, Value);                                        \
    $obstruct($enum_stringify_cases_indirect)()(Namespace, Enum, __VA_ARGS__)

#define $enum_stringify_case(Namespace, Enum, Value)                                     \
    $cat($enum_stringify_case_check_, $is_string_empty(Value))(Namespace, Enum, Value)
#define $enum_stringify_case_check_1(...) /* Empty end case */
#define $enum_stringify_case_check_0(Namespace, Enum, Value)                             \
    case Namespace::Enum::Value: {                                                       \
        return #Enum "::" #Value;                                                        \
    }

#define $enum_ostream(Namespace, Enum)                                                   \
    inline std::ostream& operator<<(std::ostream& os, Namespace::Enum value) {           \
        os << stringify(value);                                                          \
        return os;                                                                       \
    }

// enum_to_vars
#define $enum_to_vars_decls(prefix, ...) $eval($enum_to_vars_decls_(prefix, __VA_ARGS__))
#define $enum_to_vars_decls_(prefix, ...)                                                \
    $enum_to_vars_decl_($are_arguments_empty(__VA_ARGS__), prefix, __VA_ARGS__)
#define $enum_to_vars_decls_indirect() $enum_to_vars_decls_

#define $enum_to_vars_decl_(n, prefix, ...)                                              \
    $cat($enum_to_vars_decl_, n)(prefix, __VA_ARGS__)
#define $enum_to_vars_decl_0(prefix, ...) /* sink */
#define $enum_to_vars_decl_1(prefix, name, ...)                                          \
    $enum_to_vars_decl(prefix, name);                                                    \
    $obstruct($enum_to_vars_decls_indirect)()(prefix, __VA_ARGS__)

#define $enum_to_vars_decl(prefix, name) bool $cat(prefix, name)

// enum_to_vars_setter
#define $enum_to_vars_setter(name, Namespace, Enum, prefix, ...)                         \
    void name(Namespace::Enum value) {                                                   \
        switch (value) {                                                                 \
            $enum_to_vars_setter_cases(Namespace, Enum, prefix, __VA_ARGS__);            \
        default: break;                                                                  \
        }                                                                                \
    }                                                                                    \
    void name(Namespace::Enum var, bool value) {                                         \
        switch (var) {                                                                   \
            $enum_to_vars_setter_value_cases(Namespace, Enum, prefix, __VA_ARGS__);      \
        default: break;                                                                  \
        }                                                                                \
    }

#define $enum_to_vars_setter_cases(Namespace, Enum, prefix, ...)                         \
    $eval($enum_to_vars_setter_cases_(Namespace, Enum, prefix, __VA_ARGS__))
#define $enum_to_vars_setter_cases_(Namespace, Enum, prefix, ...)                        \
    $enum_to_vars_setter_case_($are_arguments_empty(__VA_ARGS__), Namespace, Enum,       \
                               prefix, __VA_ARGS__)
#define $enum_to_vars_setter_cases_indirect() $enum_to_vars_setter_cases_

#define $enum_to_vars_setter_case_(n, Namespace, Enum, prefix, ...)                      \
    $cat($enum_to_vars_setter_case_, n)(Namespace, Enum, prefix, __VA_ARGS__)
#define $enum_to_vars_setter_case_0(Namespace, Enum, prefix, ...) /* sink */
#define $enum_to_vars_setter_case_1(Namespace, Enum, prefix, name, ...)                  \
    $enum_to_vars_setter_case(Namespace, Enum, prefix, name);                            \
    $obstruct($enum_to_vars_setter_cases_indirect)()(Namespace, Enum, prefix, __VA_ARGS__)

#define $enum_to_vars_setter_case(Namespace, Enum, prefix, name)                         \
    $cat($enum_to_vars_setter_case_check_, $is_string_empty(name))(Namespace, Enum,      \
                                                                   prefix, name)
#define $enum_to_vars_setter_case_check_1(...) /* Empty end case */
#define $enum_to_vars_setter_case_check_0(Namespace, Enum, prefix, name)                 \
    case Namespace::Enum::name: {                                                        \
        $cat(prefix, name) = true;                                                       \
        break;                                                                           \
    }

#define $enum_to_vars_setter_value_cases(Namespace, Enum, prefix, ...)                   \
    $eval($enum_to_vars_setter_value_cases_(Namespace, Enum, prefix, __VA_ARGS__))
#define $enum_to_vars_setter_value_cases_(Namespace, Enum, prefix, ...)                  \
    $enum_to_vars_setter_value_case_($are_arguments_empty(__VA_ARGS__), Namespace, Enum, \
                                     prefix, __VA_ARGS__)
#define $enum_to_vars_setter_value_cases_indirect() $enum_to_vars_setter_value_cases_

#define $enum_to_vars_setter_value_case_(n, Namespace, Enum, prefix, ...)                \
    $cat($enum_to_vars_setter_value_case_, n)(Namespace, Enum, prefix, __VA_ARGS__)
#define $enum_to_vars_setter_value_case_0(Namespace, Enum, prefix, ...) /* sink */
#define $enum_to_vars_setter_value_case_1(Namespace, Enum, prefix, name, ...)            \
    $enum_to_vars_setter_value_case(Namespace, Enum, prefix, name);                      \
    $obstruct($enum_to_vars_setter_value_cases_indirect)()(Namespace, Enum, prefix,      \
                                                           __VA_ARGS__)

#define $enum_to_vars_setter_value_case(Namespace, Enum, prefix, name)                   \
    $cat($enum_to_vars_setter_value_case_check_,                                         \
         $is_string_empty(name))(Namespace, Enum, prefix, name)
#define $enum_to_vars_setter_value_case_check_1(...) /* Empty end case */
#define $enum_to_vars_setter_value_case_check_0(Namespace, Enum, prefix, name)           \
    case Namespace::Enum::name: {                                                        \
        $cat(prefix, name) = value;                                                      \
        break;                                                                           \
    }

// enum_to_vars_getter
#define $enum_to_vars_getter(name, Namespace, Enum, prefix, ...)                         \
    bool name(Namespace::Enum value) {                                                   \
        switch (value) {                                                                 \
            $enum_to_vars_getter_cases(Namespace, Enum, prefix, __VA_ARGS__);            \
        default: break;                                                                  \
        }                                                                                \
        return false;                                                                    \
    }

#define $enum_to_vars_getter_cases(Namespace, Enum, prefix, ...)                         \
    $eval($enum_to_vars_getter_cases_(Namespace, Enum, prefix, __VA_ARGS__))
#define $enum_to_vars_getter_cases_(Namespace, Enum, prefix, ...)                        \
    $enum_to_vars_getter_case_($are_arguments_empty(__VA_ARGS__), Namespace, Enum,       \
                               prefix, __VA_ARGS__)
#define $enum_to_vars_getter_cases_indirect() $enum_to_vars_getter_cases_

#define $enum_to_vars_getter_case_(n, Namespace, Enum, prefix, ...)                      \
    $cat($enum_to_vars_getter_case_, n)(Namespace, Enum, prefix, __VA_ARGS__)
#define $enum_to_vars_getter_case_0(Namespace, Enum, prefix, ...) /* sink */
#define $enum_to_vars_getter_case_1(Namespace, Enum, prefix, name, ...)                  \
    $enum_to_vars_getter_case(Namespace, Enum, prefix, name);                            \
    $obstruct($enum_to_vars_getter_cases_indirect)()(Namespace, Enum, prefix, __VA_ARGS__)

#define $enum_to_vars_getter_case(Namespace, Enum, prefix, name)                         \
    $cat($enum_to_vars_getter_case_check_, $is_string_empty(name))(Namespace, Enum,      \
                                                                   prefix, name)
#define $enum_to_vars_getter_case_check_1(...) /* Empty end case */
#define $enum_to_vars_getter_case_check_0(Namespace, Enum, prefix, name)                 \
    case Namespace::Enum::name: {                                                        \
        return $cat(prefix, name);                                                       \
    }
