#!/bin/bash

$(git diff --name-only ${TRAVIS_COMMIT}^ ${TRAVIS_COMMIT} | grep build_deps/Dockerfile > /dev/null)
dockerfile_status=$?

if [[ ${dockerfile_status} == "0" ]]
then
    eval "docker push tyler569/nightingale-build"
fi
