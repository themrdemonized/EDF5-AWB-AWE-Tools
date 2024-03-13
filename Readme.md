This repo contains tools to edit AWB sound archives with respective AWE files for Earth Defense Force 5 (EDF5)

AWB editing is dividing into 3 steps

0. Prepare folders to unpack files (0_PrepareCleanFolders.ps1)
1. Unpack AWB and convert files to WAV (1_UnpackAWBToWAV.ps1)
2. Pack WAV files into new OUT.AWB file (2_PackWAVToAWB.ps1)

Tools used:
awe_parser.py from Another EDF Tools repo with my edits to handle different file counts in AWB and AWE
AWB_repacker
HCA decoder
VGAudioCli
