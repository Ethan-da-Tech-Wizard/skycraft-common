# Skycraft — Common Library Changes

A modified copy of Ian Patterson’s common C++ utility library used by the local Skycraft/SKSE build.

**Status:** Upstream-derived development library  
**Built with:** C++, CMake

## Reading order

1. Read the root README for project purpose and setup navigation.
2. Follow the entry points below to find the implementation.
3. Use the file guide to locate individual modules, tests, assets, and configuration.
4. Check component licenses before redistributing dependencies or assets.

## Important paths

- [CMakeLists.txt](../CMakeLists.txt) — CMake build definition for .; declares targets, dependency discovery, and compiler/build settings.
- [PROJECT_NOTES.md](../PROJECT_NOTES.md) — Preserved detailed setup, usage, architecture, and development notes from the earlier README.

## Structure

| Path | Role |
| --- | --- |
| [cmake/](../cmake/) | Project-specific cmake components (3 tracked entries). |
| [common/](../common/) | Project-specific common components (68 tracked entries). |
| [docs/](../docs/) | Project documentation, decisions, and reference material (5 tracked entries). |

## Scope

Credit belongs to Ian Patterson and upstream contributors. This repository contains local modifications; retain the original zlib notice and mark altered source clearly.

## Maintaining the guide

The file catalog is a source-inspection snapshot, not proof that every runtime path works. Update it when moving files or changing module responsibilities. The 2026-10-12 documentation review did not run every application, deploy services, or validate all historical performance claims.
