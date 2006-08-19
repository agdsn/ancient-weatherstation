#!/bin/bash
pg_dump -f structure.dump -F p -s  -h 141.30.228.39 -U losinshi wetter
pg_dump -f test_data.dump -F p -Z 9 -a  -h 141.30.228.39 -U losinshi wetter
