/**
 * @file tables.hpp
 * @brief Vulkan Dispatch Tables
 * @author Alexander Rothman <[gnomesort@megate.ch](mailto:gnomesort@megate.ch)>
 * @date 2024
 * @copyright AGPL-3.0-or-later
 */
#ifndef MEGATECH_VULKAN_DISPATCH_TABLES_HPP
#define MEGATECH_VULKAN_DISPATCH_TABLES_HPP

#include <cstddef>
#include <cinttypes>

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
     *               pointer is the responsibility of the client. The table shares ownership of the pointed to
     *               function, and so the pointed to function **MUST** remain valid for its entire lifetime.
     * @throw dispatch::error If the value of `global` is null.
     * @see https://registry.khronos.org/vulkan/specs/1.3-extensions/html/vkspec.html#vkGetInstanceProcAddr
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
     *          @code{.cpp}
     *          // Table Initialization
     *          // Retrieve "vkGetInstanceProcAddr" elsewhere.
     *          auto gdt = table{ vkGetInstanceProcAddr };
     *          constexpr auto cmd = command::vkEnumerateInstanceVersion;
     *          // Load
     *          const auto vkEnumerateInstanceVersion = *reinterpret_cast<const PFN_vkEnumerateInstanceVersion*>(gdt.get(cmd));
     *          if (!vkEnumerateInstanceVersion)
     *          {
     *            // ERROR!
     *          }
     *          // It's safe to call "vkEnumerateInstanceVersion".
     *          @endcode
     *          Although it's possible for `vkGetInstanceProcAddr()` to resolve itself, the value returned by the
     *          `command::vkGetInstanceProcAddr` input will always be the loader function used to construct the
     *          table.
     * @param cmd The command to resolve.
     * @return A generic read-only pointer to the desired Vulkan command. The pointed-to value **MAY** be null.
     * @throw dispatch::error If the value of `cmd` isn't valid.
     */
    constexpr const void* get(const command cmd) const {
      if (static_cast<std::size_t>(cmd) >= m_pfns.size())
      {
        throw dispatch::error{ "The input command is outside the valid range of possible global commnds." };
      }
      return &m_pfns[static_cast<std::size_t>(cmd)];
    }

    /**
     * @brief Retrieve a pointer to a function pointer in the table.
     * @param cmd The command to resolve.
     * @return A generic read-only pointer to the desired Vulkan command. The pointed-to value **MAY** be null.
     * @throw dispatch::error If the value of `cmd` isn't valid.
     */
    constexpr const void* operator()(const command cmd) const {
      return get(cmd);
    }

/// @cond
#define MEGATECH_VULKAN_DISPATCH_COMMAND(name) case dispatch::internal::base::fnv_1a_cstr(#name): return get(command::name);
/// @endcond

    /**
     * @brief Retrieve a pointer to a function pointer in the table.
     * @details Although ::get(command) const always returns a valid pointer (unless you create an invalid ::command
     *          value on purpose), this overload can fail and return a `nullptr`. Failure occurs for any hash value
     *          unknown to the method at compile-time.
     *
     *          For this variant, a pointer can be safely reinterpreted as follows:
     *          @code{.cpp}
     *          // Table Initialization
     *          // Retrieve "vkGetInstanceProcAddr" elsewhere.
     *          auto gdt = table{ vkGetInstanceProcAddr };
     *          // The implementation of fnv_1a_64 is left up to the client, but this library provides a consteval
     *          // version as megatech::vulkan::dispatch::internal::base::fnv_1a_cstr.
     *          constexpr auto hash = fnv_1a_64("vkEnumerateInstanceVersion");
     *          // Load
     *          const auto ppfn = gdt.get(hash);
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
     *
     *          Although constant indexing is much simpler and less fault prone, the hash API provides a
     *          build-independent method for function pointer resolution. To put it simply, as long as client code
     *          computes the hash correctly, there's no need to know anything else about how this library was built.
     *          Conversely, the ::command API relies on knowledge of the order in which commands were emitted into
     *          defs.inl. This means that, for example, if an extension is disabled, the mapping of ::command
     *          values to dispatch table indices changes. This API, on the other hand, doesn't exhibit that problem.
     *          A client using hash values can safely use this library in different configurations as long as the hash
     *          values are computed correctly.
     *
     *          In short, if you rely on the hash API, you can be certain that resolution is always correct. You just
     *          have to check for `nullptr` twice. If you rely on the ::command API, you must always use the
     *          library in the correct configuration. Meaning, you must recompile any time the configuration changes.
     *          If you don't, the dispatch table could be indexed improperly.
     *
     *          If you can't guarantee the compile-time configuration libmegatech-vulkan-dispatch, you **SHOULD** use
     *          this API.
     * @param hash A 64-bit FNV-1a hash of the name of the ::command to retrieve.
     * @return A generic read-only pointer to the desired Vulkan ::command if it is found. Otherwise null.
     */
    constexpr const void* get(const std::uint_least64_t hash) const noexcept {
      switch (hash)
      {
      MEGATECH_VULKAN_DISPATCH_GLOBAL_COMMAND_LIST
      default:
        return nullptr;
      }
    }

