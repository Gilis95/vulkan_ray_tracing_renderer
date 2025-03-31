#include "assets/asset_types.h"

namespace wunder {
std::uint32_t asset_handle::s_invalid = 0;

 asset_handle asset_handle::invalid() { return asset_handle(s_invalid); }

 asset_handle::asset_handle() : m_value(s_invalid) {}
asset_handle::asset_handle(type value) : m_value(value) {}

asset_handle::asset_handle(const asset_handle& other) = default;
asset_handle& asset_handle::operator=(const asset_handle& other) = default;

asset_handle::~asset_handle() = default;

bool asset_handle::is_valid() const { return m_value != s_invalid; }
asset_handle::operator bool() const { return is_valid(); }

asset_handle::type asset_handle::value() const { return m_value; }
asset_handle::operator asset_handle::type() const { return m_value; }

bool asset_handle::operator==(type other_value) const {
  return m_value == other_value;
}

bool asset_handle::operator==(asset_handle other_value) const {
  return m_value == other_value.m_value;
}

 bool asset_handle::operator<(asset_handle other_value) const {
 return m_value < other_value.m_value;
}



}