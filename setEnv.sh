#!/bin/bash
sudo sh -c "echo 0 > /proc/sys/kernel/randomize_va_space"

sudo sh -c "apt-get install openssl"

sudo sh -c "sudo apt-get install libssl-dev"