#undef MEGATECH_VULKAN_DISPATCH_COMMAND

    /**
     * @brief Retrieve a pointer to a function pointer in the table.
     * @param hash A 64-bit FNV-1a hash of the name of the ::command to retrieve.
     * @return A generic read-only pointer to the desired Vulkan ::command if it is found. Otherwise null.
     * @see ::global::table::get(const std::uint_least64_t) const
     */
    constexpr const void* operator()(const std::uint_least64_t hash) const noexcept {
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
     * @brief Construct a table.
     * @details Table objects do not have an ownership relationship with their parent
     *          table. The lifetime of the global::table **MAY** end immediately after construction.
     * @param global A reference to a global::table.
     * @param instance A valid ::VkInstance handle. The table shares ownership of the ::VkInstance, and
     *                 so it **MUST** remain valid for the table's entire lifetime.
     * @throw dispatch::error If the value of instance is `VK_NULL_HANDLE`.
     */
    table(const megatech::vulkan::dispatch::global::table& global, const VkInstance instance);

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
     * @brief Retrieve the ::VkInstance used to construct the table.
     * @return The ::VkInstance handle that was used to construct the table.
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
     * @param cmd The ::command to resolve.
     * @return A generic read-only pointer to the desired Vulkan ::command. The pointed-to value **MAY** be null.
     * @throw dispatch::error If the value of `cmd` isn't valid.
     * @see ::global::table::get(const global::command) const
     */
    constexpr const void* get(const command cmd) const {
      if (static_cast<std::size_t>(cmd) >= m_pfns.size())
      {
        throw dispatch::error{ "The input command is outside the valid range of possible instance commnds." };
      }
      return &m_pfns[static_cast<std::size_t>(cmd)];
    }

    /**
     * @brief Retrieve a pointer to a function pointer in the table.
     * @param cmd The ::command to resolve.
     * @return A generic read-only pointer to the desired Vulkan ::command. The pointed-to value **MAY** be null.
     * @throw dispatch::error If the value of `cmd` isn't valid.
     * @see ::global::table::get(const global::command) const
     */
    constexpr const void* operator()(const command cmd) const {
      return get(cmd);
    }

/// @cond
#define MEGATECH_VULKAN_DISPATCH_COMMAND(name) case dispatch::internal::base::fnv_1a_cstr(#name): return get(command::name);
/// @endcond

    /**
     * @brief Retrieve a pointer to a function pointer in the table.
     * @param hash A 64-bit FNV-1a hash of the name of the ::command to retrieve.
     * @return A generic read-only pointer to the desired Vulkan ::command if it is found. Otherwise null.
     * @see ::global::table::get(const std::uint_least64_t) const
     */
    constexpr const void* get(const std::uint_least64_t hash) const noexcept {
      switch (hash)
      {
      MEGATECH_VULKAN_DISPATCH_INSTANCE_COMMAND_LIST
      default:
        return nullptr;
      }
    }

#undef MEGATECH_VULKAN_DISPATCH_COMMAND

    /**
     * @brief Retrieve a pointer to a function pointer in the table.
     * @param hash A 64-bit FNV-1a hash of the name of the ::command to retrieve.
     * @return A generic read-only pointer to the desired Vulkan ::command if it is found. Otherwise null.
     * @see ::global::table::get(const std::uint_least64_t) const
     */
    constexpr const void* operator()(const std::uint_least64_t hash) const noexcept {
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
    VkInstance m_instance{ };
    VkDevice m_device{ };
    std::array<PFN_vkVoidFunction, MEGATECH_VULKAN_DISPATCH_DEVICE_COMMAND_COUNT> m_pfns{ };
  public:
    /**
     * @brief Construct a table.
     * @details Table objects do not have an ownership relationship with their parent tables. The
     *          lifetime of the parent tables **MAY** end immediately after construction.
     * @param global A reference to a global::table.
     * @param instance A reference to an instance::table. The table shares ownership of the instance::table's
     *                 ::VkInstance, and so it **MUST** remain valid for the table's entire lifetime.
     * @param device A valid ::VkDevice handle. The table shares ownership of the ::VkDevice, and
     *                 so it **MUST** remain valid for the table's entire lifetime.
     * @throw dispatch::error If the value of `device` is null.
     */
    table(const megatech::vulkan::dispatch::global::table& global,
          const megatech::vulkan::dispatch::instance::table& instance, const VkDevice device);

    /**
     * @brief Construct a table from a ::VkInstance handle.
     * @details Generally, you should prefer to construct device dispatch tables on a per device basis. Properly
     *          resolved device function pointers provide the minimum overhead for applications. However, it is
     *          possible to resolve device command functions per instance instead. The obvious benefit to this is
     *          having far fewer function pointers in memory (for multi-device applications). The downside is that
     *          these instance-level function pointers still have to dispatch to the correct device internally. The
     *          result is greater overhead and a potential to dispatch to a function that doesn't exist.
     *
     *          Crucially, device extension commands (e.g., `vkCreateSwapchainKHR`) will resolve to valid function
     *          pointers as long as any available device supports the extension. This means that you cannot rely
     *          on a null function pointer being resolved for disabled or otherwise unavailable device extensions in
     *          this configuration.
     *
     *          In short, I'm providing this functionality because I can imagine it being useful to someone somewhere.
     *          If you're only going to use one device for the lifetime of your application, I think that you should
     *          use the device constructor instead.
     * @param global A reference to a global::table.
     * @param instance A valid ::VkInstance handle. The table shares ownership of the ::VkInstance, and so it **MUST**
     *                 remain valid for the table's entire lifetime.
     * @throw dispatch::error If the value of `instance` is null.
     */
    table(const megatech::vulkan::dispatch::global::table& global, const VkInstance instance);

    /**
     * @brief Construct a table from an instance-level table.
     * @param global A reference to a global::table.
     * @param instance A reference to an instance::table. The table shares ownership of the instance::table's
     *                 ::VkInstance, and so it **MUST** remain valid for the table's entire lifetime.
     * @see table(const megatech::vulkan::dispatch::global::table&, const VkInstance)
     */
    table(const megatech::vulkan::dispatch::global::table& global,
          const megatech::vulkan::dispatch::instance::table& instance);

    /**
     * @brief Construct a table by extending an existing table with a ::VkDevice.
     * @param base A base table. This **MUST** be derived from a ::VkInstance and have a null ::VkDevice. The
     *             resulting table will share ownership of the base table's ::VkInstance, so it **MUST** remain valid
     *             for the table's entire lifetime.
     * @param device A ::VkDevice to create the extended table from. The table shares ownership of the device, so
     *               it **MUST** remain valid for the table's entire lifetime.
     * @throw dispatch::error If the base table already has a ::VkDevice loaded or if `device` is null.
     */
    table(const table& base, const VkDevice device);

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
     * @brief Retrieve the ::VkInstance used to construct the table.
     * @return The ::VkInstance handle that was used to construct the table.
     */
    VkInstance instance() const;

    /**
     * @brief Retrieve the ::VkDevice used to construct the table.
     * @return The ::VkDevice handle that was used to construct the table if any. Null is returned in all other
     *         cases.
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
     * @param cmd The ::command to resolve.
     * @return A generic read-only pointer to the desired Vulkan ::command. The pointed-to value **MAY** be null.
     * @throw dispatch::error If the value of `cmd` isn't valid.
     * @see ::global::table::get(const global::command) const
     */
    constexpr const void* get(const command cmd) const {
      if (static_cast<std::size_t>(cmd) >= m_pfns.size())
      {
        throw dispatch::error{ "The input command is outside the valid range of possible device commnds." };
      }
      return &m_pfns[static_cast<std::size_t>(cmd)];
    }

    /**
     * @brief Retrieve a pointer to a function pointer in the table.
     * @param cmd The ::command to resolve.
     * @return A generic read-only pointer to the desired Vulkan ::command. The pointed-to value **MAY** be null.
     * @throw dispatch::error If the value of `cmd` isn't valid.
     * @see ::global::table::get(const global::command) const
     */
    constexpr const void* operator()(const command cmd) const {
      return get(cmd);
    }

/// @cond
#define MEGATECH_VULKAN_DISPATCH_COMMAND(name) case dispatch::internal::base::fnv_1a_cstr(#name): return get(command::name);
/// @endcond

    /**
     * @brief Retrieve a pointer to a function pointer in the table.
     * @param hash An FNV-1a hash of the name of the command to retrieve.
     * @return A generic read-only pointer to the desired Vulkan ::command if it is found. Otherwise null.
     * @see ::global::table::get(const std::uint_least64_t) const
     */
    constexpr const void* get(const std::uint_least64_t hash) const noexcept {
      switch (hash)
      {
      MEGATECH_VULKAN_DISPATCH_DEVICE_COMMAND_LIST
      default:
        return nullptr;
      }
    }

#undef MEGATECH_VULKAN_DISPATCH_COMMAND

    /**
     * @brief Retrieve a pointer to a function pointer in the table.
     * @param hash An FNV-1a hash of the name of the ::command to retrieve.
     * @return A generic read-only pointer to the desired Vulkan ::command if it is found. Otherwise null.
     * @see ::global::table::get(const std::uint_least64_t) const
     */
    constexpr const void* operator()(const std::uint_least64_t hash) const noexcept {
      return get(hash);
    }
  };

}

}

#endif
