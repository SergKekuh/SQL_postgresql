#!/bin/bash

VERSION=$(git describe --tags --always 2>/dev/null)
GIT_HASH=$(git rev-parse --short HEAD 2>/dev/null || echo "dirty")
GIT_BRANCH=$(git rev-parse --abbrev-ref HEAD 2>/dev/null || echo "unknown")

if [[ -z "$VERSION" ]]; then
    VERSION="v0.1.0"
elif [[ "$VERSION" != v* ]]; then
    VERSION="v$VERSION"
fi

echo "#pragma once" > version/version.hpp
echo "#define APP_VERSION \"$VERSION\"" >> version/version.hpp
echo "#define GIT_COMMIT \"$GIT_HASH\"" >> version/version.hpp
echo "#define GIT_BRANCH \"$GIT_BRANCH\"" >> version/version.hpp

echo "Версия: $VERSION"