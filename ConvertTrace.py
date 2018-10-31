import sys


def formatTrace(trace):
    out = []
    out.append(20000)
    out.append(0)
    out.append(0)
    out.append(1)

    num_blocks = 0

    blocks = {}
    for command, *args in trace:
        if command == 'a':
            blocks[args[1]] = num_blocks
            out.append(f'a {blocks[args[1]]} {args[0]}')
            num_blocks += 1
        elif command == 'r':
            if args[0] == '0x0':
                blocks[args[2]] = num_blocks
                num_blocks += 1
            else:
                blocks[args[2]] = blocks[args[0]]
            out.append(f'r {blocks[args[2]]} {args[1]}')
        elif command == 'f' and args[0] != '0x0' and blocks[args[0]] != 'freed':
            out.append(f'f {blocks[args[0]]}')
            blocks[args[0]] = 'freed'

    out[1] = num_blocks
    out[2] = len(trace)

    return out


def readPYTrace():
    trace = []
    for line in sys.stdin:
        [command, *args] = line.strip().split(' ')
        if command:
            command = command[-1]
            trace.append([command, *args])
    return trace


def main():
    rawTrace = readPYTrace()
    for line in formatTrace(rawTrace):
        print(line)


if __name__ == '__main__':
    main()
