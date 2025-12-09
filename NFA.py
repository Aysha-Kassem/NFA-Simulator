import math

# ---------- Helper Functions ----------
def read_number_of_states():
    while True:
        try:
            n = float(input("Enter number of states (can be float, will round): "))
            n = round(n)
            if 1 <= n <= 50:
                return n
            print("Number of states must be between 1 and 50.")
        except ValueError:
            print("Invalid input. Try again.")

def read_state_name(prompt):
    while True:
        name = input(prompt).strip()
        if 0 < len(name) <= 20:
            return name
        print("Invalid state name. Try again (1-20 chars).")

def read_alphabet():
    while True:
        try:
            n = int(input("Enter number of symbols in the alphabet (1-20): "))
            if 1 <= n <= 20:
                break
            print("Number out of range.")
        except ValueError:
            print("Invalid input.")
    alphabet = []
    print("Enter alphabet symbols (one per line):")
    while len(alphabet) < n:
        sym = input(f"Symbol {len(alphabet)+1}: ").strip()
        if len(sym) == 1 and sym != 'e':
            alphabet.append(sym)
        else:
            print("Invalid symbol. Must be a single character and not 'e'.")
    return alphabet

def read_transitions(states, alphabet):
    while True:
        try:
            n = int(input("Enter number of transitions (0-100): "))
            if 0 <= n <= 100:
                break
            print("Number out of range.")
        except ValueError:
            print("Invalid input.")
    transitions = []
    print("Enter transitions (start symbol end), use 'e' for epsilon:")
    while len(transitions) < n:
        parts = input(f"Transition {len(transitions)+1}: ").split()
        if len(parts) != 3:
            print("Invalid format. Use: start symbol end")
            continue
        start, sym, end = parts
        if start not in states or end not in states:
            print("Invalid states. Try again.")
            continue
        if sym != 'e' and sym not in alphabet:
            print("Symbol not in alphabet. Try again.")
            continue
        if sym == 'e':
            sym = None  # Represent epsilon as None
        transitions.append({'start': start, 'symbol': sym, 'end': end})
    return transitions

def epsilon_closure(states_set, transitions):
    closure = set(states_set)
    stack = list(states_set)
    while stack:
        state = stack.pop()
        for t in transitions:
            if t['start'] == state and t['symbol'] is None and t['end'] not in closure:
                closure.add(t['end'])
                stack.append(t['end'])
    return closure

# ---------- NFA Simulation ----------
def nfa_simulate(states, transitions, start_state, final_states, input_string):
    # Initialize with epsilon closure of start state
    current_states = epsilon_closure({start_state}, transitions)
    print(f"Step 0: Current states = {sorted(current_states)}")

    for i, sym in enumerate(input_string):
        next_states = set()
        for state in current_states:
            for t in transitions:
                if t['start'] == state and t['symbol'] == sym:
                    next_states.add(t['end'])

        # Compute epsilon closure for all next states
        current_states = epsilon_closure(next_states, transitions)
        print(f"Step {i+1}: After symbol '{sym}', current states = {sorted(current_states)}")

    # Final states reached
    print(f"\nFinal states reached: {sorted(current_states)}")
    if any(state in final_states for state in current_states):
        print("Result: String ACCEPTED.")
    else:
        print("Result: String REJECTED.")

# ---------- Main ----------
def main():
    num_states = read_number_of_states()
    states = [read_state_name(f"State {i}: ") for i in range(num_states)]
    alphabet = read_alphabet()
    transitions = read_transitions(states, alphabet)

    while True:
        start_state = read_state_name("Enter start state: ")
        if start_state in states:
            break
        print("Invalid start state. Try again.")

    while True:
        try:
            num_final = int(input(f"Enter number of final states (0-{num_states}): "))
            if 0 <= num_final <= num_states:
                break
            print("Invalid number.")
        except ValueError:
            print("Invalid input.")

    final_states = []
    for i in range(num_final):
        while True:
            fs = read_state_name(f"Final state {i+1}: ")
            if fs in states:
                final_states.append(fs)
                break
            print("Invalid final state. Try again.")

    input_string = input("Enter string to test: ").strip()
    print("\nAll inputs validated. Simulating NFA...\n")
    nfa_simulate(states, transitions, start_state, final_states, input_string)

if __name__ == "__main__":
    main()
