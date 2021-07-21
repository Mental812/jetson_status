#!/bin/bash
YELLOW='\033[1;33m'
GREEN='\033[1;32m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${YELLOW}google.${YELLOW}\c "
echo -e "${NC}${NC}\c"
pyinstaller -n Jetson_status Main.py >/dev/null 2>&1
echo -e "${GREEN} ....done${GREEN}"
echo -e "${NC}${NC}"

if [ -d "dist/Jetson_status" ]; then
    echo -e "${YELLOW}Package program${YELLOW}"
    if [ -d "dist/Jetson_status/jtop" ]; then
        echo -e "${RED}jtop is not empty${RED}"
    else
        cp -r data/jtop dist/Jetson_status 
    fi    
    echo -e "${YELLOW}Package program${YELLOW}"
    echo -e "${NC}${NC}"
fi