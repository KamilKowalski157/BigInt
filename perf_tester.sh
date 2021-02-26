#!/bin/bash
rm log.txt
power=1
tests=10000000
make
for i in {1..15}
do
    echo ",$(./BigInt5 'm&d' $tests $power) " >> log.txt
    power=$((power * 2))
    tests=$((tests / 2))
done