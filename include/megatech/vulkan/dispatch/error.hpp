/**
 * @file error.hpp
 * @brief Dispatch Errors
 * @author Alexander Rothman <[gnomesort@megate.ch](mailto:gnomesort@megate.ch)>
 * @date 2024
 * @copyright AGPL-3.0-or-later
 */
#ifndef MEGATECH_VULKAN_DISPATCH_ERROR_HPP
#define MEGATECH_VULKAN_DISPATCH_ERROR_HPP
#include <string>
#include <exception>

namespace megatech::vulkan::dispatch {
  /**
   * @brief A class representing errors generated during dispatch table construction.
   */
  class error final : public std::exception {
  private:
    std::string m_what{ };
  public:
    /**
     * @brief Construct an error.
     * @param what An error message.
     */
    explicit error(const std::string& what);
    /**
     * @brief Copy an error.
     * @param other The error to copy.
     */
    error(const error& other) = default;
    /**
     * @brief Move an error.
     * @param other The error to move.
     */
    error(error&& other) = default;
    /**
     * @brief Destroy an error.
     */
    ~error() noexcept = default;
    /**
     * @brief Copy-assign an error.
     * @param rhs The error to copy.
     * @return A reference to the copied-to error.
     */
    error& operator=(const error& rhs) = default;
    /**
     * @brief Move-assign an error.
     * @param rhs The error to move.
     * @return A reference to the moved-to error.
     */
    error& operator=(error&& rhs) = default;
    /**
     * @brief Retrieve the error's message.
     * @return A NUL-terminated string indicating the cause of the error.
     */
    const char* what() const noexcept override;
  };
}

#endif
