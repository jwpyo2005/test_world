///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) Crossbar.io Technologies GmbH and contributors
//
// Boost Software License - Version 1.0 - August 17th, 2003
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
///////////////////////////////////////////////////////////////////////////////

#include "wamp_message_type.hpp"

#include <stdexcept>

namespace autobahn {

inline wamp_message::wamp_message(std::size_t num_fields)
    : m_zone()
    , m_fields(num_fields)
{
}

inline wamp_message::wamp_message(std::size_t num_fields, msgpack::zone&& zone)
    : m_zone(std::move(zone))
    , m_fields(num_fields)
{
}

inline wamp_message::wamp_message(message_fields&& fields, msgpack::zone&& zone)
    : m_zone(std::move(zone))
    , m_fields(std::move(fields))
{
}

inline wamp_message::wamp_message(wamp_message&& other)
{
    m_zone = std::move(other.m_zone);
    m_fields = std::move(other.m_fields);
}

inline wamp_message& wamp_message::operator=(wamp_message&& other)
{
    if (this == &other) {
        return *this;
    }

    m_zone = std::move(other.m_zone);
    m_fields = std::move(other.m_fields);

    return *this;
}

inline const msgpack::object& wamp_message::field(std::size_t index) const
{
    if (index >= m_fields.size()) {
        throw std::out_of_range("invalid message field index");
    }

    return m_fields[index];
}

template <typename Type>
inline Type wamp_message::field(std::size_t index)
{
    if (index >= m_fields.size()) {
        throw std::out_of_range("invalid message field index");
    }

    return m_fields[index].as<Type>();
}

template <typename Type>
inline void wamp_message::set_field(std::size_t index, const Type& type)
{
    if (index >= m_fields.size()) {
        throw std::out_of_range("invalid message field index");
    }

    m_fields[index] = msgpack::object(type, m_zone);
}

inline bool wamp_message::is_field_type(std::size_t index, msgpack::type::object_type type) const
{
    if (index >= m_fields.size()) {
        throw std::out_of_range("invalid message field index");
    }

    return m_fields[index].type == type;
}

inline std::size_t wamp_message::size() const
{
    return m_fields.size();
}

inline wamp_message::message_fields&& wamp_message::fields()
{
    return std::move(m_fields);
}

inline msgpack::zone&& wamp_message::zone()
{
    return std::move(m_zone);
}

inline std::ostream& operator<<(std::ostream& os, const wamp_message& message)
{
    std::size_t num_fields = message.size();
    if (num_fields == 0) {
        os << "unknown []";
        return os;
    }

    const msgpack::object& type_field = message.field(0);
    message_type type = static_cast<message_type>(type_field.as<int>());

    if (num_fields == 1) {
        os << to_string(type) << " []";
        return os;
    }

    os << to_string(type) << " [";
    os << message.field(1);
    for (std::size_t index = 2; index < num_fields; ++index) {
        os << ", " <<  message.field(index);
    }
    os << "]";

    return os;
}

} // namespace autobahn
