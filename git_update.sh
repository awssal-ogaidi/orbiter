#!/bin/bash
git status
git add .
git commit -m "version of $(date)"
git push origin master


