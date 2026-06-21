# /cpp-review — C++ PR Review

## Argument parsing

`$ARGUMENTS` controls base and target:

| Input | Base | Target |
|-------|------|--------|
| *(empty)* | `develop` | `HEAD` |
| `<branch>` | `<branch>` | `HEAD` |
| `<base> <target>` | `<base>` | `<target>` |

Parse `$ARGUMENTS` to set BASE and TARGET before doing anything else.

---

## Step 1: Gather the diff

```bash
git diff BASE...TARGET               # full diff for manual review
git diff BASE...TARGET --name-only   # file list for clang-tidy
```

Use three-dot syntax (`...`) to show only commits unique to TARGET since it diverged from BASE.

---

## Step 2: Automated checks (clang-tidy)

Filter the changed file list to C++ sources (`*.cpp`, `*.cc`, `*.h`, `*.hpp`).

If `build/linux/compile_commands.json` exists, run:
```bash
run-clang-tidy -p build/linux <changed-cpp-files>
```

Capture all output. If the build doesn't exist, note that automated checks were skipped and suggest `task build-linux` first.

**Do not manually re-check anything covered by the `cppcoreguidelines-*`, `bugprone-*`, `google-*`, `modernize-*`, `performance-*`, or `readability-*` clang-tidy check families.** Those are already enforced at `WarningsAsErrors` level.

---

## Step 3: Manual review

Review the diff against the rules below. Each finding needs file + line from the diff context.

---

## Reference: C++ Core Guidelines (non-automatable rules only)

clang-tidy covers bounds safety, type safety, owning-memory, magic numbers, narrowing, special members, macro usage, and most naming. The rules below are what static analysis cannot reliably catch.

### Philosophy (P)
- **P.1** Express intent directly in code. Code that requires a comment to explain *what* it does (not *why*) is a violation — rename, refactor, or use a better abstraction.
- **P.3** Abstractions must reflect the *domain concept*, not the *implementation mechanism*. A type or function named for how it works rather than what it represents is a smell.
- **P.5** Prefer compile-time checking over runtime. If a condition can be `static_assert`, `constexpr`, or a concept, it should be.
- **P.8** No resource leaks — this means acquiring a resource and not releasing it on *every* exit path, including early returns and exceptions.
- **P.9** No waste — wrong algorithmic complexity, unnecessary copies/conversions, or structures that hold more than needed.
- **P.10** Prefer `const` and immutable data. Every mutable variable is a potential bug; make mutability explicit and minimal.

### Interfaces (I)
- **I.1** Interfaces must be *explicit*: all behavior observable from the declaration. Hidden preconditions, global side effects, or output via parameter mutation without clear naming are violations.
- **I.3** No singletons. Pass state explicitly or inject dependencies.
- **I.4** Use strong types over bare `bool`/`int`/`string` when parameters are easily confused. A function `setSize(int, int)` invites swapping width and height.
- **I.5 / I.7** Preconditions belong in `Expects()`, postconditions in `Ensures()` (GSL). If not using GSL contracts, at minimum document the contract in a comment.
- **I.11** No raw-pointer ownership transfer. `T*` means *non-owning borrow*. Use `unique_ptr` / `shared_ptr` / `gsl::owner<T*>` when ownership transfers.
- **I.23** ≤ 4 parameters. Bundle extras into a named struct.
- **I.24** No adjacent parameters of the same type that could be swapped without a compile error and with different semantics (e.g., `copy(char* dst, char* src)`).

### Functions (F)
- **F.1 / F.2 / F.3** One logical operation per function, short enough to read at a glance, named for what it *does* (verb phrase for procedures, noun phrase for queries).
- **F.8** Prefer pure functions. Side effects should be minimal, named, and unsurprising.

### Classes (C)
- **C.2** Use `class` when the type enforces invariants; use `struct` only for passive data bundles.
- **C.20 / C.21** Rule of Zero: if the compiler-generated five are correct, define none of them. If you define *any* of {destructor, copy-ctor, copy-assign, move-ctor, move-assign}, explicitly define or `=delete` all five.
- **C.129** Prefer composition over inheritance for code reuse. Inheritance hierarchies should be narrow (few siblings) and shallow (few levels). Deep or wide hierarchies are a design smell.

### Resources (R)
- **R.1** RAII for all resources without exception. No bare `new`/`delete`, no manual `fopen`/`fclose` without an RAII wrapper.
- **R.3** If a raw owning pointer is unavoidable, mark it `gsl::owner<T*>` at the declaration.

---

## Reference: Google C++ Style Guide (non-automatable rules only)

clang-tidy `google-*` checks cover explicit constructors, global names in headers, and most naming. The rules below are what it cannot catch.

### Headers & files
- Every header must be *self-contained*: it compiles successfully in isolation without requiring other headers to be included first.
- Minimize `#include` inside headers; forward-declare types when the definition is not needed.
- Group includes: own headers, then other project headers, then third-party, then standard library. Alphabetical within each group.

