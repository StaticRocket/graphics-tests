# Graphics Tests

Various graphics related tests that do not currently have any other test mechanisms.

## Structure of Repository

Each test has its own subdirectory, under the `src` directory. Provided within the subdirectory a `README` will be present,
regarding what the test is and usage. It may also define new extra dependencies for that test, on top of the standard global
ones for this repository.

## Standard Global Dependencies

All tests with this repository at a minimum depend on the following:
- GBM
- EGL
- GLESv2

## Build guide

This repository utilizes the Meson build system.

The user will need to specify which tests to build during the setup set of meson. All tests are considered class "feature", so they need to be explicitly enabled.
The general syntax is `-D<name>=enabled`.

Example:

```bash
meson setup <build_dir> -D lightweight-offscreen=enabled
```

Following that, change directories to the `<build_dir>` and execute meson compile.

Example:

```bash
meson compile
```

To enable all tests, set `-Dauto_features=enabled`.
