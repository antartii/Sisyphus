# Sisyphus
C/C++ library for an engine using vulkan as it's graphic API and platform specific surfaces.
It is the 5th iteration of a engine library created by [Anta](https://bsky.app/profile/traveler-anta.bsky.social) and hopefully the last one.

## Dependencies
- **C99**
- **CMake 3.12**
- **Vulkan 1.2**
- **libwayland**
- **cglm**

## Compiling
To compile the Sisyphus library manually you can configure the file then building the library at the root of the project
```bash
cmake -B <BUILD_DIR> <OPTIONS>
cmake --build <BUILD_DIR>
```
The cmake options are the following :
|option|choice|definition|
|---|---|---|
|`SURFACE`|`wayland`|Surface api that the Sisyphus library will use at runtime. `wayland` is the default when nothing is specified|

you can specify an option by writing `-D<OPTION>=<CHOICE>` during the configuration command.

## Installing
To install Sisypus on your system after building it you can do so by executing the following command :
```bash
cmake --install <BUILD_DIR>
```

## Linking
To link the library using cmake after you've installed the library on your system you can directly use `find_package()` and link your target to `Sisyphus::Sisyphus`

```cmake
find_package(Sisyphus REQUIRED)

...

target_link_libraries(${PROJECT_NAME} PUBLIC Sisyphus::Sisyphus)
```

## Additional notes
This project is a side project from [Anta](https://bsky.app/profile/traveler-anta.bsky.social) it's mainly used to have fun, learn a lot and having a big puzzle to solve.
Some of the code could be (there is a high chance) inefficient or unconventionnal, if you have any suggestion I'll be really pleased to hear them, it could help a little apprentice developper in his journey.

One of the goal of this project is to feel that coding is still fun and that hopefully i won't be replaced by vibe coders.
But another goal is to learn, so I'm trying to compile documents explaining basic stuff I've learned along the way.

Also I'm french, I'm writing my markdowns in english by head, I should have a level of around B2/C1 in english but I could have badly translated words, expressions or sentences, feel free to let me know if you encountered any issues caused by my sometimes wrong english.

## Contact
- [Bluesky](https://bsky.app/profile/traveler-anta.bsky.social)
- Email: fireslidefr@gmail.com
