# Skycraft — Common Library Changes

A modified copy of Ian Patterson’s common C++ utility library used by the local Skycraft/SKSE build.

**Status:** Upstream-derived development library  
**Built with:** C++, CMake

[Project guide](docs/PROJECT_GUIDE.md) · [Every file explained](docs/FILE_GUIDE.md) · [Licenses and dependencies](docs/LICENSE_REVIEW.md)

## Start here

- [CMakeLists.txt](CMakeLists.txt) — CMake build definition for .; declares targets, dependency discovery, and compiler/build settings.
- [PROJECT_NOTES.md](PROJECT_NOTES.md) — Preserved detailed setup, usage, architecture, and development notes from the earlier README.

[Detailed setup, usage, and development notes](PROJECT_NOTES.md) preserve the previous project documentation.

## Repository layout

| Path | Role |
| --- | --- |
| [cmake/](cmake/) | Project-specific cmake components (3 tracked entries). |
| [common/](common/) | Project-specific common components (68 tracked entries). |
| [docs/](docs/) | Project documentation, decisions, and reference material (5 tracked entries). |

## Scope and current limits

Credit belongs to Ian Patterson and upstream contributors. This repository contains local modifications; retain the original zlib notice and mark altered source clearly.

## License

Original project code retains **Zlib** terms in [LICENSE](LICENSE). Third-party libraries, media, data, models, and tools retain their own terms. See the [license review](docs/LICENSE_REVIEW.md) and [dependency inventory](docs/DEPENDENCIES.csv).
