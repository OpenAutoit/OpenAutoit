#pragma once

#include "OpenAutoIt/TokenKind.hpp"
#include "Token.hpp"
#include <vector>

namespace OpenAutoIt
{
    class TokenStream
    {
    public:
        using storage_type           = std::vector<Token>;
        using iterator               = typename storage_type::iterator;
        using const_iterator         = typename storage_type::const_iterator;
        using reverse_iterator       = typename storage_type::reverse_iterator;
        using const_reverse_iterator = typename storage_type::const_reverse_iterator;

        template <typename... ArgsT>
        void emplace_back(ArgsT&&... args) noexcept
        {
#if defined(PHI_DEBUG)
            PHI_DBG_ASSERT(!m_Finialized);
#endif

            m_Tokens.emplace_back(std::forward<ArgsT>(args)...);
        }

        void push_back(const Token& value) noexcept;

        void push_back(Token&& value) noexcept;

        void finalize() noexcept;

        void reset() noexcept;

        [[nodiscard]] phi::boolean has_x_more(phi::usize x) const noexcept;

        [[nodiscard]] phi::boolean has_more() const noexcept;

        [[nodiscard]] phi::boolean reached_end() const noexcept;

        [[nodiscard]] const Token& look_ahead() const noexcept;

        void consume() noexcept;

        void skip(phi::usize n = 1u) noexcept;

        [[nodiscard]] const Token* find_first_token_of_type(TokenKind type) const noexcept;

        [[nodiscard]] const Token* find_last_token_of_type(TokenKind type) const noexcept;

        template <typename PredicateT>
        [[nodiscard]] const Token* find_first_token_if(PredicateT pred) const noexcept
        {
#if defined(PHI_DEBUG)
            PHI_ASSERT(m_Finialized);
#endif

            for (const Token& token : m_Tokens)
            {
                if (pred(token))
                {
                    return &token;
                }
            }

            return nullptr;
        }

        template <typename PredicateT>
        [[nodiscard]] const Token* find_last_token_if(PredicateT pred) const noexcept
        {
#if defined(PHI_DEBUG)
            PHI_ASSERT(m_Finialized);
#endif

            const Token* last = nullptr;

            for (const Token& token : m_Tokens)
            {
                if (pred(token))
                {
                    last = &token;
                }
            }

            return last;
        }

        [[nodiscard]] const Token& at(phi::usize index) const noexcept;

        [[nodiscard]] phi::usize size() const noexcept;

        [[nodiscard]] phi::boolean is_empty() const noexcept;

        // Iterator
        [[nodiscard]] phi::usize current_position() noexcept;

        void set_position(phi::usize index) noexcept;

        [[nodiscard]] const_iterator begin() const noexcept;

        [[nodiscard]] const_iterator cbegin() const noexcept;

        [[nodiscard]] const_iterator end() const noexcept;

        [[nodiscard]] const_iterator cend() const noexcept;

        [[nodiscard]] const_reverse_iterator rbegin() const noexcept;

        [[nodiscard]] const_reverse_iterator rend() const noexcept;

        [[nodiscard]] const Token& front() const noexcept;

        [[nodiscard]] const Token& back() const noexcept;

    private:
        storage_type m_Tokens;
        phi::usize   m_Index = 0u;
#if defined(PHI_DEBUG)
        phi::boolean m_Finialized{false};
#endif
    };
} // namespace OpenAutoIt
