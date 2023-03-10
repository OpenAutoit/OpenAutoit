#include <phi/compiler_support/platform.hpp>

#if PHI_PLATFORM_IS(WINDOWS)
#    define _CRT_SECURE_NO_WARNINGS
#endif

#include "OpenAutoIt/Utililty.hpp"
#include <phi/container/string_view.hpp>
#include <phi/core/boolean.hpp>
#include <phi/core/scope_guard.hpp>
#include <phi/text/to_lower_case.hpp>
#include <cstdio>
#include <iostream>

namespace OpenAutoIt
{
    static phi::boolean output_enabled{true};

    phi::optional<std::string> read_file(const std::filesystem::path& file_path) noexcept
    {
        // Check that the file actually exists
        if (!std::filesystem::exists(file_path))
        {
            return {};
        }

        std::FILE* file =
#if PHI_PLATFORM_IS(WINDOWS)
                _wfopen(file_path.c_str(), L"r");
#else
                std::fopen(file_path.c_str(), "r");
#endif
        auto scope_guard = phi::make_scope_guard([&file]() { (void)std::fclose(file); });

        if (file == nullptr)
        {
            // Failed to open file
            return {};
        }

        (void)std::fseek(file, 0, SEEK_END);

        const long ftell_size = std::ftell(file);
        if (ftell_size == -1L)
        {
            // Failed to optain size
            return {};
        }
        const std::size_t size = static_cast<std::size_t>(ftell_size);

        std::rewind(file);

        // Create empty string of desired size
        std::string str(size, '\0');

        // Read data
        const std::size_t objects_read =
                std::fread(str.data(), sizeof(std::string::value_type), size, file);
        if (objects_read < size)
        {
            // Failed to read the file for some reason
            return {};
        }

        return phi::move(str);
    }

    phi::boolean write_file(const std::filesystem::path& file_path, std::string_view data) noexcept
    {
        std::FILE* file =
#if PHI_PLATFORM_IS(WINDOWS)
                _wfopen(file_path.c_str(), L"w");
#else
                std::fopen(file_path.c_str(), "w");
#endif
        auto scope_guard = phi::make_scope_guard([&file]() { (void)std::fclose(file); });

        if (file == nullptr)
        {
            return false;
        }

        const std::size_t objects_wrote =
                std::fwrite(data.data(), sizeof(data.front()), data.size(), file);
        if (objects_wrote < data.size())
        {
            // Failed to write the file for some reason
            return false;
        }

        return true;
    }

    void out(std::string_view data)
    {
        if (output_enabled)
        {
            std::cout << data;
        }
    }

    void err(std::string_view data)
    {
        if (output_enabled)
        {
            std::cerr << data;
        }
    }

    void disable_output()
    {
        output_enabled = false;
    }

    phi::boolean string_equals_ignore_case(const phi::string_view lhs,
                                           const phi::string_view rhs) noexcept
    {
        // If there not the same length they can't be equal
        if (lhs.length() != rhs.length())
        {
            return false;
        }

        // Case insensitively compare the names
        for (phi::u64 index{0u}; index < lhs.length(); ++index)
        {
            const char lhs_char = lhs.at(index);
            const char rhs_char = rhs.at(index);

            if (phi::to_lower_case(lhs_char) != phi::to_lower_case(rhs_char))
            {
                return false;
            }
        }

        // Same length and all characters are equal ignoring case
        return true;
    }
} // namespace OpenAutoIt
