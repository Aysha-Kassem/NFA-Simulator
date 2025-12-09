#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_STATES 50
#define MAX_ALPHABET 20
#define MAX_TRANSITIONS 100
#define MAX_STRING 100
#define MAX_STATE_NAME 20

typedef struct {
    char start[MAX_STATE_NAME];
    char symbol; // '\0' for epsilon
    char end[MAX_STATE_NAME];
} Transition;

// Find index of state in states array
int find_state_index(char states[][MAX_STATE_NAME], int num_states, const char* name) {
    for (int i = 0; i < num_states; i++)
        if (strcmp(states[i], name) == 0) return i;
    return -1;
}

// Check if symbol is in alphabet
int is_valid_symbol(char sym, char alphabet[], int num_alphabet) {
    if (sym == '\0') return 1; // epsilon allowed
    for (int i = 0; i < num_alphabet; i++)
        if (alphabet[i] == sym) return 1;
    return 0;
}

// Epsilon closure of a set of states (indices)
void epsilon_closure(char states[][MAX_STATE_NAME], int current_indices[], int count,
                     Transition transitions[], int num_transitions,
                     char closure[][MAX_STATE_NAME], int *closure_count) {
    int stack[MAX_STATES];
    int top = -1;
    *closure_count = 0;

    for (int i = 0; i < count; i++) {
        strcpy(closure[(*closure_count)++], states[current_indices[i]]);
        stack[++top] = current_indices[i];
    }

    while (top >= 0) {
        int idx = stack[top--];
        for (int i = 0; i < num_transitions; i++) {
            if (strcmp(transitions[i].start, states[idx]) == 0 && transitions[i].symbol == '\0') {
                int exists = 0;
                for (int j = 0; j < *closure_count; j++)
                    if (strcmp(closure[j], transitions[i].end) == 0) exists = 1;
                if (!exists) {
                    strcpy(closure[(*closure_count)++], transitions[i].end);
                    int new_idx = find_state_index(states, *closure_count, transitions[i].end);
                    stack[++top] = new_idx >= 0 ? new_idx : 0;
                }
            }
        }
    }
}

// Read a float and round for number of states
int read_number_of_states(void) {
    float temp;
    int n;
    char line[50];
    while (1) {
        printf("Enter number of states (can be float, will round): ");
        if (!fgets(line, sizeof(line), stdin)) continue;
        if (sscanf(line, "%f", &temp) != 1) {
            printf("Invalid input. Try again.\n");
            continue;
        }
        n = (int)round(temp);
        if (n < 1 || n > MAX_STATES) {
            printf("Number of states out of range (1-%d). Try again.\n", MAX_STATES);
            continue;
        }
        break;
    }
    return n;
}

// Read a valid state name
void read_state_name(char *state_name) {
    char line[MAX_STATE_NAME*2];
    while (1) {
        if (!fgets(line, sizeof(line), stdin)) continue;
        if (sscanf(line, "%s", state_name) != 1) continue;
        if (strlen(state_name) >= MAX_STATE_NAME) {
            printf("State name too long. Try again: ");
            continue;
        }
        break;
    }
}

// Read a transition with validation
void read_transition(Transition *t, char states[][MAX_STATE_NAME], int num_states,
                     char alphabet[], int num_alphabet) {
    char line[100];
    char sym;
    while (1) {
        if (!fgets(line, sizeof(line), stdin)) continue;
        if (sscanf(line, "%s %c %s", t->start, &sym, t->end) != 3) {
            printf("Invalid transition format. Try again: ");
            continue;
        }
        if (find_state_index(states, num_states, t->start) == -1 ||
            find_state_index(states, num_states, t->end) == -1) {
            printf("Invalid state in transition. Try again: ");
            continue;
        }
        if (sym == 'e') sym = '\0';
        if (!is_valid_symbol(sym, alphabet, num_alphabet)) {
            printf("Symbol not in alphabet. Try again: ");
            continue;
        }
        t->symbol = sym;
        break;
    }
}

