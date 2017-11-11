#!/bin/sh

image_name=`docker build --quiet .`

docker tag $image_name nightingale_build

