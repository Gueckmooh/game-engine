#pragma once

#include <macros/macros.hpp>

#define $pimpl_decl                             \
    class Impl;                                 \
    std::unique_ptr<Impl> fpImpl;
#define $pimpl_class(Class, ...)                                        \
    Class::Class($parameter_merge(__VA_ARGS__)) : fpImpl(std::make_unique<Impl>($parameter_drop_types(__VA_ARGS__))){} \
    Class::~Class() {}

#define $pimpl_method_definition(Class, type, name, ...) type Class::name($parameter_merge(__VA_ARGS__))
#define $pimpl_method_implementation(Class, type, name, ...)            \
    $pimpl_method_implementation_return(type) fpImpl->name($parameter_drop_types(__VA_ARGS__));


#define $pimpl_method(...)                                              \
    $pimpl_method_definition(__VA_ARGS__) { $pimpl_method_implementation(__VA_ARGS__) }

#define $pimpl_method_const(...)                                                                                                 \
    $pimpl_method_definition(__VA_ARGS__) const { $pimpl_method_implementation(__VA_ARGS__) }

#define $pimpl_method_implementation_return(type) $cat($pimpl_method_implementation_return_, $type_void(type))
#define $pimpl_method_implementation_return_0 return
#define $pimpl_method_implementation_return_1

// $pimpl_whatever_v means it will be by value
#define $pimpl_attribute(Class, type, name) $pimpl_method_const(Class, const type&, name);
#define $pimpl_attribute_v(Class, type, name) $pimpl_method_const(Class, type, name);

#define $pimpl_property(Class, type, name)                                                                                       \
    $pimpl_method_const(Class, const type&, name);                                                                               \
    $pimpl_method(Class, void, name, const type&, _##name);
#define $pimpl_property_v(Class, type, name)                                                                                     \
    $pimpl_method_const(Class, type, name);                                                                                      \
    $pimpl_method(Class, void, name, type, _##name);
#define $pimpl_property_cast_v(Class, CastClass, type, name)                                                                     \
    $pimpl_method_cast_const(Class, CastClass, type, name);                                                                      \
    $pimpl_method_cast(Class, CastClass, void, name, type, _##name);
