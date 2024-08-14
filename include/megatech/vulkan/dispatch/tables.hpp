/**
 * @file tables.hpp
 * @brief Vulkan Dispatch Tables
 * @author Alexander Rothman <gnomesort@megate.ch>
 * @date 2024
 * @copyright AGPL-3.0-or-later
 */
#ifndef MEGATECH_VULKAN_DISPATCH_TABLES_HPP
#define MEGATECH_VULKAN_DISPATCH_TABLES_HPP
#include <cstddef>

#include <array>

#include "defs.hpp"
#include "commands.hpp"

namespace megatech::vulkan::dispatch {

namespace global {

  /**
   * @brief A dispatch table class for global Vulkan commands.
   */
  class table final {
  private:
    std::array<PFN_vkVoidFunction, MEGATECH_VULKAN_DISPATCH_GLOBAL_COMMAND_COUNT> m_pfns{ };
  public:
    /**
     * @brief Construct a table.
     * @param global A global loader function that behaves like the standard `vkGetInstanceProcAddr()` function.
     *               Crucially, it is not necessary that this actually is the same `vkGetInstanceProcAddr()` function
     *               that is defined by `libvulkan`. It only needs to behave correctly. Acquiring a working function
     *               pointer is the responsibility of the client.
     * @throw dispatch::error If the value of global is nullptr.
     */
    explicit table(const PFN_vkGetInstanceProcAddr global);
    /**
     * @brief Copy a table.
     * @param other The table to copy.
     */
    table(const table& other) = default;
    /// @cond
    table(table&& other) = delete;
    /// @endcond
    /**
     * @brief Destroy a table.
     */
    ~table() noexcept = default;
    /**
     * @brief Copy-assign a table.
     * @param rhs The table to copy.
     * @return A reference to the copied-to table.
     */
    table& operator=(const table& rhs) = default;
    /// @cond
    table& operator=(table&& rhs) = delete;
    /// @endcond
    /**
     * @brief Retrieve the size of the table.
     * @return The number of commands held in the table.
     */
    constexpr std::size_t size() const noexcept {
      return m_pfns.size();
    }
    /**
     * @brief Retrieve a pointer to a function pointer in the table.
     * @details Dispatch tables make no attempt to validate the function pointers they retrieve. Clients are
     *          responsible for ensuring that the retrieved pointer is not null and for reinterpreting it to the
     *          correct type. A pointer can be safely reinterpreted as follows:
     *
     *          @code{.cpp}
     *          // Table Initialization
     *          // Retrieve "vkGetInstanceProcAddr" elsewhere.
     *          auto gdt = table{ vkGetInstanceProcAddr };
     *          constexpr auto cmd = command::vkEnumerateInstanceVersion;
     *          // Load
     *          auto vkEnumerateInstanceVersion = *reinterpret_cast<const PFN_vkEnumerateInstanceVersion*>(gdt(cmd));
     *          if (!vkEnumerateInstanceVersion)
     *          {
     *            // ERROR!
     *          }
     *          // It's safe to call "vkEnumerateInstanceVersion".
     *          @endcode
     *
     *          Although it's possible for `vkGetInstanceProcAddr()` to resolve itself, the value returned by the
     *          `command::vkGetInstanceProcAddr` input will always be the loader function used to construct the
     *          table.
     * @param cmd The command to resolve.
     * @return A generic read-only pointer to the desired Vulkan command. The pointed-to value **MAY** be null.
     */
    constexpr const void* get(const command cmd) const noexcept {
      return &m_pfns[static_cast<std::size_t>(cmd)];
    }
    /**
     * @brief Retrieve a pointer to a function pointer in the table.
     * @param cmd The command to resolve.
     * @return A generic read-only pointer to the desired Vulkan command. The pointed-to value **MAY** be null.
     */
    constexpr const void* operator()(const command cmd) const noexcept {
      return get(cmd);
    }
/// @cond
#define MEGATECH_VULKAN_DISPATCH_COMMAND(name) case dispatch::internal::base::fnv1a_cstr(#name): return get(command::name);
/// @endcond
    /**
     * @brief Retrieve a pointer to a function in the table.
     * @details Although `get(command)` always returns a valid pointer (unless you manipulate create an invalid command
     *          value), this overload can fail and return a `nullptr`. Failure occurs for any hash value unknown to the
     *          method at compile-time.
     *
     *          For this variant, a pointer can be safely reinterpreted as follows:
     *
     *          @code{.cpp}
     *          // Table Initialization
     *          // Retrieve "vkGetInstanceProcAddr" elsewhere.
     *          auto gdt = table{ vkGetInstanceProcAddr };
     *          // The implementation of fnv1a is left up to the client, but this library provides a `consteval`
     *          // version as `megatech::vulkan::dispatch::internal::base::fnv1a_cstr()`.
     *          constexpr auto hash = fnv1a("vkEnumerateInstanceVersion");
     *          // Load
     *          const auto ppfn = gdt(hash);
     *          // The hash based getters **MAY** return nullptr.
     *          if (!ppfn)
     *          {
     *            // ERROR!
     *          }
     *          auto vkEnumerateInstanceVersion = *reinterpret_cast<const PFN_vkEnumerateInstanceVersion*>(ppfn);
     *          if (!vkEnumerateInstanceVersion)
     *          {
     *            // ERROR!
     *          }
     *          // It's safe to call "vkEnumerateInstanceVersion".
     *          @endcode
     * @param hash An FNV1a hash of the name of the command to retrieve.
     * @return A generic read-only pointer to the desired Vulkan command if it is found. Otherwise null.
     */
    constexpr const void* get(const std::size_t hash) const noexcept {
      switch (hash)
      {
      MEGATECH_VULKAN_DISPATCH_GLOBAL_COMMAND_LIST
      default:
        return nullptr;
      }
    }
#undef MEGATECH_VULKAN_DISPATCH_COMMAND
    /**
     * @brief Retrieve a pointer to a function in the table.
     * @param hash An FNV1a hash of the name of the command to retrieve.
     * @return A generic read-only pointer to the desired Vulkan command if it is found. Otherwise null.
     */
    constexpr const void* operator()(const std::size_t hash) const noexcept {
      return get(hash);
    }

  };

}

namespace instance {

