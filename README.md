# Kiwi³ v0.0.5pre-alpha 
### (version system subject to change)

This game / engine is focused on being extremely expandable, having easy and powerful modding support, being ultra performant, and having official tools for creating content and mods that anyone can use and get the hang of.

Currently working on:
- Gameplay elements such as items and entities.
- Survival gameplay elements

# Table of Contents
1. Compiling
2. Controls
3. Other Information lol

## 1. Compiling

Clone the repository, and run ``just setup`` in the root. Once that finishes, you can run ``just builddebug`` in the root to build the project, and ``just run`` to quickly rebuild and launch the project.

To compile a release build, run ``just buildrelease``. To run the release build quickly use ``just release``.

Use ``just clean`` to reconfigure both the debug and release build directories.

## 2. Controls

You can use ``tab/enter`` in most uis to select/trigger buttons.

You can switch between gamemodes by hitting ``f4``.

In survival mode:
* Physics are enabled
* Use ``wasd`` to move around
* ``Space`` to jump

In creative mode:
* Physics are disabled
* Use ``wasd`` to fly
* ``Space`` to go up, ``shift`` to go down

You can toggle the chunk debug visualization by hitting ``f3``.

To open your inventory, hit ``e``.
You can mine/place blocks using ``left/right click`` respectively, which will add and remove from your inventory.

## 3. Other Information lol

The game looks for new chunks to load/unload around you when you cross a chunk boundary. When you first load in, you will likely need to move around a little bit for it to realize it needs to generate more of the world around you. 

While the fast generation/meshing of chunks makes the game relatively playable for now, it's supremely bad at knowing where it needs to generate and mesh for you to actually see it (aka not generating air/underground chunks). Fixing this issue will make it a lot faster without actually generating the chunks themselves faster.

Also if you can't go back to areas that have been unloaded behind you it's because I forgot to fix that. If I did forget remind me or something I'm stupid.
