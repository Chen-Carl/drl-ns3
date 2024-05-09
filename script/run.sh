#!/bin/bash

command_to_run="../bin/main"

times_to_run=$1

for ((i=1; i<=times_to_run; i++))
do
   $command_to_run
done