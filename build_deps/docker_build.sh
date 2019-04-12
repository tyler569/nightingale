#!/bin/bash

ARCHLOWER=$(echo $ARCH | awk '{print tolower($0)}')

if [[ ${TRAVIS_SECURE_ENV_VARS} == "true" ]]
then
echo "$DOCKER_PASSWORD" | docker login -u "$DOCKER_USERNAME" --password-stdin
fi


eval "docker pull ${DOCKER_USERNAME}/nightingale-build-${ARCHLOWER}:latest"

eval "docker build -t ${DOCKER_USERNAME}/nightingale-build-${ARCHLOWER} --cache-from ${DOCKER_USERNAME}/nightingale-build-${ARCHLOWER}:latest build_deps/${ARCH}/"


if [[ ${TRAVIS_SECURE_ENV_VARS} == "true" ]]
then
eval "docker push ${DOCKER_USERNAME}/nightingale-build-${ARCHLOWER}"
fi


if [[ ${ARCH} == "X86_64" ]]
then
eval "docker run --rm -v "${TRAVIS_BUILD_DIR}":/nightingale ${DOCKER_USERNAME}/nightingale-build-${ARCHLOWER} /bin/sh -c 'cd /nightingale && make'"
else
eval "docker run --rm -v "${TRAVIS_BUILD_DIR}":/nightingale ${DOCKER_USERNAME}/nightingale-build-${ARCHLOWER} /bin/sh -c 'cd /nightingale && make iso32'"
fi
