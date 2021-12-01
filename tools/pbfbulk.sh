#!/bin/bash

for f in *.pbf
do
  echo "converting $f..."
  pbf2img.py "$f" "png"
done
