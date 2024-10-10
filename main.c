#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TARGET_LENGTH 55
#define MIN_OVERLAP 2
#define MAX_SEQUENCES 8
#define MAX_SEQ_LENGTH 1000

void
reverse_complement(char *seq, char *rev_comp) {
    int len = strlen(seq);
    for (int i = 0; i < len; i++) {
        switch (seq[len - 1 - i]) {
        case 'A':
            rev_comp[i] = 'T';
            break;
        case 'T':
            rev_comp[i] = 'A';
            break;
        case 'C':
            rev_comp[i] = 'G';
            break;
        case 'G':
            rev_comp[i] = 'C';
            break;
        default:
            rev_comp[i] = seq[len - 1 - i];
            break;
        }
    }
    rev_comp[len] = '\0';
}

int
overlap(char *seq1, char *seq2, int min_overlap) {
    int len1 = strlen(seq1), len2 = strlen(seq2);
    for (int i = min_overlap; i <= len1; i++) {
        if (strncmp(seq1 + len1 - i, seq2, i) == 0)
            return i;
    }
    return 0;
}

char *sequences[] = {
    "ATCCGTTGAAGCCGCGGGC", 
    "TTAACTCGAGG", 
    "TTAAGTACTGCCCG", 
    "ATCTGTGTCGGG", 
    "CGACTCCCGACACA", 
    "CACAGATCCGTTGAAGCCGCGGG", 
    "CTCGAGTTAAGTA", 
    "CGCGGGCAGTACTT"
};

int graph[8][8];

void
visualize_graph() {
    printf("Adjacency Matrix:\n");
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            printf("%d ", graph[i][j]);
        }
        printf("\n");
    }
}

int
is_valid(int v, int pos, int path[], int V) {
    if (graph[path[pos - 1]][v] == 0)
        return 0;
    for (int i = 0; i < pos; i++)
        if (path[i] == v)
            return 0;
    return 1;
}

int
hamiltonian_path_util(int path[], int pos, int V) {
    if (pos == V)
        return 1;
    for (int v = 0; v < V; v++) {
        if (is_valid(v, pos, path, V)) {
            path[pos] = v;
            if (hamiltonian_path_util(path, pos + 1, V))
                return 1;
            path[pos] = -1;
        }
    }
    return 0;
}

void
hamiltonian_path(int V) {
    int path[8];
    memset(path, -1, sizeof(path));
    path[0] = 0;

    if (hamiltonian_path_util(path, 1, V)) {
        printf("Hamiltonian Path: ");
        for (int i = 0; i < V; i++)
            printf("%d ", path[i]);
        printf("\n");
    } else {
        printf("No Hamiltonian Path found.\n");
    }
}

void
output_graph_structure(int V) {
    FILE *fp = fopen("graph_structure.txt", "w");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return;
    }

    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if (graph[i][j]) {
                fprintf(fp, "%d %d\n", i, j);
            }
        }
    }

    fclose(fp);
    printf("Graph structure output to graph_structure.txt\n");
}

void
find_best_path(int V) {
    int path[MAX_SEQUENCES];
    int best_path[MAX_SEQUENCES];
    int visited[MAX_SEQUENCES] = {0};
    int best_length = 0;

    for (int start = 0; start < V; start++) {
        int length = 1;
        path[0] = start;
        visited[start] = 1;

        for (int i = 1; i < V; i++) {
            int next = -1;
            for (int j = 0; j < V; j++) {
                if (!visited[j] && graph[path[i-1]][j]) {
                    next = j;
                    break;
                }
            }
            if (next == -1)
                break;
            path[i] = next;
            visited[next] = 1;
            length++;
        }

        if (length > best_length) {
            best_length = length;
            memcpy(best_path, path, sizeof(int) * length);
        }

        memset(visited, 0, sizeof(visited));
    }

    printf("Debug: Best path found:\n");
    for (int i = 0; i < best_length - 1; i++) {
        printf("%d -> %d (Edge exists: %s)\n",
               best_path[i], best_path[i+1], 
               graph[best_path[i]][best_path[i+1]] ? "Yes" : "No");
    }

    FILE *fp = fopen("best_path.txt", "w");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return;
    }

    for (int i = 0; i < best_length - 1; i++) {
        fprintf(fp, "%d %d\n", best_path[i], best_path[i+1]);
    }

    fclose(fp);
    printf("Best path output to best_path.txt\n");
}

void
find_consensus_and_print_aligned(char **sequences, int n) {
    char best_consensus[TARGET_LENGTH + 1] = "";
    int best_length = 0;
    char temp_seq[1000];
    char aligned[MAX_SEQUENCES][TARGET_LENGTH + 1];
    memset(aligned, '-', sizeof(aligned));

    for (int i = 0; i < n; i++) {
        strcpy(temp_seq, sequences[i]);
        for (int j = 0; j < n; j++) {
            if (i == j)
                continue;
            char rev_comp[1000];
            reverse_complement(sequences[j], rev_comp);
            
            int overlap_direct = overlap(temp_seq, sequences[j], MIN_OVERLAP);
            int overlap_revcomp = overlap(temp_seq, rev_comp, MIN_OVERLAP);

            if (strlen(temp_seq) >= TARGET_LENGTH)
                break;

            if (overlap_direct > overlap_revcomp) {
                strcat(temp_seq, sequences[j] + overlap_direct);
            } else {
                strcat(temp_seq, rev_comp + overlap_revcomp);
            }
        }

        int temp_length = strlen(temp_seq);
        if (temp_length > TARGET_LENGTH)
            temp_length = TARGET_LENGTH;
        if (temp_length > best_length) {
            best_length = temp_length;
            strncpy(best_consensus, temp_seq, TARGET_LENGTH);
            best_consensus[TARGET_LENGTH] = '\0';
        }
    }

    printf("Best Consensus Sequence: %s\n", best_consensus);
    printf("Length: %d\n", best_length);

    for (int i = 0; i < n; i++) {
        char *seq = sequences[i];
        char *pos = strstr(best_consensus, seq);
        if (pos) {
            int start = pos - best_consensus;
            strncpy(aligned[i] + start, seq, strlen(seq));
        } else {
            char rev_comp[MAX_SEQ_LENGTH];
            reverse_complement(seq, rev_comp);
            pos = strstr(best_consensus, rev_comp);
            if (pos) {
                int start = pos - best_consensus;
                strncpy(aligned[i] + start, rev_comp, strlen(rev_comp));
            } else {
                strncpy(aligned[i], seq, strlen(seq));
            }
        }
        aligned[i][TARGET_LENGTH] = '\0';
    }

    printf("Aligned Sequences:\n");
    for (int i = 0; i < n; i++) {
        printf("%s\n", aligned[i]);
    }
    printf("Consensus: %s\n", best_consensus);
}

int
main() {
    int n = 8;
    char *sequences[] = {
        "ATCCGTTGAAGCCGCGGGC", 
        "TTAACTCGAGG", 
        "TTAAGTACTGCCCG", 
        "ATCTGTGTCGGG", 
        "CGACTCCCGACACA", 
        "CACAGATCCGTTGAAGCCGCGGG", 
        "CTCGAGTTAAGTA", 
        "CGCGGGCAGTACTT"
    };

    find_consensus_and_print_aligned(sequences, n);

    int linkage_threshold = MIN_OVERLAP;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i != j) {
                int link = overlap(sequences[i], sequences[j], linkage_threshold);
                graph[i][j] = link > 0 ? 1 : 0;
            }
        }
    }

    visualize_graph();
    output_graph_structure(n);
    find_best_path(n);

    return 0;
}

