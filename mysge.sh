#!/bin/bash
#
# Put your Job commands here.
#
#------------------------------------------------

/opt/openmpi/bin/mpirun -np $NSLOTS -machinefile $TMPDIR/machines -mca btl tcp,self,sm \
/home/team11/Desktop/LAB1/Permission_Denied/APSP $size

#------------------------------------------------
