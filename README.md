# INTEGRATE-VHF
Documentation for the INTEGRATE-VHF tool. INTEGRATE-VHF is a virus-host gene fusion detection tool for next-generation sequecing (NGS) data. It is written in C++.

## Installation
Download INTEGRATE-VHF at link

## Setup
**Docker Setup (Optional)**

To simplify installation and ensure reproducibility, INTEGRATE-VHF can be run using a Docker container:

      docker pull hasabi3/integrate-dev
      docker run -it hasabi3/integrate-dev

**1. System Requirements**
- Linux-based operating system (recommended)
- C++ compiler (e.g., g++ with C++11 support or later)
- Sufficient memory and storage for NGS data processing

**2. Dependencies**

INTEGRATE-VHF requires the following system libraries and tools to be installed prior to compilation:

  - **Compiler and Build Tools**
  
          clang / clang++ (C++ compiler with OpenMP support)
          cmake (>= 3.10)
          ninja (build system)
          pkg-config
  - **Required Libraries**
    
          HTSlib (libhts)
          divsufsort (libdivsufsort)
          OpenMP (libomp)
  - **Bioinformatics Tools**
    
          samtools 
  - **Optional (Recommended for full pipeline)**
  
        STAR: RNA-seq alignment
        BWA: DNA alignment
      
  - **Installation (Ubuntu example)**
    
          sudo apt-get update
          sudo apt-get install -y \
          clang cmake ninja-build pkg-config \
          libhts-dev libdivsufsort-dev libomp-dev \
          samtools


## Data Preparation
### 1. Reference Genome (`reference.fasta`)
The reference genome should contain **both host and virus sequences**.

### 2. Virus Annotations (`virus_index.txt`, `virus_genotype.gff`)
Virus annotations should be given to the tool in GFF format, including both features and the corresponding FASTA sequence. Virus annotations can be passed to the tool in two different ways:
* `-virusIndex virus_index.txt -virusType:HPVEM /path/to/hpvEM.results.tsv`, designed to be used with the output of the [HPV-EM](https://github.com/jin-wash-u/HPV-EM) tool, providing paths to one or more virus annotation files in GFF3 format with the corresponding virus name. Below is an example of the `virus_index.txt` file.
```
[HPV147]
NAME=HPV147
GFF{HPV147REF}=path/to/HPV147.gff
[HPV95]
NAME=HPV95
GFF{HPV95REF}=path/to/HPV95.gff
```
* `-virusType:HPV16 /path/to/HPV16.gff` designed for general use, providing the path to a single virus of interest and its associated GFF. All GFFs must follow the GFF3 format, contain both features and the corresponding FASTA sequence.

### 3. BAM files (`accepted_hits.bam`, `unmapped.bam`, `dna.tumor.bam`, `dna.normal.bam`)
All BAMs (RNA-seq, WGS tumor, WGS normal) for a sample should be aligned to a single reference genome containing **both human and virus sequences**. [STAR](https://github.com/alexdobin/STAR) is the preferred tool for RNA-seq alignment, while tools like [BWA](http://bio-bwa.sourceforge.net/) can be used for WGS alignment. Unmapped reads may be output into the same file as mapped reads. If so, pass this file to the command twice.

### 4. Human Annotations (`human_annot.txt`)
Human gene annotations can be downloaded from UCSC genome browser or alternatively created from a GTF file. View more details on the [INTEGRATE sourceforge page](https://sourceforge.net/p/integrate-fusion/wiki/annotation/).

### 5. Create BWTs (`mkbwt`)
* With **VirusIndex** file
```
integratevhf mkbwt (options) -virusIndex virus_index.txt reference.fasta
```

* With **individual virus GFF3** files
```
integratevhf mkbwt (options) -virusType:{virus name} virus_annot.gff reference.fasta
```
# Running INTEGRATE-VHF

1. Using a VirusIndex file
* (RNA-seq only)
```
integratevhf fusion (options) -virusIndex virus_index.txt -virusType:HPVEM hpvEM_results.tsv reference.fasta human_annot.txt directory_to_bwt accepted_hits.bam unmapped.bam
```
* (RNA-seq and Tumor DNA)
```
integratevhf fusion (options) -virusIndex virus_index.txt -virusType:HPVEM hpvEM_results.tsv reference.fasta human_annot.txt directory_to_bwt accepted_hits.bam unmapped.bam dna.tumor.bam
```
* (RNA-seq, Tumor DNA, and normal DNA)
```
integratevhf fusion (options) -virusIndex virus_index.txt -virusType:HPVEM hpvEM_results.tsv reference.fasta human_annot.txt directory_to_bwt accepted_hits.bam unmapped.bam dna.tumor.bam dna.normal.bam
```
2. Index-free mode
* (RNA-seq only)
```
integratevhf fusion (options) -virusType:{X} virus_annot.gff reference.fasta human_annot.txt directory_to_bwt accepted_hits.bam unmapped.bam
```
* (RNA-seq and Tumor DNA)
```
integratevhf fusion (options) -virusType:{X} virus_annot.gff reference.fasta human_annot.txt directory_to_bwt accepted_hits.bam unmapped.bam dna.tumor.bam
```
* (RNA-seq, Tumor DNA, and normal DNA)
```
integratevhf fusion (options) -virusType:{X} virus_annot.gff reference.fasta human_annot.txt directory_to_bwt accepted_hits.bam unmapped.bam dna.normal.bam
```


## Output
Upon a successful run, INTEGRATE-VHF outputs the following files:
* `summary.tsv`: Fusion-level summary including gene partners, support tier, and splice structure
* `reads.tsv`: Read-level evidence for encompassing and spanning reads
* `exons.tsv`: Transcript and exon annotation for detected fusion junctions
* `breakpoints.tsv`: Nucleotide-resolution breakpoint coordinates
* `fusions.bedpe`: Fusion breakpoints reported in BEDPE format
