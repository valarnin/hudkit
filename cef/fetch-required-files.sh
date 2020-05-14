#!/bin/bash

CEF_BUILD='cef_binary_81.2.15%2Bge07275d%2Bchromium-81.0.4044.92_linux64_minimal'
CEF_FOLDER='cef_binary_81.2.15+ge07275d+chromium-81.0.4044.92_linux64_minimal'
CEF_URL="http://opensource.spotify.com/cefbuilds/${CEF_BUILD}.tar.bz2"

if [[ ! -f "v8_context_snapshot.bin" ]]; then
    mkdir 'cef-project' &> /dev/null
    echo 'Fetching required CEF binaries'
    curl -ocef-project/tmp.tar.bz2 "${CEF_URL}" &> /dev/null
    pushd cef-project > /dev/null
    echo 'Extracting and moving'
    tar xf tmp.tar.bz2
    mv ${CEF_FOLDER}/Release/* ../
    mv ${CEF_FOLDER}/Resources/* ../
    echo 'Cleaning up'
    popd > /dev/null
    rm -rf cef-project
fi
