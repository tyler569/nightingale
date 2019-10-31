#!/bin/bash

ARCHLOWER=$(echo $ARCH | awk '{print tolower($0)}')

if [[ ${TRAVIS_SECURE_ENV_VARS} == "true" ]]
then
echo "$DOCKER_PASSWORD" | docker login -u "$DOCKER_USERNAME" --password-stdin
fi


docker pull ${DOCKER_USERNAME}/nightingale-build-${ARCHLOWER}:latest

docker build -t ${DOCKER_USERNAME}/nightingale-build-${ARCHLOWER} \
        --cache-from ${DOCKER_USERNAME}/nightingale-build-${ARCHLOWER}:latest \
        build_deps/${ARCH}/


if [[ ${TRAVIS_SECURE_ENV_VARS} == "true" ]]
then
docker push ${DOCKER_USERNAME}/nightingale-build-${ARCHLOWER}
fi

docker run --rm -v "${TRAVIS_BUILD_DIR}":/nightingale \
        --env ARCH --env BUILD_LUA \
        ${DOCKER_USERNAME}/nightingale-build-${ARCHLOWER} \
        /bin/sh -c "cd /nightingale && make"

