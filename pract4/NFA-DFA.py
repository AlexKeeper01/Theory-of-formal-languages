states1 = []
states2 = []
alphabet = []
state_transitions1 = []
state_transitions2 = []
initial_states1 = []
initial_states2 = []
final_states1 = []
final_states2 = []

def input_state():
    states1.extend(input("Enter set of states: ").split(' '))
    print()
    alphabet.extend(input("Enter the input alphabet: ").split(' '))
    print()
    temp = input("Enter state-transitions function (current state, input character, next state): ").split(' ')
    for i in temp:
        state_transitions1.append(tuple(i.strip("()").split(',')))
    print()
    initial_states1.extend(input("Enter a set of initial states: ").split(' '))
    print()
    final_states1.extend(input("Enter a set of final states: ").split(' '))
    print()


def calculate():
    t = ""
    for i in initial_states1:
        t = t + i
    initial_states2.append(t)
    states2.append(t)
    for o in states2:
        for k in alphabet:
            m = set()
            for j in o:
                for l in state_transitions1:
                    if l[0] == j and l[1] == k:
                        m.add(l[2])
            d = ""
            for h in sorted(m):
                d += h
            state_transitions2.append((o, k, d))
            if d not in states2 and d != "":
                states2.append(d)


def get_result():
    print("DFA:")
    print("Set of states: " + ' '.join(states2) + '\n')
    print("Input alphabet: " + ' '.join(alphabet) + '\n')
    print("State-transitions function:")
    for i in state_transitions2:
        print(str(i))
    print()
    print("Initial states: " + ' '.join(initial_states2) + '\n')
    print("Final states: ", end=' ')
    for i in states2:
        for j in final_states1:
            j in i and print(i, end=' ')

def main():
    input_state()
    calculate()
    get_result()

if __name__ == "__main__":
    main()
