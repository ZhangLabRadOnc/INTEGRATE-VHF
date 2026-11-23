#!/bin/bash

# Combined hg19 human and HPV viral reference
REFDIR=hg19genomeBuild

TOPDIR=$PWD

folders=("simulated_reads_100p" "simulated_reads_1p")

for folder in "${folders[@]}"; do
    for n in {1..9}; do
    	sample_dir="${folder}/sample${n}"
    	cd $sample_dir
    
    	LOGDIR=$PWD/logs
    	mkdir -p $LOGDIR
        
    	#Skip this case if IntegrateVHF output files already exist
    	[ -e "$OUTDIR/summary.tsv" ] && continue
        
    	#Run integrate VHF using the aligned RNA-seq BAM
    	SAMPLE="${folder}_${n}"
    	echo $SAMPLE
    
    	RNABAM=$SAMPLE.RNA.Aligned.sort.bam
    
    	echo Running Integrate VHF && \
    	  integratevhf fusion \
    	    -mb 100 \
    	    -cfn 1 \
    	    -normcfn 0.1 \
    	    -virusType:HPV16 $REFDIR/HPVREF/HPV16REF.gff \
    	    ${REFDIR}/hg19_HPV.fa \
    	    ${REFDIR}/annot.ensembl.txt \
    	    ${BWTDIR} \
    	    $RNABAM \
    	    $RNABAM
    
    	cd $TOPDIR
    done
done

