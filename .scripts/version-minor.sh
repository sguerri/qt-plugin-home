#!/bin/bash

version=$(./bump-version.sh minor)
cd ..
cmd git-release --no-log $version
