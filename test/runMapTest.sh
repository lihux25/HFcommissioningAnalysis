#!/bin/sh

cmsRun h2testbeamanalyzer_cfg.py 000$1
python testMap.py -r $1 
cp testMap_$1.p* ~/www/HFcommissioning/.