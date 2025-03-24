#!/bin/bash
echo "PID do processo: $$"
while true; do
    echo "$(date): Processo ativo..." >> log.txt
    sleep 15
done

