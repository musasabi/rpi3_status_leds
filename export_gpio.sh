#!/bin/bash

for pin in 17 18 27 22 23 24 21 20 16 26 19 13; do
  gpio export $pin out
done
