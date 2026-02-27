#!/bin/bash
URL="https://httpbin.org/get"
while true; do
    echo "--- $(date) ---"
    curl "$URL"
    echo
    sleep 1
done
