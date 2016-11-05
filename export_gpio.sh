#!/bin/bash

for pin in 17 18 27 22 23 24 25 4 28 29 30 31; do
  gpio export $pin out
done
