#!/bin/bash

version=$(./bump-version.sh patch)
cd ..
cmd git-release --no-log $version
