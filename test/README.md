# INTEGRATE-VHF simulated HPV integrations test
This folder contains files and scripts used to generate the benchmarking result using simulated HPV16 viral integration reads in our Bioinformatics application note, "INTEGRATE-VHF: A Virus-Host Gene Fusion Detection Tool".

## Step 1: Generate chimeric RNA FASTA and read depth files
The script to generate the chimeric reference FASTA and read depths is in prep_synthetic_reads.ipynb
Note that this step requires download of the hg19 reference genome in FASTA format (hg19.txt) in addition to the included HPV16 reference genome from PaVE (HPV16REF.fa)
Output files:
- chimeric_transcripts.fasta
- reads_per_transcript.txt

## Step 2: Generate FASTA files of simulated RNA-seq reads using polyester
Script to generate the reads: generate_RNA_reads.R

## Step 3: Convert the FASTA files to FASTQ and spike into TCGA reads data to generate a set of test samples
Script to generate the spiked-in reads files: prep_sample_fq.sh
Note that this step requires downloading and extracting the reads from the TCGA-LUAD tumor RNA-seq BAM for case TCGA-38-4631 (UUID: 29e5c4a5-3659-44c1-8609-82af113494e0)
Output files:
- spiked_R1.fq.gz and spiked_R2.fq.gz for 9 samples with simulated integration events, sample1 - sample9

## Step 4: Run STAR aligner on the simulated sample data
Script to run STAR aligner (v. 2.7.5): run_RNAseq_align.hg19.sh
This step requires the building and compiling of a combined hg19-viral reference
Output files:
- sample*.RNA.Aligned.sort.bam
- sample*.RNA.Aligned.sort.bam.bai

## Step 5: Run IntegrateVHF on the aligned BAM files
Script to run this step: run_integrateVHF_RNA.hg19.sh