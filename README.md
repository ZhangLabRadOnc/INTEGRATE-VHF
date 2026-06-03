# INTEGRATE-VHF
Documentation for the INTEGRATE-VHF tool. INTEGRATE-VHF is a virus-host gene fusion detection tool for next-generation sequecing (NGS) data. It is written in C++.

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
    
## Installation
Download INTEGRATE-VHF
```
git clone https://github.com/ZhangLabRadOnc/HPV-EM.git
```

Compile INTEGRATE-VHF
```
(1) cd IntegrateVHF
(2) mkdir build
(3) cmake -G Ninja -S ./ -B ./build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=/usr/local
(4) ninja -C ./build install
```
The executable Integrate is now at /usr/local/bin/integratevhf

## Data Preparation
### 1. Reference Genome (`reference.fasta`)
The reference genome should contain **both host and virus sequences**.

### 2. Virus Annotations (`virus_index.txt`, `virus_genotype.gff`)
The tool requires virus annotations be in GFF format, including both features and the corresponding FASTA sequence. Virus annotations can be passed to the tool in two different ways:
* `-virusIndex virus_index.txt -virusType:HPVEM /path/to/hpvEM.results.tsv`, designed to be used with the output of the [HPV-EM](https://github.com/jin-wash-u/HPV-EM) tool. This method accepts a file, `virus_index.txt`, containing the names of one or more viral genotypes together with the path their corresponding virus annotation files in GFF3 format. The `-virusType` option is designated as `HPVEM`, and this is followed by the path to the HPV-EM results file for your sample. Below is an example of the `virus_index.txt` file:
```
[HPV147]
NAME=HPV147
GFF{HPV147REF}=path/to/HPV147.gff
[HPV95]
NAME=HPV95
GFF{HPV95REF}=path/to/HPV95.gff
```
* `-virusType:HPV16 /path/to/HPV16.gff` designed for general use, specifying the name of a single virus of interest and the path to its associated GFF. All GFFs must follow the GFF3 format, containing both features and the corresponding FASTA sequence.

### 3. BAM files (`accepted_hits.bam`, `unmapped.bam`, `dna.tumor.bam`, `dna.normal.bam`)
All BAMs (RNA-seq, WGS tumor, WGS normal) for a sample should be aligned to a single reference genome containing **both human and virus sequences**. [STAR](https://github.com/alexdobin/STAR) is the preferred tool for RNA-seq alignment, while tools like [BWA](http://bio-bwa.sourceforge.net/) can be used for WGS alignment. Unmapped reads may be output into the same file as mapped reads. If so, pass this file to the command as both the `accepted_hits.bam` and `unmapped.bam`.

### 4. Human Annotations (`human_annot.txt`)
Human gene annotations can be downloaded from UCSC genome browser or alternatively created from a GTF file. The following example workflow demonstrates the creation of this annotation file from a downloaded GTF:
```
a. Download GTF file from http://ftp.ensembl.org/pub/current_gtf/homo_sapiens/Homo_sapiens.GRCh38.115.gtf.gz
b. Run the following commands:
(1) gtfToGenePred -genePredExt -geneNameAsName2 Homo_sapiens.GRCh38.115.gtf Homo_sapiens.GRCh38.115.genePred
(2) cut -f 1-10,12 Homo_sapiens.GRCh38.115.genePred > tmp.txt
(3) echo -e "#ensGene.name\tensGene.chrom\tensGene.strand\tensGene.txStart\tensGene.txEnd\tensGene.cdsStart\tensGene.cdsEnd\tensGene.exonCount\tensGene.exonStarts\tensGene.exonEnds\tensemblToGeneName.value" > human_annot.txt
(4) cat tmp.txt >> human_annot.txt
```

### 5. Create BWTs (`mkbwt`)
* Using a **VirusIndex** file
```
integratevhf mkbwt (options) -virusIndex virus_index.txt reference.fasta
```

* Using **individual virus GFF3** files
```
integratevhf mkbwt (options) -virusType:{virus name} virus_annot.gff reference.fasta
```
# Running INTEGRATE-VHF

1. Using a **VirusIndex** file
* With RNA-seq BAMs only:
```
integratevhf fusion (options) -virusIndex virus_index.txt -virusType:HPVEM hpvEM_results.tsv reference.fasta human_annot.txt directory_to_bwt accepted_hits.bam unmapped.bam
```
* With RNA-seq and Tumor DNA BAMs:
```
integratevhf fusion (options) -virusIndex virus_index.txt -virusType:HPVEM hpvEM_results.tsv reference.fasta human_annot.txt directory_to_bwt accepted_hits.bam unmapped.bam dna.tumor.bam
```
* With RNA-seq, Tumor DNA, and matched Normal DNA BAMs:
```
integratevhf fusion (options) -virusIndex virus_index.txt -virusType:HPVEM hpvEM_results.tsv reference.fasta human_annot.txt directory_to_bwt accepted_hits.bam unmapped.bam dna.tumor.bam dna.normal.bam
```
2. Using **individual virus GFF3** files
* With RNA-seq BAMs only:
```
integratevhf fusion (options) -virusType:{X} virus_annot.gff reference.fasta human_annot.txt directory_to_bwt accepted_hits.bam unmapped.bam
```
* With RNA-seq and Tumor DNA BAMs:
```
integratevhf fusion (options) -virusType:{X} virus_annot.gff reference.fasta human_annot.txt directory_to_bwt accepted_hits.bam unmapped.bam dna.tumor.bam
```
* With RNA-seq, Tumor DNA, and matched Normal DNA BAMs:
```
integratevhf fusion (options) -virusType:{X} virus_annot.gff reference.fasta human_annot.txt directory_to_bwt accepted_hits.bam unmapped.bam dna.normal.bam
```


## Output
Upon a successful run, INTEGRATE-VHF outputs the following files:
* `summary.tsv`: Fusion-level summary including gene partners, support tier, and splice structure
* `reads.tsv`: Read-level evidence from encompassing and spanning reads
* `exons.tsv`: Transcript and exon annotation for detected fusion junctions
* `breakpoints.tsv`: Nucleotide-resolution breakpoint coordinates
* `fusions.bedpe`: Fusion breakpoints reported in BEDPE format
