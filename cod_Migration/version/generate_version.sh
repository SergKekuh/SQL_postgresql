#!/bin/bash

# Получаем версию из Git (тег или хэш)
VERSION=$(git describe --tags --always 2>/dev/null || echo "unknown")

if [[ "$VERSION" == *"-"* ]]; then
    VERSION="v0.1.0+"$VERSION
else
    VERSION="v"$VERSION
fi

# Сохраняем в файл version.hpp
echo "#pragma once" > version/version.hpp
echo "#define APP_VERSION \"$VERSION\"" >> version/version.hpp

echo "Версия: $VERSION"