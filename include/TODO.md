# FXT — Holistic Design Review

This document complements the inline `// TODO:` comments in the headers (grep for
`TODO: BUG`, `TODO: SAFETY`, etc. for localized issues). It takes a step back and
looks at the library as a whole: design choices worth revisiting, parts of the API
that arguably do not belong here, and functionality that is missing.

---

## 1. Library identity: adaptors over std types — embrace it

`fxt::tuple`, `fxt::optional`, `fxt::expected`, `fxt::variant` and `fxt::get`/
`fxt::apply`/`fxt::make_tuple` are now plain aliases for their std counterparts.
That is the right call (the commented-out wrapper experiments in `Tuple.hpp`
confirm the alternative was painful), but it has a consequence worth stating as a
design principle:

> **FXT is not a vocabulary-type library. It is a pipeline-adaptor library over
> the standard vocabulary types.**

Several current design decisions conflict with this identity:

- `flat_tuple` is the one remaining *bespoke* vocabulary type, and it does not
  deliver its stated promise (see §3).
- The `enums/` subsystem defines new vocabulary types unrelated to pipelines
  (see §4).
- The `tl::expected`/`tl::optional` escape hatches (`FXT_USE_TL_EXPECTED`) are
  good, but they only swap the alias — every adaptor that spells
  `fxt::expected<T, E>` concretely (e.g. `with()`, `to_expected`, `mget`,
  `mselect`) should instead be constrained on `expected_like`/`optional_like` so
  the library genuinely works with either backend, and with user-provided
  expected-likes.

**Action:** write this principle into the README, and audit every signature that
names `fxt::expected`/`fxt::optional` concretely where a concept would do.

---

## 2. Unify the pipe machinery (the single highest-leverage change)

There are currently **five independent `operator|` definitions** with three
different scoping strategies:

| Header              | LHS constraint            | Namespace |
|---------------------|---------------------------|-----------|
| `monads/Expected.hpp`  | `expected_like`         | `fxt` (found via... nothing — std types get no ADL into `fxt`; works only because call sites are inside `fxt` or via `using`) |
| `monads/Optional.hpp`  | `optional_like`         | `fxt` |
| `tuples/TuplePipe.hpp` | `tuple_like`            | **global** |
| `tuples/FlatTuple.hpp` | `flat_tuple` (4 overloads) | `fxt` (ADL works — flat_tuple lives in fxt) |
| `variants/VariantPipe.hpp` | `variant_like`      | **global** |

Plus `monads/With.hpp` and `monads/Sequence.hpp` add their own
`operator|(F&&, adaptor)` / `operator|(Container&&, adaptor)` overloads, and
`LogicalOr.hpp` adds twelve global `operator||` overloads.

Problems with the current scheme:

- The global-namespace operators leak into every TU that includes `fxt.hpp` and
  match *extremely* broadly (`any std::tuple | any compatible callable`), which
  can change overload resolution in unrelated code.
- The `fxt`-namespace operators for std types are only found when the call site
  can see them by ordinary unqualified lookup — subtle and fragile; users inside
  their own namespaces with their own `operator|` will shadow them.
- "Anything invocable with the container" as the dispatch rule means a stray
  lambda taking `auto&&` pipes happily, and diagnostics on mismatch are deep
  template errors rather than "not an fxt adaptor".

**Proposed redesign:** adopt the standard-library range-adaptor-closure pattern.

1. Introduce one CRTP/base tag, e.g. `struct adaptor_closure_base {}` (or C++23
   `std::ranges::range_adaptor_closure`-style).
2. Every adaptor returned by `and_then(f)`, `transform(f)`, `take<N>()`,
   `traverse(f)`, `with(ex)`, ... derives from it.
3. Exactly **one** `operator|` exists, in namespace `fxt`, constrained on
   `derived_from<adaptor_closure_base>` for the RHS — found by ADL on the
   *adaptor*, which always lives in `fxt`, so it works regardless of what the
   LHS is and never fires for non-FXT callables.
4. Free composition falls out almost for free: `adaptor | adaptor` can yield a
   fused closure (like ranges), giving FXT function composition (§5.1).

This one change fixes the namespace pollution, the ADL fragility, the
overly-broad matching, and removes ~10 redundant operator definitions.

---

## 3. API surface that should shrink

The library has grown by accretion and several areas now have two (or three)
ways to do the same thing. Pick winners:

