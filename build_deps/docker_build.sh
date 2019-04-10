#!/bin/bash

$(git diff --name-only ${TRAVIS_COMMIT}^ ${TRAVIS_COMMIT} | grep build_deps/Dockerfile > /dev/null)
dockerfile_status=$?


eval "docker pull tyler569/nightingale-build:latest"

if [[ ${dockerfile_status} == "0" ]]
then
    eval "docker build -t tyler569/nightingale-build --cache-from tyler569/nightingale-build:latest build_deps/"
fi

