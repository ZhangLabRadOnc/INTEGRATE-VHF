# Install polyester if required
if (!requireNamespace("BiocManager", quietly = TRUE))
  install.packages("BiocManager")

library(polyester)
library(Biostrings)

#setwd()

# Load FASTA
fasta_file <- "chimeric_transcripts.fasta"
transcripts <- readDNAStringSet(fasta_file)
num_tx <- length(transcripts)

# Load read depths
count_info <- read.table("reads_per_transcript.txt", header = TRUE, sep = "\t")

# Define target parameters
read_len <- 48        # Read length in bp
is_paired <- TRUE

# Get transcript lengths (L)
tx_lengths <- width(transcripts)

# Compute required read count vector
if (is_paired) {
  reads_needed <- round((count_info$count * tx_lengths) / (2 * read_len))
} else {
  reads_needed <- round((count_info$count * tx_lengths) / read_len)
}

# Ensure minimum count of 1 read pair if length is very small
reads_needed <- pmax(reads_needed, 1)

# Set up counts matrix (rows = transcripts, cols = replicates)
reads_matrix <- matrix(reads_needed, nrow = num_tx, ncol = 1)

# Define fold_changes matrix of 1s (no differential expression adjustment)
fc_matrix <- matrix(1, nrow = num_tx, ncol = 1)

# Run Polyester simulation
simulate_experiment(
  fasta = fasta_file,
  reads_per_transcript = reads_matrix,
  fold_changes = fc_matrix,     # Supply fold_changes matrix
  num_reps = c(1),
  readlen = read_len, #To match TCGA BAM 48
  paired = TRUE,
  distr = 'normal',
  error_model = "illumina5",
  outdir = "simulated_reads_100p",
  write_gff = FALSE
)


# Now do 1% of target depths
reads_needed_perc = reads_needed*0.01
reads_needed_perc <- pmax(reads_needed_perc, 1)

# Set up counts matrix (rows = transcripts, cols = replicates)
reads_matrix_perc <- matrix(reads_needed_perc, nrow = num_tx, ncol = 1)

# Run Polyester simulation
simulate_experiment(
  fasta = fasta_file,
  reads_per_transcript = reads_matrix_perc,
  fold_changes = fc_matrix,     # Supply fold_changes matrix
  num_reps = c(1),
  readlen = read_len, #To match TCGA BAM
  paired = TRUE,
  distr = 'normal',
  error_model = "illumina5",
  outdir = "simulated_reads_1p",
  write_gff = FALSE
)


