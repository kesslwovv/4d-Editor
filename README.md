#  4d Object Editor

This is an Edior for four dimensional Objects

These Objects are represented by Meshes which can be saved to .obj4d or .obj4d.bin files  
See `FILE FORMAT` for a description of these Formats


# Build

you can build this using `cmake`:

```
mkdir build
cmake -S . -B build
cmake --build build
```

make sure to run the app in the same directory as the `Resources` and `Shaders` folder.  
Otherwise files won't load correctly

# License

see the LICENSE file for license

this also uses 3rd party Libraries (in the Externals folder). Their licenses are placed in their respective directories


 ## Editing

right now this Program does not have the editing part in it.  
there was a version that supported this but this has been deprecated.  
there are plans to add these back in.  


## File loading

you can load files by using the GUI.  
the GUI should be easily navigable.  
it is built on the DearImGui Library using the FileDialog widget provided by <https://github.com/gallickgunner/ImGui-Addons>


## Textures and UVs

you can load textures from the GUI.
you can also load UV coordinates or colordata from the GUI.
use the `clear texture` button to clear the texture and use the UV coords as colordata instead

## Known Issues

This is early in development so some of the features may be incomplete, have bugs or may crash when used incorrectly.

This App leaks memory when loading textures or meshes.

The Enable Lighting Option messes with the texture coordinates (works OK if you are using regular colordata)
