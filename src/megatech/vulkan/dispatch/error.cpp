/**
 * @file error.cpp
 * @brief Vulkan Dispatch Errors
 * @author Alexander Rothman <[gnomesort@megate.ch](mailto:gnomesort@megate.ch)>
 * @date 2024
 * @copyright AGPL-3.0-or-later
 */
#include "megatech/vulkan/dispatch/error.hpp"

namespace megatech::vulkan::dispatch {

  error::error(const std::string& what) : m_what{ what } { }

  const char* error::what() const noexcept {
    return m_what.data();
  }

}
