# CheatDevicePS2 CI container
# Custom image of ps2dev/ps2dev:v1.0 with glibc compatibility pre-installed.
#
# PROBLEM FIXED:
# GitHub Actions runner is now based on Node 24 Alpine (musl libc).
# Node's binary requires glibc symbols that are NOT present in musl:
#   Error relocating /__e/node24_alpine/bin/node: pthread_getname_np: symbol not found
#
# This happens because actions/checkout@v4 runs BEFORE any apk add step,
# so the glibc compat libraries must already exist inside the image.
#
# BUILD & PUSH (run locally on your machine):
#   docker build -f docker/CI.Dockerfile -t root670/ps2dev-docker:v1.0-gcompat .
#   docker push root670/ps2dev-docker:v1.0-gcompat
#
# Then update .github/workflows/CI.yml:
#   container: root670/ps2dev-docker:v1.0-gcompat

# Base image: ps2dev/ps2dev:v1.0 (minimal Alpine-based PS2 toolchain image)
# Keep same base as the original working CI for toolchain parity.
FROM ps2dev/ps2dev:v1.0

# Install glibc compatibility layer for Node.js
# Pre-seeded so GitHub Actions Node-based actions (checkout@v4, etc.) work inside this Alpine container.
# gcompat provides glibc 2.x compatibility on Alpine (musl).
# libc6-compat provides the actual libc.so shim with glibc symbols.
RUN apk add --no-cache gcompat libc6-compat libgcc libstdc++ musl-dev \
    && ln -sf /usr/lib/libc6-compat.so /usr/lib/libc.so \
    && ldconfig

# Verify glibc compatibility is available
RUN ls -la /usr/lib/libc.so && ldd --version 2>/dev/null | head -1
