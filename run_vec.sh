#!/bin/sh
#
#PBS -N teste
#PBS -l walltime=20:00
#PBS -l nodes=1:ppn=1:r662
#PBS -q mei
#PBS -o out/output_vec.txt
#PBS -e err/error.txt

echo $HOSTNAME

cd "$PBS_O_WORKDIR"

module load gcc/5.3.0
module load papi/5.4.1
source /share/apps/intel/parallel_studio_xe_2019/compilers_and_libraries_2019/linux/bin/compilervars.sh intel64
#source /share/apps/intel/advisor_2019.4.0.597843/advixe-vars.sh

echo "Compiling..."

implementation_array="IMP1 IMP2 IMP3"
matrix_size_array="L1 L2"
transposed_array="N Y"

make clean

for t in $transposed_array; do
	echo "-------------------------------------- TRANSPOSED: $t -------------------------------------------"
for i in $implementation_array; do
for j in $matrix_size_array; do	
	echo "-------------------------------------- IMPLEMENTATION: $i | MATRIX_SIZE: $j --------------------------------------"
	FILENAME="matrix_mult_${i}_${j}_${t}"
	make vec IMPL=$i M_SIZE=$j TRANS=$t FILENAME=$FILENAME
	echo "Running the tests"
	./bin/${FILENAME}.o
done
done
done


