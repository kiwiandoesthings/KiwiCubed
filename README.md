# Kiwi³ v0.0.4pre-alpha (version system subject to change)

This game / engine is focused on being extremely expandable, having easy and powerful modding support, being ultra performant, and having official tools for creating content and mods that anyone can use and get the hang of.

Currently working on:
- Adding support for multiple block types, such as grass blocks and stone blocks, along with attributes for (future) gameplay.
- Gameplay elements such as items and entities.
- UI systems
- Separating the world from the game
- Unloading the world around you
- Survival gameplay elements

## Compiling

Clone the repository, and run ``just setup`` in the root. Once that finishes, you can run ``just debugbuild`` in the root to build the project, and ``just run`` to quickly rebuild and launch the project.

To compile a release build, run ``just releasebuild``. To run the release build quickly use ``just release``.

Use ``just clean`` to reconfigure both the debug and release build directories.