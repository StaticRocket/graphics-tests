# Graphics tests

Various graphics related tests that do not currently have any other test
mechanisms.

## Structure

Each test is contained within a subdirectory, it may optionally have it's own
README with additional usage information. It may also define it's own
dependencies, if required.

## Global dependencies

Currently we expect GBM, EGL, and GLESv2 to be present for all tests. Individual
tests may define additional dependencies.

## Build guide

This repository utilizes the Meson build system. It uses features to select
various test applications. To enable tests selectively, set `-D<name>=enabled`.
To enable all tests, set `-Dauto_features=enabled`. This may be an unusual use
of the features function of Meson, but it allows for more flexibility for the
end user.

See [`meson_options.txt`](meson_options.txt) for a list of supported features.

To quickly build everything issue the following command sequence:

```
meson setup builddir -Dauto_features=enabled
meson compile -C builddir
```
