#!/bin/bash
# Set environment variables for the current shell
export OMP_PLACES="ll_caches"
export OMP_WAIT_POLICY="passive"
export OMP_NUM_THREADS=8
export OMP_MAX_ACTIVE_LEVELS=2
