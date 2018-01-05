#!/bin/bash

netstat -ntp 2>/dev/null | grep -e :8490 -e :8491 -e :8250