// NFA simulation
void nfa_simulate(char states[][MAX_STATE_NAME], int num_states, Transition transitions[], int num_transitions,
                  char start_state[], char final_states[][MAX_STATE_NAME], int num_final, char input[]) {

    int current_indices[MAX_STATES];
    int current_count = 1;
    current_indices[0] = find_state_index(states, num_states, start_state);

    char closure[MAX_STATES][MAX_STATE_NAME];
    int closure_count;
    epsilon_closure(states, current_indices, current_count, transitions, num_transitions, closure, &closure_count);

    printf("Step 0: Current states = { ");
    for (int i = 0; i < closure_count; i++) printf("%s ", closure[i]);
    printf("}\n");

    size_t len = strlen(input);  // ✅ Use size_t to avoid warning

    for (size_t i = 0; i < len; i++) {
        char sym = input[i];
        int next_indices[MAX_STATES], next_count = 0;

        for (int j = 0; j < closure_count; j++) {
            int idx = find_state_index(states, num_states, closure[j]);
            for (int k = 0; k < num_transitions; k++) {
                if (strcmp(transitions[k].start, states[idx]) == 0 && transitions[k].symbol == sym) {
                    int exists = 0;
                    for (int x = 0; x < next_count; x++)
                        if (strcmp(states[next_indices[x]], transitions[k].end) == 0) exists = 1;
                    if (!exists) next_indices[next_count++] = find_state_index(states, num_states, transitions[k].end);
                }
            }
        }

        if (next_count == 0) {
            printf("Step %zu: No transitions on symbol '%c'. String rejected.\n", i+1, sym);
            return;
        }

        epsilon_closure(states, next_indices, next_count, transitions, num_transitions, closure, &closure_count);

        printf("Step %zu: After symbol '%c', current states = { ", i+1, sym);
        for (int j = 0; j < closure_count; j++) printf("%s ", closure[j]);
        printf("}\n");
    }

    for (int i = 0; i < closure_count; i++)
        for (int j = 0; j < num_final; j++)
            if (strcmp(closure[i], final_states[j]) == 0) {
                printf("String ends in final state %s. Accepted.\n", closure[i]);
                return;
            }

    printf("String ends in states { ");
    for (int i = 0; i < closure_count; i++) printf("%s ", closure[i]);
    printf("}. Not accepted.\n");
}

int main(void) {
    char states[MAX_STATES][MAX_STATE_NAME];
    int num_states = read_number_of_states();

    printf("Enter state names (strings or numbers):\n");
    for (int i = 0; i < num_states; i++) {
        printf("State %d: ", i);
        read_state_name(states[i]);
    }

    // Alphabet input
    char alphabet[MAX_ALPHABET];
    int num_alphabet;
    while (1) {
        printf("Enter number of symbols in the alphabet (1-%d): ", MAX_ALPHABET);
        char line[20];
        if (!fgets(line, sizeof(line), stdin)) continue;
        if (sscanf(line, "%d", &num_alphabet) != 1 || num_alphabet < 1 || num_alphabet > MAX_ALPHABET) {
            printf("Invalid number. Try again.\n");
            continue;
        }
        break;
    }

    printf("Enter alphabet symbols (single characters, one per line):\n");
    for (int i = 0; i < num_alphabet; i++) {
        while (1) {
            char line[10];
            if (!fgets(line, sizeof(line), stdin)) continue;
            if (sscanf(line, " %c", &alphabet[i]) != 1) {
                printf("Invalid symbol. Try again: ");
                continue;
            }
            break;
        }
    }

    // Transitions
    int num_transitions;
    while (1) {
        printf("Enter number of transitions (0-%d): ", MAX_TRANSITIONS);
        char line[20];
        if (!fgets(line, sizeof(line), stdin)) continue;
        if (sscanf(line, "%d", &num_transitions) != 1 || num_transitions < 0 || num_transitions > MAX_TRANSITIONS) {
            printf("Invalid number of transitions. Try again.\n");
            continue;
        }
        break;
    }

    Transition transitions[MAX_TRANSITIONS];
    printf("Enter transitions (start symbol end), use 'e' for epsilon:\n");
    for (int i = 0; i < num_transitions; i++) {
        printf("Transition %d: ", i+1);
        read_transition(&transitions[i], states, num_states, alphabet, num_alphabet);
    }

    // Start state
    char start_state[MAX_STATE_NAME];
    while (1) {
        printf("Enter start state: ");
        read_state_name(start_state);
        if (find_state_index(states, num_states, start_state) == -1) {
            printf("Invalid start state. Try again.\n");
            continue;
        }
        break;
    }

    // Final states
    int num_final;
    while (1) {
        printf("Enter number of final states (0-%d): ", num_states);
        char line[20];
        if (!fgets(line, sizeof(line), stdin)) continue;
        if (sscanf(line, "%d", &num_final) != 1 || num_final < 0 || num_final > num_states) {
            printf("Invalid number. Try again.\n");
            continue;
        }
        break;
    }

    char final_states[MAX_STATES][MAX_STATE_NAME];
    for (int i = 0; i < num_final; i++) {
        while (1) {
            printf("Final state %d: ", i+1);
            read_state_name(final_states[i]);
            if (find_state_index(states, num_states, final_states[i]) == -1) {
                printf("Invalid final state. Try again.\n");
                continue;
            }
            break;
        }
    }

    // Input string
    char input[MAX_STRING];
    while (1) {
        printf("Enter string to test (max %d chars): ", MAX_STRING-1);
        if (!fgets(input, sizeof(input), stdin)) continue;
        if (sscanf(input, "%s", input) != 1 || strlen(input) >= MAX_STRING) {
            printf("Invalid or too long input. Try again.\n");
            continue;
        }
        break;
    }

    printf("All inputs validated. Simulating NFA...\n\n");
    nfa_simulate(states, num_states, transitions, num_transitions, start_state, final_states, num_final, input);

    return 0;
}
