# HTN Visualizer

A **Hierarchical Task Network (HTN) visualization and debugging tool** written with Dear ImGui.

HTN is a planning model widely used in game AI, where an agent's high-level goal is broken down into smaller, executable tasks ("move to target", "fire weapon"). This tool was built to inspect, navigate, and debug such a task tree in real time.

> **Note:** This project is a personal, from-scratch version of a similar AI debugging tool I needed on AAA production (IO Interactive, *007: First Light*). It contains no proprietary code, data formats, or assets from that work.

## Features

![HTN Visualizer Screenshot](./htn-visualizer-screenshot.png)

- **Node Hierarchy** - a searchable list view of the full tree, from compound tasks (e.g. `Exploration`, `Combat`, `ResourceCollection`, `SupportAlly`) down to primitive tasks (e.g. `ScanArea`, `FireWeapon`, `LocateResource`). Every node has `Open in File` to jump straight to its source, and `Focus` to jump to it on the canvas.
- **Visual Node Graph** - task decomposition relationships rendered in real time on a draggable node canvas, with parent-child links drawn as arrows.
- **Node Properties Panel** - live inspection of a selected task's name, type, and custom parameters (e.g. `EngagementRange: 300.0` on the `Combat` task).
- **Mini Map** - zoomable overview for fast navigation on larger trees.

## Why I built it

At IO Interactive, our AI behavior definitions lived in an in-house DSL built almost entirely out of macros and strings. That meant AI programmers had no symbol navigation in the IDE, finding a specific task in a large tree meant text-searching through it by hand. I built a proof-of-concept visualizer there to make that tree browsable and debuggable at a glance, with the long-term idea of growing it into something closer to Unreal Engine's Behavior Tree editor. Eventually letting you author and create tasks directly from the editor, not just inspect them.

This repository is my own independent take on that same problem: browsing a large, otherwise hard-to-navigate task tree visually. It's built from scratch with my own architecture, DSL, and serialization format, solving it end-to-end on my own time, with nothing carried over from the internal codebase.

## Tech

- C++17
- Dear ImGui + a node-editor library for the visual graph view
- GLFW, tinyxml2
- CMake

## Building

Requires CMake 3.15+, a C++17 compiler, and (currently) Windows, the project links directly against `opengl32`.

```bash
git clone --recursive https://github.com/EsgiJ/HTN_Visualizer.git
cd HTN_Visualizer/HTNVisualizer
cmake -B build -S .
cmake --build build --config Release
```

Dependencies live under `extern/`: Dear ImGui, GLFW, tinyxml2, and a node-editor library for the graph view.

> **!Worth checking before someone else clones this:** the repo's `.gitmodules` only registers `imnodes` and `IconFontCppHeaders` as submodules. `imgui`, `glfw`, `tinyxml2`, and the folder `CMakeLists.txt` expects at `extern/imgui-node-editor` aren't listed there. If those are committed directly (vendored, not submodules), that's fine, just confirm it. If not, `git clone --recursive` will pull an empty `extern/` for them and the build will fail for anyone else. Also worth double-checking: `.gitmodules` registers `imnodes` (by Nelarius), but `CMakeLists.txt` references a folder named `imgui-node-editor` and links `imgui_node_editor`, make sure these actually resolve to the same library and aren't a leftover from a swap.
