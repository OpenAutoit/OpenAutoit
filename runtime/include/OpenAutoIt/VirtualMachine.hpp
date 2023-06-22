#pragma once

#include "OpenAutoIt/AST/ASTStatement.hpp"
#include "OpenAutoIt/Scope.hpp"
#include "OpenAutoIt/StackTraceEntry.hpp"
#include "OpenAutoIt/Utililty.hpp"
#include "OpenAutoIt/VariableScope.hpp"
#include "OpenAutoIt/Variant.hpp"
#include <phi/compiler_support/warning.hpp>
#include <phi/container/string_view.hpp>
#include <phi/core/boolean.hpp>
#include <phi/core/forward.hpp>
#include <phi/core/observer_ptr.hpp>
#include <phi/core/types.hpp>
#include <iostream>
#include <iterator>
#include <list>
#include <ostream>
#include <string_view>
#include <unordered_map>

PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wuninitialized")

#include <fmt/core.h>
#include <fmt/format.h>

PHI_GCC_SUPPRESS_WARNING_POP()

namespace OpenAutoIt
{
using StackTrace = std::vector<StackTraceEntry>;

// The abstract virtual machine running AutoIt
class VirtualMachine
{
public:
    template <typename... ArgsT>
    void RuntimeError(std::string_view format_string, ArgsT&&... args) noexcept
    {
        err("[OpenAutoIt] ");
        err("\033[31m");
        err("RUNTIME ERROR!");
        err("\033[0m\n");
        err(" > ");
        err("\033[31m");
        err(fmt::format(fmt::runtime(format_string), phi::forward<ArgsT>(args)...));
        err("\033[0m\n");
        err("Stack trace:\n");

        // Print stack trace
        StackTrace strack_trace = GetStrackTrace();
        for (phi::u64 index{0u}; index < strack_trace.size(); ++index)
        {
            const StackTraceEntry& entry = strack_trace.at(index.unsafe());

            err(fmt::format("\t#{:d} {:s} {:s}:{:d}:{:d}\n", index.unsafe(), entry.function,
                            entry.file, entry.line.unsafe(), entry.column.unsafe()));
        }

        m_Aborting = true;
    }

    void PushFunctionScope(std::string_view function_name, Statements& statements) noexcept;
    void PushBlockScope(Statements& statements) noexcept;
    void PushGlobalScope(Statements& statements) noexcept;
    void PopScope() noexcept;

    [[nodiscard]] Scope&       GetCurrentScope() noexcept;
    [[nodiscard]] const Scope& GetCurrentScope() const noexcept;

    [[nodiscard]] Scope&       GetGlobalScope() noexcept;
    [[nodiscard]] const Scope& GetGlobalScope() const noexcept;

    [[nodiscard]] StackTrace GetStrackTrace() const noexcept;

    phi::boolean PushVariable(std::string_view name, Variant value) noexcept;
    phi::boolean PushVariableGlobal(std::string_view name, Variant value) noexcept;
    phi::boolean PushVariableWithScope(std::string_view name, Variant value,
                                       VariableScope scope) noexcept;

    void PushOrAssignVariable(std::string_view name, Variant value) noexcept;

    // Needs to be the variable name without a leading $
    [[nodiscard]] phi::optional<Variant> LookupVariableByName(
            std::string_view variable_name) const noexcept;
    [[nodiscard]] phi::optional<Variant&> LookupVariableRefByName(
            std::string_view variable_name) noexcept;
    [[nodiscard]] phi::optional<const Variant&> LookupVariableRefByName(
            std::string_view variable_name) const noexcept;

    [[nodiscard]] phi::boolean CanRun() const noexcept;

    void Exit(phi::u32 exit_code) noexcept;

    [[nodiscard]] phi::u32 GetExitCode() const noexcept;

    void OverwriteIOSreams(phi::observer_ptr<std::ostream> out,
                           phi::observer_ptr<std::ostream> err) noexcept;

    [[nodiscard]] phi::observer_ptr<std::ostream> GetStdout() const noexcept;
    [[nodiscard]] phi::observer_ptr<std::ostream> GetStderr() const noexcept;

private:
    std::list<Scope> m_Scopes;

    phi::observer_ptr<std::ostream> m_Stdout{&std::cout};
    phi::observer_ptr<std::ostream> m_Stderr{&std::cerr};
    phi::boolean                    m_Aborting{false};
    phi::u32                        m_ExitCode{0u};
};
} // namespace OpenAutoIt
