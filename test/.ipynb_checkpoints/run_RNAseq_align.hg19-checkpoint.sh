#!/bin/bash

# Combined hg19 human and HPV viral reference
REFDIR=hg19genomeBuild
STARINDEX=hg19genomeBuild/genomeDir_Star2.7.5
BWAINDEX=hg19genomeBuild/bwa-mem2_index/hg19_HPV.fa
BWTDIR=hg19genomeBuild/bwts

TOPDIR=$PWD

folders=("simulated_reads_100p" "simulated_reads_1p")

for folder in "${folders[@]}"; do
    for n in {1..9}; do
    	sample_dir="${folder}/sample${n}"
    	cd $sample_dir
    
    	LOGDIR=$PWD/logs
    	mkdir -p $LOGDIR
    
    	SAMPLE="${folder}_${n}"
    	echo $SAMPLE
        #Align RNA-seq reads against hg19+HPV reference
    	echo Align FASTQ && \
    	STAR \
    	    --genomeDir $STARINDEX \
    	    --readFilesIn spiked_R1.fq.gz spiked_R2.fq.gz \
    	    --readFilesCommand zcat \
    	    --runThreadN 16 \
    	    --quantMode GeneCounts \
    	    --limitSjdbInsertNsj 1200000 \
    	    --chimSegmentMin 15 \
    	    --chimMainSegmentMultNmax 1 \
    	    --chimJunctionOverhangMin 15 \
    	    --chimOutJunctionFormat 1 \
    	    --chimOutType Junctions   WithinBAM   SoftClip \
    	    --twopassMode Basic \
    	    --outSAMunmapped Within \
    	    --outSAMtype BAM Unsorted \
    	    --outSAMstrandField intronMotif \
    	    --outSAMattributes NH   HI   AS   nM   NM   ch \
    	    --outFilterType BySJout \
    	    --outFilterMultimapNmax 20 \
    	    --outFilterScoreMinOverLread 0.33 \
    	    --outFilterMatchNminOverLread 0.33 \
    	    --outFilterMismatchNmax 999 \
    	    --outFilterMismatchNoverLmax 0.1 \
    	    --outFilterIntronMotifs None \
    	    --alignIntronMin 20 \
    	    --alignIntronMax 1000000 \
    	    --alignMatesGapMax 1000000 \
    	    --alignSJoverhangMin 8 \
    	    --alignSJDBoverhangMin 1 \
    	    --alignSoftClipAtReferenceEnds Yes \
    	    --outSAMattrRGline ID:$SAMPLE PL:Illumina SM:$SAMPLE \
    	    --outFileNamePrefix $SAMPLE.RNA. &&\
        echo sort and index BAM &&\
    	samtools sort -o $SAMPLE.RNA.Aligned.sort.bam $SAMPLE.RNA.Aligned.out.bam &&\
    	samtools index $SAMPLE.RNA.Aligned.sort.bam &&\
    	rm $SAMPLE.RNA.Aligned.out.bam &&\
        rm -rf $SAMPLE.RNA._STARpass1 $SAMPLE.RNA._STARgenome"
    
    	cd $TOPDIR
    done
done
