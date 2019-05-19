#!/bin/bash

source /opt/ti/ti-processor-sdk-linux-am335x-evm-05.03.00.07/linux-devkit/environment-setup

$CC AM335x_deviceID.c -Wall -Werror -o AM335x_deviceID
