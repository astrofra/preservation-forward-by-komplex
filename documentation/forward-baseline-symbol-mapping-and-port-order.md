# Forward Baseline, Symbol Mapping, and C++ Port Order

## Objective

This note clarifies the recommended order of work between:

- Java desktop baseline preservation
- Java symbol renaming
- C++11 offline porting

It also defines the naming policy for the C++ port so that the project does not confuse behavioral evidence with speculative deobfuscation.

## Short Answer

The C++11 port should not inherit obfuscated Java names.

It should inherit:

- the behavioral baseline of the current `java-desktop` build
- a traceable symbol map with confidence levels

A broad Java renaming campaign is therefore not a prerequisite for starting the C++ port.

## Core Principle

Keep these three things separate:

1. Observed behavior
2. Inferred meaning
3. Chosen name in the target codebase

That separation matters because a readable name is not the same thing as a proven fact.
If the project renames too aggressively too early, guesses can become mistaken assumptions that are harder to audit later.

## What the Baseline Is For

The baseline exists to answer one question:

"Does the implementation still behave like the current reference build?"

That is a behavioral question, not a naming question.

The Java desktop tree is currently the most practical reference implementation in this repository.
It should remain stable enough to support:

- capture generation
- timing checks
- scene-by-scene validation
- future parity comparisons against the C++ exporter

For that reason, broad Java renaming should not happen before a usable regression baseline exists.

## Why Java Renaming Should Not Be the First Major Porting Prerequisite

Renaming Java first has one real advantage:

- it can improve local readability in the source reference

But it also has clear costs:

- it introduces churn into the current behavioral reference
- it increases review and regression surface without changing behavior
- it risks mixing "understood with high confidence" and "plausible but not yet proven"
- it can slow the start of the C++ port for reasons unrelated to preservation fidelity

So the right dependency is not:

- "rename Java first, then port"

The right dependency is:

- "stabilize behavior first, document symbol meaning second, port third"

## The Correct Knowledge Artifact: a Symbol Map

Before or alongside the early C++11 port work, the project should maintain a symbol map such as:

- `documentation/forward-symbol-map.csv`

Recommended columns:

- `legacy_symbol`
- `proposed_name`
- `kind`
- `confidence`
- `evidence`
- `files_touched`
- `notes`
- `status`

This is the epistemically clean layer between obfuscated source and readable target code.

It lets the team record:

- what is known
- how strongly it is known
- why that conclusion was reached

## Naming Policy for the C++11 Port

The C++ port should use readable names immediately where confidence is high.

### High-confidence symbols

Use readable names directly in C++ and preserve the legacy mapping in comments or documentation.

Example:

```cpp
class WatercubeScene; // legacy: kmajmka
class Scene;          // legacy: mmjjmma
```

These are good candidates for readable names:

- scene classes
- scene base classes
- obvious lifecycle methods
- core math primitives
- loader entry points
- top-level orchestration symbols with strong evidence

### Medium-confidence symbols

Use conservative names.

Prefer:

- subsystem-scoped names
- neutral descriptive names
- explicit comments about uncertainty

Avoid names that imply a stronger semantic claim than the evidence supports.

### Low-confidence symbols

Do not force readability at the cost of accuracy.

Keep either:

- the legacy name
- or a neutral placeholder such as `state_a`, `accumulator_1`, or `temp_phase`

These names can be promoted later when validation catches up.

## Practical Rule

The C++ port inherits behavioral truth, not lexical inheritance.

In other words:

- it should preserve what the Java code does
- it does not need to preserve how the obfuscator named things

This avoids a false choice between:

- "port with unreadable names"
- "fully clean Java before porting"

There is a better middle path:

- port from a stable behavioral reference
- name the target code from a documented confidence-based model

## Recommended Work Order

1. Freeze the Java desktop build as the behavioral baseline.
2. Capture reference outputs and establish comparison workflow.
3. Build and maintain a symbol map with confidence levels.
4. Start the C++11 exporter with readable names only for high-confidence symbols.
5. Validate subsystem by subsystem against the Java baseline.
6. Promote medium-confidence names only after targeted validation.
7. Run a broader Java renaming pass later only if maintaining `java-desktop` as a long-term readable codebase is still a project goal.

## What This Means for Java Renaming

Java renaming remains useful, but its role changes.

It is best treated as one of these:

- a maintainability improvement for the Java reference tree
- a follow-up cleanup after the baseline and symbol map are in place
- an optional parallel effort on high-confidence subsystems only

It should not be the main gate that blocks the C++11 port.

## Recommended Supporting Artifacts

- `documentation/forward-symbol-map.csv`
- `documentation/reference-capture/baseline/...`
- `documentation/regression/...`
- comparison reports for scene windows and full-timeline captures
- subsystem notes documenting uncertain names and later confirmations

## Bottom Line

The clean project order is:

- behavioral baseline first
- symbol knowledge second
- C++11 port third
- broad Java renaming later, if still useful

That keeps the repository honest about what is proven, what is inferred, and what is merely named for readability.