### Classes
- Avoid multiple *implementation* inheritance. Pure-virtual interfaces are fine.
- Prefer composition over inheritance.
- Avoid `mutable` unless the member is a genuine implementation detail invisible to the user (e.g., a mutex or a cache). Mutable visible state is a design error.

### Ownership & smart pointers
- `unique_ptr` is the default for sole ownership.
- `shared_ptr` only when ownership is *genuinely shared* and lifetime cannot be determined statically. Over-use of `shared_ptr` hides ownership and creates cycles.
- Raw pointer = non-owning borrow. Document this at the declaration site (`// non-owning` or use `gsl::not_null<T*>` for an always-valid borrow).

### Comments
- Comments explain *why*, not *what*. If a reader needs a "what" comment to understand the code, the code needs better names or structure.
- All public API symbols must have a doc comment explaining the contract (parameters, return value, preconditions, side effects).
- TODOs: `// TODO(username): description`.

### Integer types
- Default to `int`. Use `<cstdint>` fixed-width types (`int32_t`, `uint64_t`) when size matters for serialisation, protocols, or SIMD.
- Avoid unsigned integers for arithmetic; signed overflow is UB but unsigned wraparound silently produces wrong answers.

---

## Known standard conflicts — always flag, never silently pick one

When the diff touches any of these areas, add a **CONFLICT** finding for the user to decide:

| # | Topic | C++ Core Guidelines | Google Style Guide |
|---|-------|--------------------|--------------------|
| ~~**C1**~~ | ~~**Exceptions**~~ | ~~Encouraged for error signaling; RAII makes them safe~~ | ~~Banned in new code; use error codes or status returns~~ — **resolved: Google wins** |
| **C2** | **`auto`** | Use liberally to reduce noise | Use only when the type is obvious from context |
| **C3** | **Include guards** | No preference stated | `#define` guards preferred; `#pragma once` widely tolerated |
| **C4** | **Trailing return types** | Use when they aid clarity | Use only when syntactically required |
| **C5** | **Streams** | No restriction | Avoid; prefer `printf`-style for diagnostics |

---

## Step 4: Report format

### Automated findings (clang-tidy)
Paste a brief summary. If the build was unavailable, say so. If clean, say so.

### Manual findings

One block per issue:

```
[SEVERITY] path/to/file.cpp:LINE — Short title
  Divergence : What the code does and why it's a violation.
  Rule(s)    : e.g., CG I.23 · Google: Ownership
  Resolution : Specific, actionable fix.
```

Severity levels:
- `ERROR` — clear, unambiguous violation
- `WARNING` — likely violation; context may change the verdict
- `CONFLICT` — the two standards disagree; needs a project-level decision

### Decisions required
List every CONFLICT finding with location and the tradeoff summary.

### Verdict
One short paragraph: overall code quality, most critical issues, and whether the diff is merge-ready.

---

## Repo-specific rules

### R1 — No trailing return types (C4 resolved, Google wins)
Use conventional `T f()` syntax. `modernize-use-trailing-return-type` is disabled in
`.clang-tidy`. Do **not** flag conventional return types as violations.

**Rationale**: codebase targets C++17 for consistency with the author's work environment,
where all existing code uses conventional syntax. Trailing return types are not required
until the syntax forces it (e.g., dependent return types in templates).

### R3 — No exceptions (C1 resolved); constructors must always succeed
Do not use exceptions for error signaling. Preferred error-handling hierarchy:

1. **Constructor that always succeeds** — preferred. Design the type so construction
   cannot fail: separate resource *ownership* (constructor sets up valid empty/default
   state) from resource *acquisition* (done elsewhere). This may require architectural
   changes — e.g. splitting a monolithic class into a small always-valid RAII wrapper
   and a higher-level type that composes them. A constructor that can be called without
   external preconditions and leaves the object in a fully usable state is the goal.
2. **Factory function** returning `std::optional<T>` — when the object fundamentally
   requires an external resource to be useful at all and a valid-empty state is
   meaningless. The factory owns the failure path; the object type itself is always
   valid once it exists.
3. **Two-phase `Init()` / `Close()`** — last resort, only when the above two are
   genuinely infeasible (e.g. wrapping a C API that has no equivalent decomposition).
   `Init()` must return `bool`; destructors must be safe to call on
   partially-initialized objects.

**Rationale**: the author works in environments where exceptions are disabled.
Always-succeeding constructors satisfy CG C.41 without exceptions and avoid zombie
objects — the best compromise between CG and Google Style. Architectural changes to
achieve this are preferable to two-phase init.

### R2 — Target is C++17; GSL fills the gap
The project deliberately targets C++17 (`CMAKE_CXX_STANDARD 17`). Features that arrive
in C++20/23 (contracts, `std::span`, `std::format`, etc.) are bridged via GSL
(`gsl::not_null`, `gsl::owner`, `gsl::span`, `Expects()`, `Ensures()`). When reviewing:
- Prefer GSL types over raw pointers for ownership/non-null semantics.
- Do **not** suggest C++20+ standard-library replacements for things GSL already covers.
- Flag raw `T*` that carry ownership intent as `ERROR` (use `gsl::owner<T*>`).
