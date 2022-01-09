#!/bin/bash

for f in maze*.pbf
do
  echo "solving ${f}..."
  ./mazesolver "${f}"
done
