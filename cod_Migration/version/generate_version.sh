#!/bin/bash

# Получаем текущую версию из Git
VERSION=$(git describe --tags --always 2>/dev/null)
GIT_HASH=$(git rev-parse --short HEAD 2>/dev/null || echo "unknown")
GIT_BRANCH=$(git rev-parse --abbrev-ref HEAD 2>/dev/null || echo "unknown")

# Если нет тегов — дефолтная версия
if [ -z "$VERSION" ]; then
    VERSION="unknown"
fi

# Добавляем 'v', если его нет
if [[ "$VERSION" != v* ]]; then
    VERSION="v$VERSION"
fi

# Сохраняем в файл
echo "#pragma once" > version/version.hpp
echo "#define APP_VERSION \"$VERSION\"" >> version/version.hpp
echo "#define GIT_COMMIT \"$GIT_HASH\"" >> version/version.hpp
echo "#define GIT_BRANCH \"$GIT_BRANCH\"" >> version/version.hpp

echo -e "\e[32m🔄 Версия: $VERSION\e[0m"