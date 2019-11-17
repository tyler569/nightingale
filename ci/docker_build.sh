#!/bin/bash

ARCHLOWER=$(echo $ARCH | awk '{print tolower($0)}')

if [[ ${TRAVIS_SECURE_ENV_VARS} == "true" ]]; then
    echo "$DOCKER_PASSWORD" | docker login -u "$DOCKER_USERNAME" --password-stdin
fi

IMAGE="${DOCKER_USERNAME}/nightingale-build-${ARCHLOWER}"

docker pull ${IMAGE}:latest

docker build -t ${IMAGE} --cache-from ${IMAGE}:latest ci/${ARCH}/


if [[ ${TRAVIS_SECURE_ENV_VARS} == "true" ]]; then
    docker push ${IMAGE}
fi

docker run --rm -v "${TRAVIS_BUILD_DIR}":/nightingale \
        --env ARCH --env BUILD_LUA --env USE_CLANG \
        ${IMAGE} /bin/sh -c "cd /nightingale && make"

