#!/bin/bash

version=$(./bump-version.sh major)
cd ..
cmd git-release --no-log $version
