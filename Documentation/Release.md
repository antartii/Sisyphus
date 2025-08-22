# Release
This part of the documentation will explain how to properly release a new version of the Sisyphus library, making sure you do not forget any important steps.

## Version
A new release mean a new version, and as a little side school project we will follow a already near-perfect set of rules called the [Semantic Versioning](https://semver.org/) to determine what version number should be the new release.

We may derive a little bit from semantic versioning and write our custom set of rules in the future so make sure to acknowledge this document summarizing our current rules in application.

In case of conflict with the semantic versioning set of rules, only the rules stated in this document should be applied, otherwise if no clear rules has been stated by this document, please refer to the [Semantic Versioning](https://semver.org/) set of rules.

### Covering the basic of versioning
Each versions should be written as follow : `vX.Y.Z`
- `X` mean a major version, being updated only if the new release isn't backward compatible. In our case meaning exposed non-static functions or variables changed behavior in a way that can break a previous program using one of thoses functions, and yes even if the order of the function need to change inside of the program due to a change in the architecture of the code (e.g. order a() -> b() works before change, but will need b() -> a() in the new release).
- `Y` mean a minor version, being updated only if the new release added functionalities that is still backward compatible. In our case any newly added functions, variable types, or macros shouldn't break backward compatibility and be a minor version.
- `Z` mean a patch version, being updated only if the new release is composed of fixes, or little changed from here and there that didn't break backward compatibility. In our case changing the interior of a function to optimise it or fixing a little bug inside of it should be a patch version.

The `v` at the start of the version should stay v and means "version", just you know, reading `v3.2.5` looks cooler than `3.2.5` and there is no specific reason apart from aestetic.

To remind you how a version number should work :
- They should be non-negative integer and starting from 0
- Incremented only by 1 per version

When the release update it's versions, each lower labels should be reset to 0 (e.g. If `X` is updated, `Y` and `Z` should be reset to 0, and if `Y` is updated, `Z` should be reset to 0).

The project start at `0.1.0`, versions `0.x.y` are made for initial developpement, reaching the `1.0.0` meaning ready for production.

We do not use pre-release version or fancy wording/characters, only straight numbers, it's harder to keep up (bye `v.1.0.0-alpha`).

### Changing version in the code
In the code there is a few things to change in order to update the new version, so make sure you update the following :
- `\CMakeLists.txt` : inside of project(), change the number version after `VERSION `to the new version of the release. You should also change the latest version still compatible with your changes under the variable `COMPAT_VERSION`.

## Documenting
Another thing to consider when starting a new release is the documentation, remember that the documentation will help both other developpers to understand your code but also yourself in the future.

We dividing the documenting process into 3 steps :
- **Source file's documentation**
- **Release notes documentation**
- **Usage and consumer's documentation**

The **source file's documentation** is a classic [Doxygen](https://doxygen.nl/) documentation of your header files, you do not need to document and write every functions or variables types but at least :
- Macros and typedefs
- Exposed functions, variables and variable types

A part of documenting also include easy to understand names for your functions, variables, variable types, macros and files. We do not have strict rules, but we do try to follow some conventions such as :
- **snake_case** for functions, variables, C files and header files
- **ALL_CAPS** for macros
- **PascalCase** for structs

You are free to name them whatever you like but it should be clear enough to understand easily.

Lastly you can document your code using comments, but most of the time it shouldn't be necessary if your code is clear and precise enough.

The **Release notes documentation** is a simple yet precise document wrote in a markdown to describe the change made in the new release. Here is a template of a version release :
```markdown
# vX.Y.Z Release title
*release date: dd Month YYYY*

summary

## Changes

## Additional notes
```

In order :
- The **version** should be the version of the new release (obviously)
- The **Release title** should be a simple few words summarizing the changes
- The **summary** should be a more precise summary of the changes
- The **Changes** can be either a bullet point for small bug fixes or tiny changes but it should be a precise document in case of bigger changes (e.g. A new platform being available for compilation should be documented how the platform is available for compilation, how and why we use certain libraries to make it work)
- The **Additional notes** are optionnal and can be used to add few tips on usage of the new release.

This document should be in an understandable english.

And lastly the **Usage and consumer's documentation** may be updated from time to time, it include the **README.md** of this project, and other documentation located in the `Documentation/` directory at the source of this project. The reason to updated them could be for improving the quality of the writing, fixing english mistakes, precising usage/rules or covering more cases.