  /**
   * @brief A dispatch table class for Vulkan instance commands.
   */
  class table final {
  private:
    VkInstance m_instance{ };
    std::array<PFN_vkVoidFunction, MEGATECH_VULKAN_DISPATCH_INSTANCE_COMMAND_COUNT> m_pfns{ };
  public:
    /**
     * @brief Construct an table.
     * @details table objects do not have an ownership relationship with their parent
     *          table. The lifetime of the global table may end immediately after construction.
     * @param global A reference to a global::table.
     * @param instance A valid @ref VkInstance handle. The table shares ownership of the @ref VkInstance, and
     *                 so it must remain valid for the table's entire lifetime.
     * @throw dispatch::error If the value of instance is VK_NULL_HANDLE (a/k/a nullptr).
     */
    table(megatech::vulkan::dispatch::global::table& global, const VkInstance instance);
    /**
     * @brief Copy an table.
     * @param other The table to copy.
     */
    table(const table& other) = default;
    /// @cond
    table(table&& other) = delete;
    /// @endcond
    /**
     * @brief Destroy an table.
     */
    ~table() noexcept = default;
    /**
     * @brief Copy-assign an table.
     * @param rhs The table to copy.
     * @return A reference to the copied-to table.
     */
    table& operator=(const table& rhs) = default;
    /// @cond
    table& operator=(table&& rhs) = delete;
    /// @endcond
    /**
     * @brief Retrieve the @ref VkInstance used to construct the table.
     * @return The @ref VkInstance handle that was used to construct the table.
     */
    VkInstance instance() const;
    /**
     * @brief Retrieve the size of the table.
     * @return The number of commands held in the table.
     */
    constexpr std::size_t size() const noexcept {
      return m_pfns.size();
    }
    /**
     * @brief Retrieve a pointer to a function pointer in the table.
     * @details Dispatch tables make no attempt to validate the function pointers they retrieve. Clients are
     *          responsible for ensuring that the retrieved pointer is not null and for reinterpreting it to the
     *          correct type.
     * @param cmd The command to resolve.
     * @return A generic read-only pointer to the desired Vulkan command. The pointed-to value **MAY** be null.
     */
    constexpr const void* get(const command cmd) const noexcept {
      return &m_pfns[static_cast<std::size_t>(cmd)];
    }
    /**
     * @brief Retrieve a pointer to a function pointer in the table.
     * @param cmd The command to resolve.
     * @return A generic read-only pointer to the desired Vulkan command. The pointed-to value **MAY** be null.
     */
    constexpr const void* operator()(const command cmd) const noexcept {
      return get(cmd);
    }
/// @cond
#define MEGATECH_VULKAN_DISPATCH_COMMAND(name) case dispatch::internal::base::fnv1a_cstr(#name): return get(command::name);
/// @endcond
    /**
     * @brief Retrieve a pointer to a function in the table.
     * @details Although `get(command)` always returns a valid pointer (unless you manipulate create an invalid command
     *          value), this overload can fail and return a `nullptr`. Failure occurs for any hash value unknown to the
     *          method at compile-time.
     * @param hash An FNV1a hash of the name of the command to retrieve.
     * @return A generic read-only pointer to the desired Vulkan command if it is found. Otherwise null.
     */
    constexpr const void* get(const std::size_t hash) const noexcept {
      switch (hash)
      {
      MEGATECH_VULKAN_DISPATCH_INSTANCE_COMMAND_LIST
      default:
        return nullptr;
      }
    }
#undef MEGATECH_VULKAN_DISPATCH_COMMAND
    /**
     * @brief Retrieve a pointer to a function in the table.
     * @param hash An FNV1a hash of the name of the command to retrieve.
     * @return A generic read-only pointer to the desired Vulkan command if it is found. Otherwise null.
     */
    constexpr const void* operator()(const std::size_t hash) const noexcept {
      return get(hash);
    }
  };

}

namespace device {

