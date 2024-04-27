#!/usr/bin/env bash
set -euo pipefail

cmake -B build

cmake --build build -j 4
