#!/bin/bash
YELLOW='\033[1;33m'
GREEN='\033[1;32m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${YELLOW}Install Dependment Package${YELLOW}"
echo -e "${NC}${NC}"
sudo pip3 install pyinstaller
sudo pip3 install jtop
echo -e "${GREEN}Done${GREEN}"