# xQsite 3D

A 3D Test with SDL2, OpenGL and AssImp with a Gradle build script.

For AssImp you will need CMake and an environment variable named CMAKE_EXECUTABLE pointing to the cmake executable.

You will need to download a font and put it into `res/font/`. 
Also I did not include textures. They have to be present in `res/textures/`.

Then just run `gradle build`.

# Creating and Exporting Models in Blender

Let the camera look down towards -Z, with the up vector pointing to Y. Now hit Ctrl-A with the camera selected and chose 
'Rotation to Deltas'.

Export to COLLADA with Forward Axis Y and Up Asis Z. This will ensure that the camera vectors are correct.

# License

See [LICENSE](./LICENSE)
