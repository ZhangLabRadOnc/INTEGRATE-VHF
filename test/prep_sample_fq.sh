#!/usr/bin/env bash

# Define the target root directories
folders=("simulated_reads_100p" "simulated_reads_1p")

# Array of 18 genes
genes=(
  'ARF5' 'CD99' 'CSDE1' 'GABARAPL2' 'GCLC' 'GPRC5A'
  'GRN' 'HSPA5' 'IL32' 'LAP3' 'LYPLA2' 'MRPS10'
  'NDUFAB1' 'PSMB1' 'PSMC4' 'STRAP' 'SYPL1' 'UQCRC1'
)

# Outer loop: Iterate through each dilution directory
for folder in "${folders[@]}"; do
  echo "Processing top-level directory: ${folder}"

  # Inner loop: Run 9 times for 9 to group reads for 9 separate samples (N = 1 to 9)
  for n in {1..9}; do
    # Compute 0-based array indices for the gene pair (2 genes per iteration)
    idx1=$(( (n - 1) * 2 ))
    idx2=$(( idx1 + 1 ))

    gene1="${genes[$idx1]}"
    gene2="${genes[$idx2]}"

    # Construct directory path and create sample folder
    sample_dir="${folder}/sample${n}"
    mkdir -p "${sample_dir}"
    echo $sample_dir

    # Select the reads from the desired 2 integration genes for this sample
    grep -A1 $gene1 $folder/sample_01_1.fasta > $sample_dir/sim_1.fasta
    grep -A1 $gene2 $folder/sample_01_1.fasta >> $sample_dir/sim_1.fasta

    grep -A1 $gene1 $folder/sample_01_2.fasta > $sample_dir/sim_2.fasta
    grep -A1 $gene2 $folder/sample_01_2.fasta >> $sample_dir/sim_2.fasta

    # Convert FASTA reads file to FASTQ
    seqtk seq -F 'I' $sample_dir/sim_1.fasta | gzip > $sample_dir/sim_1.fq.gz
    seqtk seq -F 'I' $sample_dir/sim_2.fasta | gzip > $sample_dir/sim_2.fq.gz

    # Combine simulated reads with the real HPV-negative sample reads to create the simulated sample
    cat TCGA-38-4631.RNA.1.fq.gz $sample_dir/sim_1.fq.gz > $sample_dir/spiked_R1.fq.gz
    cat TCGA-38-4631.RNA.2.fq.gz $sample_dir/sim_2.fq.gz > $sample_dir/spiked_R2.fq.gz

  done
done
