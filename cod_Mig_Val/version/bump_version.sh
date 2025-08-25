#!/bin/bash

# Получаем текущий тег
CURRENT_TAG=$(git describe --tags --match "v*" 2>/dev/null | head -n1)

# Если нет тега — начинаем с v0.1.0
if [ -z "$CURRENT_TAG" ]; then
    NEW_VERSION="v0.1.0"
else
    # Парсим версию
    VERSION=${CURRENT_TAG#v}
    MAJOR=$(echo $VERSION | awk -F. '{print $1}')
    MINOR=$(echo $VERSION | awk -F. '{print $2}')
    PATCH=$(echo $VERSION | awk -F. '{print $3}')

    # Увеличиваем PATCH
    NEW_VERSION="v$MAJOR.$MINOR.$((PATCH + 1))"
fi

# Создаём новый тег и пушим
git tag "$NEW_VERSION"
git push origin "$NEW_VERSION"

echo -e "\e[32m🆕 Новая версия: $NEW_VERSION\e[0m"