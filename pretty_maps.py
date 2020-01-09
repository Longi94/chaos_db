import argparse

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-p', '--process', type=str, required=True)
    args = parser.parse_args()

    maps = f'/proc/{args.process}/maps'

    with open(maps) as f:
        for line in f:
            line = line.strip()
            parts = list(filter(lambda x: len(x) > 0, line.split(' ')))

            addr_start, addr_end = tuple(parts[0].split('-'))
            size = int(addr_end, 16) - int(addr_start, 16)
            name = '[anon]' if len(parts) < 6 else parts[5]
            print('{0: <33} {1: <4} {2: <10} {3}'.format(f'{addr_start}-{addr_end}', parts[1], size, name))
