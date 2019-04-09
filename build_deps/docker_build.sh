#!/bin/bash

$(git diff --name-only ${TRAVIS_COMMIT}^ ${TRAVIS_COMMIT} | grep build_deps/Dockerfile > /dev/null)
dockerfile_status=$?


eval "docker pull connorj/nightingale_build:latest"

if [[ ${dockerfile_status} == "0" ]]
then
    eval "docker build -t connorj/nightingale_build --cache-from connorj/nightingale_build:latest build_deps/"
fi