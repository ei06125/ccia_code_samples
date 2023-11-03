#!/bin/bash

echo "[configure.sh][TRACE] Running Ubuntu configuration script"
git config core.hooksPath ./tools/scripts/git/hooks
chmod a+x ./tools/scripts/git/hooks/pre-commit
chmod a+x ./tools/scripts/git/hooks/pre-push
