# CFA Core Release Procedure

This checklist describes the release flow for CFA Core. CFA ships portable C
source code, so each release publishes one source zip and no platform-specific
binary packages.

Before starting, decide the new release number and use it as the `VERSION`
input throughout this checklist. Set `VERSION` without a leading `v`; the Git
tag adds the leading `v` separately. The first CFA Core release is `0.001`, with
tag `v0.001`.

## 1. Prepare the Version

Update the CFA Core version in `src/cagefight.h`:

```c
#define CFA_CORE_VERSION "<VERSION>"
```

The version must remain queryable from C and Swift integration code through:

```c
const char *cfa_core_version(void);
```

The command-line simulator must report the same value:

```sh
make native
build/native/cagefight --version
```

Use the same `VERSION` value in the Git tag, GitHub release title, release
description, and source zip filename.

## 2. Write the Release Synopsis

Create an approximately 200-word synopsis for this version before packaging the
release. Summarize simulator behavior changes first, then call out API, source
compatibility, command-set, logging, build, or platform changes that matter to
downstream users. Use this synopsis as the GitHub release description.

## 3. Verify the Source

From the repository root, run the automated build matrix. This checks the
available build variants on the current machine and verifies the native C log
smoke test.

```sh
make test
```

If the Swift/Xcode app is not available on the release machine, run the C-only
check:

```sh
make test TEST_SWIFT=0
```

Verify that the native command-line version matches `CFA_CORE_VERSION`:

```sh
VERSION="<VERSION>"
test "$(build/native/cagefight --version)" = "$VERSION"
```

## 4. Tag the Source

After the version number is decided and the final release commit is ready, tag
the exact source commit that will be archived.

```sh
VERSION="<VERSION>"
git tag -a "v${VERSION}" -m "CFA Core ${VERSION}"
git push origin "v${VERSION}"
```

If the release version changes, update `CFA_CORE_VERSION`, rebuild, retest, and
recreate the tag before publishing it.

## 5. Create the Source Package

Create a versioned source archive from the release tag. The archive must contain
a stable top-level directory and must be created from the tag, not from a dirty
working tree.

```sh
VERSION="<VERSION>"
mkdir -p dist
git archive \
  --format=zip \
  --prefix="cfa-core-${VERSION}/" \
  -o "dist/cfa-core-src-${VERSION}.zip" \
  "v${VERSION}"
```

The only release artifact is:

```text
dist/cfa-core-src-<VERSION>.zip
```

## 6. Verify the Release Artifact

Inspect the zip, record its size, and compute its checksum:

```sh
VERSION="<VERSION>"
ls -lh "dist/cfa-core-src-${VERSION}.zip"
shasum -a 256 "dist/cfa-core-src-${VERSION}.zip"
unzip -l "dist/cfa-core-src-${VERSION}.zip" | head -40
```

Optionally unpack the archive into a temporary directory and run the C build
checks from the packaged source:

```sh
VERSION="<VERSION>"
STAGE_DIR="$(mktemp -d)"
unzip -q "dist/cfa-core-src-${VERSION}.zip" -d "$STAGE_DIR"
make -C "$STAGE_DIR/cfa-core-${VERSION}" test TEST_SWIFT=0
PACKAGE_VERSION="$("$STAGE_DIR/cfa-core-${VERSION}/build/native/cagefight" --version)"
test "$PACKAGE_VERSION" = "$VERSION"
```

## 7. Publish the GitHub Release

Create a GitHub release for `v<VERSION>` with the synopsis from step 2. Attach
only the source archive:

```text
dist/cfa-core-src-<VERSION>.zip
```

Include the SHA-256 checksum in the release notes so downstream users can
verify the downloaded source package.