| Keep | Remove / fold in | Why |
|------|------------------|-----|
| `tee`, `tee_error`, `tee_none` (renamed `tap`?) | `Tap.hpp` entirely | Tap is a broken duplicate (returns void). One name, one file. |
| `mtuple_append` | `mappend` (`Append.hpp`) | Same operation, two names; mappend's error-conversion feature can be folded into mtuple_append. |
| `mapply` | The plain-tuple overloads inside `mselect` | `mselect`'s overload set mixes monadic and plain handling; `select`/`mselect` should mirror `take`/`mtake`. |
| `tuple_transform` | doc-name `transform_tuple` | One spelling; fix the ~6 files whose examples use the other. |
| `for_each` (ranges) + `tuple_for_each` | `tuple_foreach` spelling | One word-separation convention. |
| named function (e.g. `or_value`/`either`) | `operator\|\|` overloads (`LogicalOr.hpp`) | Operator syntax that silently evaluates both sides is a trap; a named function makes the eager evaluation visible. |
| — | ~400 lines of commented-out code | `Tuple.hpp`, `Get.hpp`, `Apply.hpp`, `ApplyReplace.hpp`, `TuplePipe.hpp`, `Expected.hpp`, `Optional.hpp`, `IsOptional.hpp`, `Lazy.hpp`. Git history preserves it. |

### 3.1 `flat_tuple`: fix it or drop it

`flat_tuple` promises "contiguous storage, better cache locality" but is
implemented as `std::array<std::variant<indexed<I, Ts>...>, N>`: every slot is
`max(sizeof(Ts)...)` plus a discriminator, access goes through a variant, there
are no `tuple_size`/`tuple_element` specializations (no structured bindings, no
`std::apply`), no comparisons, and `get` is neither constexpr nor noexcept.
Meanwhile every tuple algorithm in the library carries a second overload or
`if constexpr` branch to support it — it roughly **doubles the maintenance cost
of the entire `tuples/` directory**.

Options, in order of preference:

1. **Drop it.** For homogeneous data, `std::array` + `as_array` already covers
   the use case; for heterogeneous data, std::tuple's layout is fine.
2. Reimplement honestly (aligned byte buffer + offset table), give it the full
   tuple protocol, and benchmark to justify its existence (`benchmarks/` exists).
3. Keep, but quarantine: make the generic algorithms dispatch through
   `fxt::get`/`fxt::tuple_size_v` only (most already do), delete the
   per-type duplicate overloads in `Apply.hpp`/`ApplyAppend.hpp`/`TupleCat.hpp`.

### 3.2 The `m`-prefix family: generate, don't hand-write

Every tuple/variant operation is duplicated by hand into an `m`-version that is
literally `container.transform([](auto&& x){ return op(x); })`. That is ~15
hand-rolled wrappers (mtake, mdrop, mtuple_reverse, mtuple_transform, mselect,
mget, mindex, mvisit, mas_array, ...), each with its own doc block and its own
chance of divergence (several have already diverged: const-only parameters,
missing rvalue support, the Case-2b bug in `Apply.hpp`).

**Proposal:** one lifting combinator:

```cpp
// monad | fxt::lifted(fxt::take<2>())  — or even auto-lift inside operator|
template<typename Adaptor>
constexpr auto lifted(Adaptor a) {
    return [a = std::move(a)]<monad_like M>(M&& m) {
        return std::forward<M>(m).transform(a);
    };
}
```

Then `mX()` becomes `lifted(X())` (keep the popular `mX` names as one-line
aliases if desired). This removes hundreds of lines, makes the m-family
impossible to get out of sync, and automatically extends to any future adaptor.
Only the operations with genuinely monadic semantics (`mwith`, `mtuple_append`
with monadic values, `mzip`, `mapply` with monadic return) need bespoke code.

### 3.3 One adaptor-invocation convention

`and_then(f)` is used bare; `value()`, `to_optional()`, `join()`, `index()`,
`take<2>()` need a trailing `()`. Half the doc examples get this wrong, which is
strong evidence users will too. With the §2 closure design, nullary adaptors can
simply *be* closure objects (`inline constexpr join_t join{};`) so `| fxt::join`
works without parentheses — matching `std::views::reverse` precedent.

---

## 4. Parts that do not belong in this library

- **`enums/` (string_enum, type_enum, typed_string, EnumBase).** Well-built, but
  it is a *reflection/vocabulary* utility, not a functional-composition tool: it
  has zero interaction with the pipe/monad machinery, pulls in an external
  dependency (`fixed_string.hpp`) that nothing else uses, uses a different
  naming convention (PascalCase helpers), and accounts for ~1,600 lines. It
  would thrive as a sibling library (`xte`?), keeping FXT dependency-free.
- **`utils/Immutable.hpp` and `utils/Lazy.hpp`.** General-purpose utilities with
  no connection to pipelines. Less urgent than `enums/` (they're small and
  dependency-free), but they widen the library's stated scope. If kept, `lazy`
  at least deserves monadic integration (e.g. `lazy<...> | fxt::transform(f)`
  producing a lazy) to justify its presence in an FP library.
- **`utils/Lift.hpp` macros (`FWD`, `LIFT`).** The functionality belongs; the
  unprefixed global macro names do not. `FXT_FWD`/`FXT_LIFT`, with `#ifndef`
  guards.