  /**
   * @brief A dispatch table class for Vulkan device commands.
   */
  class table final {
  private:
    VkDevice m_device{ };
    std::array<PFN_vkVoidFunction, MEGATECH_VULKAN_DISPATCH_DEVICE_COMMAND_COUNT> m_pfns{ };
  public:
    /**
     * @brief Construct a table.
     * @details table objects do not have an ownership relationship with their parent tables. The
     *          lifetime of the parent tables may end immediately after construction.
     * @param global A reference to a global::table.
     * @param instance A reference to an instance::table.
     * @param device A valid @ref VkDevice handle. The table shares ownership of the @ref VkDevice, and
     *                 so it must remain valid for the table's entire lifetime.
     * @throw dispatch::error If the value of device is VK_NULL_HANDLE (a/k/a nullptr).
     */
    table(megatech::vulkan::dispatch::global::table& global, megatech::vulkan::dispatch::instance::table& instance,
          const VkDevice device);
    /**
     * @brief Copy a table.
     * @param other The table to copy.
     */
    table(const table& other) = default;
    /// @cond
    table(table&& other) = delete;
    /// @endcond
    /**
     * @brief Destroy a table.
     */
    ~table() noexcept = default;
    /**
     * @brief Copy-assign a table.
     * @param rhs The table to copy.
     * @return A reference to the copied-to table.
     */
    table& operator=(const table& rhs) = default;
    /// @cond
    table& operator=(table&& rhs) = delete;
    /// @endcond
    /**
     * @brief Retrieve the @ref VkDevice used to construct the table.
     * @return The @ref VkDevice handle that was used to construct the table.
     */
    VkDevice device() const;
    /**
     * @brief Retrieve the size of the table.
     * @return The number of commands held in the table.
     */
    constexpr std::size_t size() const noexcept {
      return m_pfns.size();
    }
    /**
     * @brief Retrieve a pointer to a function pointer in the table.
     * @details Dispatch tables make no attempt to validate the function pointers they retrieve. Clients are
     *          responsible for ensuring that the retrieved pointer is not null and for reinterpreting it to the
     *          correct type.
     * @param cmd The command to resolve.
     * @return A generic read-only pointer to the desired Vulkan command. The pointed-to value **MAY** be null.
     */
    constexpr const void* get(const command cmd) const noexcept {
      return &m_pfns[static_cast<std::size_t>(cmd)];
    }
    /**
     * @brief Retrieve a pointer to a function pointer in the table.
     * @param cmd The command to resolve.
     * @return A generic read-only pointer to the desired Vulkan command. The pointed-to value **MAY** be null.
     */
    constexpr const void* operator()(const command cmd) const noexcept {
      return get(cmd);
    }
/// @cond
#define MEGATECH_VULKAN_DISPATCH_COMMAND(name) case dispatch::internal::base::fnv1a_cstr(#name): return get(command::name);
/// @endcond
    /**
     * @brief Retrieve a pointer to a function in the table.
     * @details Although `get(command)` always returns a valid pointer (unless you manipulate create an invalid command
     *          value), this overload can fail and return a `nullptr`. Failure occurs for any hash value unknown to the
     *          method at compile-time.
     * @param hash An FNV1a hash of the name of the command to retrieve.
     * @return A generic read-only pointer to the desired Vulkan command if it is found. Otherwise null.
     */
    constexpr const void* get(const std::size_t hash) const noexcept {
      switch (hash)
      {
      MEGATECH_VULKAN_DISPATCH_DEVICE_COMMAND_LIST
      default:
        return nullptr;
      }
    }
#undef MEGATECH_VULKAN_DISPATCH_COMMAND
    /**
     * @brief Retrieve a pointer to a function in the table.
     * @param hash An FNV1a hash of the name of the command to retrieve.
     * @return A generic read-only pointer to the desired Vulkan command if it is found. Otherwise null.
     */
    constexpr const void* operator()(const std::size_t hash) const noexcept {
      return get(hash);
    }
  };

}

}

#endif