- **`LogicalOr.hpp`** in its operator form — see §3 table.
- **`utils/TypeValue.hpp`.** Tag-dispatch utility used by nothing else in the
  library; docs refer to a type (`fxt::Type`) that doesn't exist. Either adopt
  it somewhere (e.g. typed selection) or move it out.

A reasonable end-state: `fxt` = concepts + monads + tuples + variants + ranges +
the FP utils (curry, overload, attempt/failure, unit), everything else split out.

---

## 5. Missing functionality worth adding

### 5.1 Function composition
For a functional library, the most conspicuous absence: there is no way to
compose two functions or two adaptors without a value flowing through.

```cpp
auto validate = fxt::transform(parse) | fxt::and_then(check);   // adaptor fusion (falls out of §2)
auto h = fxt::compose(f, g);                                    // h(x) == f(g(x))
```

### 5.2 Optional symmetry for the applicative layer
`with`/`mwith`, `sequence`, `traverse` are expected-only. The optional versions
(`Container<optional<T>> -> optional<Container<T>>`, `curry(f) | with(opt)`)
are natural, cheap, and users will expect them given everything else in the
library handles both monads.

### 5.3 Missing core monad operations
- `filter` / `take_if`: `optional<T> -> optional<T>` given a predicate
  (and `expected` -> `expected` given predicate + error). Very common; currently
  needs a hand-written `and_then`.
- `flatten`/`join` for `optional<optional<T>>` (join is expected-only today —
  already flagged inline).
- `zip_with` (`mzip` + apply in one step): `mzip(a, b) | mapply(f)` is the
  documented idiom; a direct `zip_with(f, a, b...)` reads better.
- `to_expected` overload taking an error *factory* (lazy error construction)
  — the current version constructs the error even on the success path.
- `attempt`-style adaptor form: `| fxt::attempted` to wrap a throwing step
  mid-pipeline, complementing the free function.
- An `expects`/`ensure` assertion adaptor: `| fxt::ensure(pred, err)` for
  validation chains, which is the library's flagship use case (XLThermo
  validators).

### 5.4 Ranges story
`ranges/ForEach.hpp` is a lone outpost. Either commit to a small ranges layer —
`fxt::traverse` already overlaps with it; `Container | traverse(f)` plus
`sequence` is most of what's needed — or document that std::ranges is the
intended tool and keep only the monad×range bridges (`sequence`/`traverse`).
A `fxt::to_vector` / `ranges::to` bridge for sequencing into non-push_back
containers belongs here too.

### 5.5 Diagnostics & interop
- `std::format`/`operator<<` support for `expected`/`optional` pipelines'
  results would make `tee`-based debugging pleasant (`| fxt::tee(print)`).
  `string_enum` already has a formatter; `failure` has `operator<<` — extend
  the idea.
- A `fxt::result<T> = expected<T, failure>` alias: `attempt` already returns
  it; naming it makes signatures throughout user code shorter and nudges users
  toward a single error type per codebase.

### 5.6 Quality infrastructure
- **Compile the documentation.** A large fraction of the inline-TODO findings
  were doc examples that do not compile (`fxt::value` vs `fxt::value()`,
  `fxt::append`, `transform_tuple`, `Index<>` vs `IndexOf<>`, lazy's
  `auto copy = expensive;`). Extract `@code` blocks into a doc-snippet test
  target so they can never rot again.
- **Single-header / module distribution.** The 39-line license banner is
  duplicated in ~60 files (~2,300 lines); consider a one-line SPDX header
  (`// SPDX-License-Identifier: MIT`) per file with the full text in LICENSE,
  and optionally a generated single-header + C++23 `export module fxt;` build.
- **IWYU pass.** Several headers compile only via transitive includes
  (`TupleForEach.hpp` uses `fxt::apply` without including `Apply.hpp`;
  `ranges/ForEach.hpp` lacks `<ranges>`/`<utility>`; `TuplePipe.hpp` lacks
  `<functional>`).
- **Adaptor reusability tests.** Add tests that store every curried adaptor in
  a variable and apply it twice — this would have caught the single-use
  (move-from-capture) bugs and the broken `tap` immediately.

---

## 6. Suggested priority order

1. **Fix the outright bugs** (broken `tap`, `failure` hash, `mapply` Case 2b,
   single-use adaptors) — inline TODOs mark each site.
2. **§2 pipe unification** — everything else (composition, bare nullary
   adaptors, removing global operators) builds on it.
3. **§3 surface reduction** (tap/tee, mappend, dead code, flat_tuple decision)
   — do this *before* 1.0, while breaking changes are free.
4. **§5.6 doc-snippet tests** — locks in the cleanup.
5. **§3.2 lifted() + §5.2/5.3 completeness** — the feature work.
6. **§4 scope split** (`enums/` extraction) — whenever a second consumer
   appears or the dependency on fixed_string becomes annoying.
